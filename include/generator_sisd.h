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
* @ spk_next : raw generator output
* @ spk_rand : bounded random integers in [L, H] inclusive
* @ spk_bias : iid biased bits with probability p = N/2^M, M <= 64, 0 < n < 2^m
* @ spk_unid : uniform variates of type double along the unit interval
*******************************************************************************/
struct spk_generator
{
    int (*next) (struct spk_generator *, uint64_t *, size_t);
    int (*rand) (struct spk_generator *, uint64_t *, size_t, uint64_t L, uint64_t H);
    int (*bias) (struct spk_generator *, uint64_t *, size_t, size_t N, size_t M);
    int (*unid) (struct spk_generator *, double *, size_t);
    uint64_t buffer[];
};

/*******************************************************************************
* NAME: spk_GeneratorNew
* DESC: initialize and seed some pseudo random number generator
* OUTP: scipack error code
* @ identifier : see list of available generators
* @ seed : pass zero for non-deterministic seeding
*******************************************************************************/
int spk_GeneratorNew(struct spk_generator **rng, int identifier, uint64_t seed);

/*******************************************************************************
* NAME: spk_GeneratorDelete
* DESC: release system resources
* OUTP: spk_generator will point to NULL hereafter
*******************************************************************************/
void spk_GeneratorDelete(struct spk_generator **rng);

#endif
