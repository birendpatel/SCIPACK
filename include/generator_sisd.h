/*
* NAME: Copyright (c) 2021, Biren Patel
* DESC: Subroutines for SISD psuedo random number generation
* NOTE: Low level subroutines only, use probability module for high level API
* LICS: MIT License
*/

#ifndef SPK_GENERATOR_SISD_H
#define SPK_GENERATOR_SISD_H

#include "scipack_config.h"

#include <stddef.h> //size_t
#include <stdint.h> //uint64_t

/*******************************************************************************
* DESC: list of available random number generators
* @ SPK_GENERATOR_PCG64i : PCG 64-bit insecure by Melissa O'Neill
* @ SPK_GENERATOR_XSH64 : Xorshift 64-bit by George Marsaglia
*******************************************************************************/
#define SPK_GENERATOR_PCG64i        0x140
#define SPK_GENERATOR_XSH64         0x240
#define SPK_GENERATOR_DEFAULT       SPK_GENERATOR_PCG64i

/*******************************************************************************
* NAME: struct spk_generator
* DESC: abstract interface between some generator and the end user
* @ next : raw generator output, state points to self->state
* @ rand : bounded random integers in [L, H] inclusive
* @ bias : iid biased bits with probability p = N/2^M, M <= 64, 0 < n < 2^m
* @ unid : uniform variates of type double along the unit interval
* @ state : internal generator state
*******************************************************************************/
typedef struct spk_generator *spk_generator;

struct spk_generator
{
    int (*next)
    (
        uint64_t *state,
        uint64_t *dest,
        const size_t n
    );
    
    int (*rand)
    (
        spk_generator rng,
        uint64_t *dest,
        const size_t n,
        const uint64_t min,
        const uint64_t max
    );
    
    int (*bias)
    (
        spk_generator rng,
        uint64_t *dest,
        const size_t n,
        const double p,
        const int exp
    );
    
    int (*unid)
    (
        spk_generator rng,
        double *dest,
        const size_t n
    );
    
    uint64_t state[];
};

/*******************************************************************************
* NAME: spk_GeneratorNew
* DESC: initialize and seed some pseudo random number generator
* OUTP: scipack error code
* @ identifier : see list of available generators
* @ seed : pass zero for non-deterministic seeding
*******************************************************************************/
int spk_GeneratorNew(spk_generator *rng, int identifier, uint64_t seed);

/*******************************************************************************
* NAME: spk_GeneratorDelete
* DESC: release system resources
*******************************************************************************/
void spk_GeneratorDelete(spk_generator rng);

#endif
