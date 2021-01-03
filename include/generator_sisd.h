/*
* NAME: Copyright (c) 2021, Biren Patel
* DESC: Subroutines for SISD psuedo random number generation
* LICS: MIT License
*/

#ifndef SPK_GENERATOR_SISD_H
#define SPK_GENERATOR_SISD_H

#include "scipack_config.h"

#include <stdint.h>

/*******************************************************************************
* DESC: list of available random number generators
* @ SPK_GENERATOR_PCG64i : PCG 64-bit insecure by Melissa O'Neill
* @ SPK_GENERATOR_XSH64 : Xorshift 64-bit by George Marsaglia
* @ SPK_GENERATOR_LCG128 : Linear Congruential Generator 128/64 output
*******************************************************************************/
#define SPK_GENERATOR_PCG64i        0x140
#define SPK_GENERATOR_XSH64         0x240
#define SPK_GENERATOR_LCG128        0x340
#define SPK_GENERATOR_DEFAULT       SPK_GENERATOR_PCG64i

/*******************************************************************************
* NAME: struct spk_generator
* DESC: abstract interface between some generator and the end user
* @ spk_next : raw generator output
* @ spk_rand : bounded random integers in [L, H] inclusive
* @ spk_bias : output i.i.d biased bits with probability p = N/2^M, M <= 64
* @ spk_unid : uniform variates
*******************************************************************************/
struct spk_generator
{
    int (*next) (struct spk_generator *, uint64_t *, size_t);
    int (*rand) (struct spk_generator *, uint64_t *, size_t, uint64_t L, uint64_t H);
    int (*bias) (struct spk_generator *, uint64_t *, size_t, uint64_t N, int M);
    int (*unid) (struct spk_generator *, double *, size_t);
    uint64_t state[];
};

/*******************************************************************************
* NAME: spk_GeneratorNew
* DESC: initialize and seed some pseudo random number generator
* OUTP: spk_generator
* @ identifier : see list of available generators
* @ seed : pass zero for non-deterministic seeding
*******************************************************************************/
struct spk_generator *spk_GeneratorNew(int identifier, uint64_t seed);

/*******************************************************************************
* NAME: spk_GeneratorDelete
* DESC: release system resources
* OUTP: spk_generator will point to NULL hereafter
*******************************************************************************/
void spk_GeneratorDelete(struct spk_generator **rng);


/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

/*******************************************************************************
* Error Codes
*******************************************************************************/
enum RANDOM_SISD_ERROR_CODES
{
    RANDOM_SISD_SUCCESS              = 0,
    RANDOM_SISD_RDRAND_FAIL          = 1,
    RANDOM_SISD_MALLOC_FAIL          = 2,
    RANDOM_SISD_UNDEFINED_ERROR      = 3,
};

const char *random_sisd_error_desc(int error);

/*******************************************************************************
* NAME: u(128/64/32)_generator
* DESC: abstract interface for psuedo random number generator
* OUTP: this object must be returned by random_sisd_init_* prior to use
*******************************************************************************/
typedef struct u64_generator *u64_generator;

struct u64_generator
{
    void *state;
    uint64_t (*next) (void *state);
    void (*free) (u64_generator self);
    uint64_t (*randint) (u64_generator self, const uint64_t min, const uint64_t max);
    uint64_t (*bernoulli) (u64_generator self, const uint64_t n, const int m);
    uint64_t (*binomial) (u64_generator self, uint64_t k, const uint64_t n, const int m);    
};

/*******************************************************************************
* NAME: random_sisd_init_*
* DESC: initialize a statistical psuedo random number generator
* OUTP: null on error, check error argument for details
* NOTE: nonzero seeds are deterministically hashed for increased entropy
* @ seed : zero for non-deterministic seeding
* @ error : can be passed as null, else one of enum RANDOM_SISD_ERROR_CODES
*******************************************************************************/
u64_generator random_sisd_init_pcg64_insecure(uint64_t seed, int *error);
u64_generator random_sisd_init_xorshift64(uint64_t seed, int *error);

#endif

/*******************************************************************************
TODO: move this to docs

* @ state : internal state of the generator initialized by random_init_*

* @ next  : generate raw random integers directly from the underlying PRNG
    * @ state : self internal state
    
* @ rint  : unbiased integers between min and max inclusive
    * @ min : lower bound inclusive
    * @ max : upper bound inclusive
    
* @ bern  : generate 64 iid bernoulli trials with probability p = n/(2^m)
    * @ n : numerator of p = n/(2^m) where 0 < n < 2^m
    * @ m : denominator of p = n/(2^m) where 0 < m <= 64
    
* @ bino  : sample from a binomial distribution X~(k, p) where p = n/(2^m)
    * @ k : total trials
    * @ n : numerator of p = n/(2^m) where 0 < n < 2^m
    * @ m : denominator of p = n/(2^m) where 0 < m <= 64
*******************************************************************************/
