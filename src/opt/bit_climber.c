/*
* NAME: Copyright (C) 2021, Biren Patel
* DESC: Single real loss minimization via bit-encoded hill climbing heuristic
* LICS: MIT License
*/

#include "bit_climber.h"

#include <string.h>

/*******************************************************************************
spk_bit_climber
* @ rng : used to explore search space
* @ ofunc : objective function to minimize
* @ value : loss value of current candidate
* @ len : total bit length of candidate (user len rounded up to multiple of 64)
* @ candidate : the current and best candidate found within the search space
*******************************************************************************/
struct spk_bit_climber
{
    spk_generator rng;
    spk_BitClimberObjFunc ofunc;
    double value;
    size_t len;
    uint64_t candidate[];
};

#define SIZEOF_BIT_CLIMBER (sizeof(struct spk_bit_climber))

//to round up user length for flexible array member
#define MULTIPLE_OF(x, y) (y += ((x - (y & (x-1))) & (x-1)))

/*******************************************************************************
spk_BitClimberNew, configure all aspects of the bit climber in preparation for
a minimization call.
*******************************************************************************/

int spk_BitClimberNew
(
    spk_bit_climber *cmb_ref,
    spk_BitClimberObjFunc ofunc,
    int rng_type,
    uint64_t seed,
    size_t len,
    const void *init_candidate
)
{    
    int error = SPK_ERROR_UNDEFINED;
    spk_bit_climber cmb = *cmb_ref;
    
    //argument checking
    if (len == 0) return SPK_ERROR_ARGBOUNDS;
    if (!ofunc) return SPK_ERROR_NULLINPUT;
    
    //scale up requested length to a multiple of 64 (for rng bias calls)
    size_t adjusted_len = MULTIPLEOF(64, len);
    if (adjusted_len < len) return SPK_ERROR_OVERFLOW;
    
    cmb = malloc(SIZEOF_BIT_CLIMBER + adjusted_len);
    if (!cmb) return SPK_ERROR_STDMALLOC;
    
    //preliminary members
    cmb->ofunc = ofunc;
    cmb->len = adjusted_len;
    
    //hook the random number generator
    spk_generator rng;
    error = spk_GeneratorNew(&rng, rng_type, seed);
    if (error) return error;
    cmb->rng = rng;
    
    //initialize a candidate
    if (init_candidate)
    {
        memcpy(cmb->candidate, init_candidate, MULTIPLEOF(8, len)/8);
    }
    else
    {
        error = rng->next(rng->state, cmb->candidate, cmb->len);
    }
    
    cmb->value = ofunc(cmb->candidate);
    
    //update pass by ref
    *cmb_ref = cmb;
    return SPK_ERROR_SUCCESS;
}

/*******************************************************************************
spk_BitClimberDelete, nothing special just release the rng and then the climber
*******************************************************************************/

void spk_BitClimberDelete(spk_bit_climber cmb)
{
    spk_GeneratorDelete(cmb->rng);
    free(cmb);
    return;
}