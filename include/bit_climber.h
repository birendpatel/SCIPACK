/*
* NAME: Copyright (C) 2021, Biren Patel
* DESC: Single real loss minimization via bit-encoded hill climbing heuristic
* LICS: MIT License
*/

#ifndef SPK_BIT_CLIMBER
#define SPK_BIT_CLIMBER

#include "generator_sisd.h"
#include "scipack_config.h"

#include <stdint.h>
#include <stdlib.h>

/*******************************************************************************
* NAME: spk_bit_climber
* DESC: bit climber handle, must be initialized via spk_BitClimberNew
*******************************************************************************/
typedef struct spk_bit_climber *spk_bit_climber;

/*******************************************************************************
* NAME: spk_BitClimberObjFunc
* DESC: prototype satisfying bit climber loss function requirements
* OUTP: loss value of candidate after evaluation
* @ candidate : bit stream, guaranteed at least length n used on initialization
*******************************************************************************/
typedef double (*spk_BitClimberObjFunc)(const void *candidate);

/*******************************************************************************
* NAME: spk_BitClimberNew
* DESC: initialize an opaque bit climber
* OUTP: scipack error code
* @ cmb : pointer to previously defiend spk_bit_climber
* @ loss : minimization objective
* @ rng_type : see generator_sisd list of available random number generators
* @ seed : rng seed
* @ len : bit length of candidate (minimum but not necessarily exact length)
* @ init_candidate : starting candidate value, or null for random initialization
*******************************************************************************/
int spk_BitClimberNew
(
    spk_bit_climber *cmb,
    spk_BitClimberObjFunc ofunc,
    int rng_type,
    uint64_t seed,
    size_t len,
    const void *init_candidate
);

/*******************************************************************************
* NAME: spk_BitClimberDelete
* DESC: free system resources, the bit climber cannot be used hereafter
*******************************************************************************/
void spk_BitClimberDelete(spk_bit_climber cmb);

/*******************************************************************************
* NAME: spk_BitClimberMinimize
* DESC: minimize the loss function provided in prior call to spk_BitClimberNew
* OUTP: scipack error code
* @ max_iter : maximum number of evolution iterations of candidate before halt
* @ solution : contains loss value of current (best) candidate on halt
*******************************************************************************/
int spk_BitClimberMinimize
(
    size_t max_iter,
    double *solution;
);

#endif
