/*
* NAME: Copyright (c) 2021, Biren Patel
* DESC: subroutines for SISD psuedo random number generation
* LICS: MIT License
*/

#include "generator_sisd.h"

#include <assert.h>
#include <immintrin.h>
#include <stdlib.h>

/*******************************************************************************
Prototypes
*******************************************************************************/

static uint64_t Hash(uint64_t *value);
static int RdRandRetry(uint64_t *x, size_t limit);

static int Pcg64iNew(struct spk_generator **rng, uint64_t seed);
static int Xsh64New(struct spk_generator **rng, uint64_t seed);

static int Pcg64iNext(struct spk_generator *rng, uint64_t *dest, size_t n);
static int Xsh64Next(struct spk_generator *rng, uint64_t *dest, size_t n);

static int UnbiasedRandInt(struct spk_generator *, uint64_t *, size_t, uint64_t, uint64_t);
static int BernoulliVector(struct spk_generator *, uint64_t *, size_t, size_t, size_t);

/*******************************************************************************
Sebastiano Vigna's version of Java SplittableRandom. This is used as a one-off 
mixing function for seeding, so the state increment from Vigna's original code
is removed in favor of an overwriting call by reference. For references, see:
http://xoshiro.di.unimi.it/splitmix64.c, http://prng.di.unimi.it/splitmix64.c.
*******************************************************************************/

static uint64_t Hash(uint64_t *value)
{    
    uint64_t i = *value;
    
    i ^=  i >> 30;
    i *= 0xbf58476d1ce4e5b9ULL;
    i ^= i >> 27;
    i *= 0x94d049bb133111ebULL;
    i ^= i >> 31;
    
    *value = i;
    
    return i;
}

/*******************************************************************************
The rdrand instruction is used to seed all generators when non-deterministic
behavior is requested. Per Intel documentation, the rdrand instruction must be 
retried ten times on the rare chance that underflow occurs. The limit param is
available if AMD guidelines differ. The ULL cast is to silence some GCC warnings
since uint64_t is an unsigned long on some machines.

TODO: see concerns about rdrand here, we may want to replace it with something
else. https://cr.yp.to/talks/2014.05.16/slides-dan+tanja-20140516-4x3.pdf
perhaps an internal LCG which uses a high-res counter coupled with ASLR.
*******************************************************************************/

static int RdRandRetry(uint64_t *x, size_t limit)
{    
    for (size_t i = 0; i < limit; i++)
    {
        if (_rdrand64_step((unsigned long long *) x))
        {
            return SPK_ERROR_SUCCESS;
        }
    }
    
    return SPK_ERROR_RDRAND;
}

/*******************************************************************************
Internal state of pcg 64-bit insecure, corresponding to SPK_GENERATOR_PCG64i
*******************************************************************************/

struct pcg64i
{
    uint64_t state;
    uint64_t increment;
};

/*******************************************************************************
Internal state of xorshift 64-bit, corresponding to SPK_GENERATOR_XSH64
*******************************************************************************/

struct xsh64
{
    uint64_t state;
};

/*******************************************************************************
Dynamic allocation helpers
*******************************************************************************/

#define SIZEOF_INTERFACE (sizeof(struct spk_generator))
#define SIZEOF_PCG64I (sizeof(struct pcg64i))
#define SIZEOF_XSH64 (sizeof(struct xsh64))

/*******************************************************************************
Dynamically allocate a new random number generator
*******************************************************************************/

int spk_GeneratorNew(struct spk_generator **rng, int identifier, uint64_t seed)
{
    switch (identifier)
    {
        case SPK_GENERATOR_PCG64i:
            return Pcg64iNew(rng, seed);
            break;
            
        case SPK_GENERATOR_XSH64:
            return Xsh64New(rng, seed);
            break;
            
        default:
            return SPK_ERROR_ARGBOUNDS;
    }
}

/*******************************************************************************
Initialize a pcg64i generator
*******************************************************************************/

static int Pcg64iNew(struct spk_generator **rng, uint64_t seed)
{
    *rng = malloc(SIZEOF_INTERFACE + SIZEOF_PCG64I);
    if (!(*rng)) return SPK_ERROR_STDMALLOC;
    
    struct pcg64i *pcg = (struct pcg64i *) (*rng)->buffer;
    
    if (seed != 0)
    {
       pcg->state = Hash(&seed);
       pcg->increment = Hash(&seed);
    }
    else
    {
        int error = SPK_ERROR_UNDEFINED;
        
        error = RdRandRetry(&pcg->state, 10);
        if (error) return error;
        
        error = RdRandRetry(&pcg->increment, 10);
        if (error) return error;
    }
    
    //PCG increment must be odd
    pcg->increment |= 1;
    
    //hook in methods
    (*rng)->next = Pcg64iNext;
    (*rng)->rand = UnbiasedRandInt;
    (*rng)->bias = BernoulliVector;
    (*rng)->unid = NULL;
    
    return SPK_ERROR_SUCCESS;
}

/*******************************************************************************
Initialize a xsh64 generator
*******************************************************************************/

static int Xsh64New(struct spk_generator **rng, uint64_t seed)
{
    *rng = malloc(SIZEOF_INTERFACE + SIZEOF_XSH64);
    if (!(*rng)) return SPK_ERROR_STDMALLOC;
    
    struct xsh64 *xsh = (struct xsh64 *) (*rng)->buffer;
    
    if (seed != 0)
    {
       xsh->state = Hash(&seed);
    }
    else
    {
        int error = SPK_ERROR_UNDEFINED;
        
        error = RdRandRetry(&xsh->state, 10);
        if (error) return error;
    }
    
    //hook in methods
    (*rng)->next = Xsh64Next;
    (*rng)->rand = UnbiasedRandInt;
    (*rng)->bias = BernoulliVector;
    (*rng)->unid = NULL;
    
    return SPK_ERROR_SUCCESS;
}

/*******************************************************************************
Free a random number generator, set handle to NULL
*******************************************************************************/

void spk_GeneratorDelete(struct spk_generator **rng)
{
    free(*rng);
    *rng = NULL;
}

/*******************************************************************************
The following function is originally Copyright 2014 Melissa O'Neill, which is
licensed under the Apache License, Version 2.0. PCG was licensed under the 
Apache License, Version 2.0 (the "License"); You may obtain a copy of the 
License at http://www.apache.org/licenses/LICENSE-2.0

This block of code is the default insecure 64-bit output PCG denoted commonly 
as pcg_output_rxs_m_xs_64_64. This function exhibits the following modifications
from the original source written by Melissa O'Neill.

All credit for the design, theory, and innovations of the PCG family is due to
Melissa O'Neill, you can find a copy of the original source at her website,
https://www.pcg-random.org/

1. The original seeding mechanism via either dev/urandom or address space
layout randomization has been replaced with x86 rdrand and/or seed hashing.

2. The preprocssor macros from O'Neill have been replaced, decimal constants
were swapped for hex, and variable names have been changed. These modifications
are made to engender simplicity.

3. A data buffer is filled to reduce latency due to static library usage, since
link time optimization alone does not result in sufficient speed gains.
*/

static int Pcg64iNext(struct spk_generator *rng, uint64_t *dest, size_t n)
{
    struct pcg64i *pcg = (struct pcg64i *) rng->buffer;    
    uint64_t permuted_state = 0;
    
    for (size_t i = 0; i < n; i++)
    {
        //permute the current state
        permuted_state = pcg->state >> 59ULL;
        permuted_state += 5ULL;
        permuted_state = pcg->state >> permuted_state;
        permuted_state ^= pcg->state;
        permuted_state *= 0xAEF17502108EF2D9ULL;
        permuted_state ^= (permuted_state >> 43ULL);
        
        //update internal state
        pcg->state = pcg->state * 0x5851F42D4C957F2DULL + pcg->increment;
        
        //track output
        dest[i] = permuted_state;
    }
    
    return SPK_ERROR_SUCCESS;
}


/*******************************************************************************
* xorshift 64-bit by George Marsaglia
*******************************************************************************/

static int Xsh64Next(struct spk_generator *rng, uint64_t *dest, size_t n)
{
    struct xsh64 *xsh = (struct xsh64 *) rng->buffer;
    
    for (size_t i = 0; i < n; i++)
    {
        xsh->state ^= xsh->state << 13;
        xsh->state ^= xsh->state >> 7;
        xsh->state ^= xsh->state << 17;
        
        dest[i] = xsh->state;
    }
    
    return SPK_ERROR_SUCCESS;
}

/*******************************************************************************
random integers, aka discrete uniform variates. This is an unbiased variant via
rejection sampling. It includes a variable lower bound.

TODO: attempt to use remaining upper bits after rejection.
*/

static int UnbiasedRandInt
(
    struct spk_generator *rng,
    uint64_t *dest,
    size_t n,
    uint64_t min,
    uint64_t max
)
{
    uint64_t outp = 0;
    uint64_t ceil = max - min;
    uint64_t mask = ~((uint64_t) 0) >> __builtin_clzll(ceil);
    
    for (size_t i = 0; i < n; i++)
    {
        do
        {
            rng->next(rng, &outp, 1);
            outp = outp & mask;
        }
        while (outp > ceil);
        
        dest[i] = outp + min;
    }
    
    return SPK_ERROR_SUCCESS;;
}

/*******************************************************************************
This function uses a virtual machine to simultaneously generate 64 iid bernoulli
trials without the SIMD instruction set. I wrote a short essay at the following
url: https://stackoverflow.com/questions/35795110/ (username Ollie) which uses
256 bit resolution to demonstrate the main concepts.

To give some additional detail on why this works, consider a binary tree with
double values where all left children are half the value of the parent, and all
right children are P + (.5 - .5P). With a root value of 0.5, the traversal path
taken to a desired probability maps precisely to a compact binary representation
when AND=0 and OR=1.

                                    ORI
                                     |
                                     v
                                    0.5
                                     |
                  _______AND_________|________OR________
                 |                                      |
                .25                                    .75
                 |                                      |
      ____AND____|____OR____                 ____AND____|____OR____
     |                      |               |                      |
   .125                   .625            .325                   .875
     |                      |               |                      |
     |                      |               |                      |
    ...                    ...             ...                    ...
    
Therefore, for some p = n/(2^m), n unpacks from its compact binary and can be
read from the first nonzero LSB to MSB as the exact traversal starting at the
root node. Or, alternatively, the bit trie expansion of the numerator n gives
the very same bitcode.

So given a generator that outputs unbiased bits, we can psuedo-vectorized the
production of biased bits with at most log2(m) calls to the generator for some
resolution 2^m. Although this is more wasteful from an information theoretic
view than running an arithmetic decoder, it is almost certainly faster.

In probablity, nodes map to some event given a sequence of bernoulli trials.
i.e,. .875 is the event of at least one success. 0.625 is the event where either
the first two trials are both successful, or the final trial is successful.

TODO: additional error checking on n/2^m bounds
*/

static int BernoulliVector
(
    struct spk_generator *rng,
    uint64_t *dest,
    size_t len,
    size_t numerator,
    size_t exponent
)
{
    if (numerator == 0) return SPK_ERROR_ARGBOUNDS;
    if (exponent > 64) return SPK_ERROR_ARGBOUNDS;
    
    const size_t offset = __builtin_ctzll(numerator);
    const size_t total = exponent - offset;
    uint64_t buffer[total];
    uint64_t accumulator = 0;
    
    for (size_t i = 0; i < len; i++)
    {
        rng->next(rng, buffer, total);
        
        for (size_t j = 0; j < total; j++)
        {            
            switch (numerator & (1ULL << (j + offset)))
            {
                case 0:
                    accumulator &= buffer[j];
                    break;
                    
                case 1:
                    accumulator |= buffer[j];
                    break;
            }
        }
        
        dest[i] = accumulator;
    }
    
    return SPK_ERROR_SUCCESS;
}
