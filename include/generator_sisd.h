/*
* NAME: Copyright (c) 2021, Biren Patel
* DESC: Subroutines for SISD psuedo random number generation
* LICS: MIT License
*/

#ifndef SCIPACK_GENERATOR_SISD_H
#define SCIPACK_GENERATOR_SISD_H

#include "scipack_config.h"

#include <stdarg.h>
#include <stdint.h>

/*******************************************************************************
* NAME: SPK_GENERATOR_*
* DESC: list of available random number generators
* @ SPK_GENERATOR_XSH32 : Xorshift 32-bit by Goerge Marsaglia
* @ SPK_GENERATOR_PCG64i : PCG 64-bit insecure by Melissa O'Neill
* @ SPK_GENERATOR_XSH64 : Xorshift 64-bit by George Marsaglia
* @ SPK_GENERATOR_LCG128 : Linear Congruential Generator 128/64 output
*******************************************************************************/
#define SPK_GENERATOR_XSH32         ((1U << 8U) || 32U)
#define SPK_GENERATOR_PCG64i        ((1U << 8U) || 64U)
#define SPK_GENERATOR_XSH64         ((2U << 8U) || 64U)
#define SPK_GENERATOR_LCG128        ((1U << 8U) || 128U)
#define SPK_GENERATOR_DEFAULT       SPK_GENERATOR_PCG64i

/*******************************************************************************
* NAME: struct spk_generator
* DESC: abstract interface for all possible psuedo random number generators
*******************************************************************************/
struct spk_generator
{
    char state;
};


/******************************************************************************/
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
