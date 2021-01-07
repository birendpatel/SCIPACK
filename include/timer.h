/*
* NAME: Copyright (C) 2021, Biren Patel
* DESC: Precision timer API for elapsed-time microbenchmarking via TSC
* LICS: MIT License
*/

#ifndef SPK_TIMER_H
#define SPK_TIMER_H

#include "scipack_config.h"

#include <x86intrin.h> //fence

/*******************************************************************************
* NAME: enum spk_timer_resolution
* DESC: all possible resolutions used by API for elapsed wall measurements
*******************************************************************************/
enum spk_timer_resolution
{
    SPK_TIMER_SECONDS        = 0,
    SPK_TIMER_MILLISECONDS   = 1,
    SPK_TIMER_MICROSECONDS   = 2,
    SPK_TIMER_NANOSECONDS    = 3,
};

/*******************************************************************************
* NAME: spk_TimerStart
* DESC: fence instructions and record the initial time stamp counter
*******************************************************************************/
#define spk_TimerStart()                                                       \
        unsigned long long spk_tsc_start = 0;                                  \
        unsigned long long spk_tsc_end = 0;                                    \
        unsigned int spk_tsc_dummy = 0;                                        \
        _mm_mfence();                                                          \
        _mm_lfence();                                                          \
        spk_tsc_start = __rdtsc()
        
/*******************************************************************************
* NAME: spk_TimerStop
* DESC: record the terminating time stamp counter with a fence
*******************************************************************************/
#define spk_TimerStop()                                                        \
        spk_tsc_end = __rdtscp(&spk_tsc_dummy);                                \
        _mm_lfence()
        
/*******************************************************************************
* NAME: spk_TimerElapsedCycles
* DESC: the total elapsed cycles (ticks) between spk_TimerStart and spk_TimerStop
* OUTP: unsigned long long
*******************************************************************************/
#define spk_TimerElapsedCycles() (spk_tsc_end - spk_tsc_start)

/*******************************************************************************
* NAME: struct spk_timer_result
* DESC: returned by spk_TimerElapsedTime
* @ elapsed : time domain transformation of total cycles
* @ resolution : unit of the elapsed timem
* @ symbol : null-terminated string representing resolution as sec/ms/us/ns
*******************************************************************************/
typedef struct spk_timer_result
{
    double elapsed;
    enum spk_timer_resolution resolution;
    char symbol[4];
} spk_timer_result;

/*******************************************************************************
* NAME: spk_TimerElapsedTime
* DESC: convert total elapsed cycles to an estimated elapsed wall time
* OUTP: struct spk_timer_result
* NOTE: Typically called as spk_TimerElapsedTime(spk_TimerElapsedCycles())
*******************************************************************************/
spk_timer_result spk_TimerElapsedTime(unsigned long long cycles);

/*******************************************************************************
* NAME: spk_TimerGetFrequency
* DESC: Get the estimated frequency of the underlying time stamp counter
* OUTP: in units Hertz
* NOTE: On first call, this function may take > 10 seconds to execute
*******************************************************************************/
unsigned long long spk_TimerGetFrequency(void);

#endif
