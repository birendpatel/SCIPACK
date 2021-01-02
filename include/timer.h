/*
* NAME: Copyright (C) 2020, Biren Patel
* DESC: Precision timer API for elapsed-time microbenchmarking via TSC
* LICS: MIT License
*/

#ifndef SCIPACK_TIMER_H
#define SCIPACK_TIMER_H

#include "scipack_config.h"

#include <x86intrin.h>

/*******************************************************************************
* NAME: enum timer_resolution
* DESC: all possible resolutions used by API for elapsed wall measurements
*******************************************************************************/
enum timer_resolution
{
    TIMER_SECONDS        = 0,
    TIMER_MILLISECONDS   = 1,
    TIMER_MICROSECONDS   = 2,
    TIMER_NANOSECONDS    = 3,
};

/*******************************************************************************
* NAME: TimerStart
* DESC: fence instructions and record the initial time stamp counter
*******************************************************************************/
#define TimerStart()                                                           \
        unsigned long long tsc_start = 0;                                      \
        unsigned long long tsc_end = 0;                                        \
        unsigned int tsc_dummy = 0;                                            \
        _mm_mfence();                                                          \
        _mm_lfence();                                                          \
        tsc_start = __rdtsc()
        
/*******************************************************************************
* NAME: TimerStop
* DESC: record the terminating time stamp counter with a fence
*******************************************************************************/
#define TimerStop()                                                            \
        tsc_end = __rdtscp(&tsc_dummy);                                        \
        _mm_lfence()
        
/*******************************************************************************
* NAME: TimerElapsedCycles
* DESC: the total elapsed cycles (ticks) between TimerStart and TimerStop
* OUTP: unsigned long long
*******************************************************************************/
#define TimerElapsedCycles() (tsc_end - tsc_start)

/*******************************************************************************
* NAME: struct timer_result
* DESC: returned by TimerElapsedTime
* @ elapsed : time domain transformation of total cycles
* @ resolution : unit of the elapsed timem
* @ symbol : null-terminated string representing resolution as sec/ms/us/ns
*******************************************************************************/
typedef struct timer_result
{
    double elapsed;
    enum timer_resolution resolution;
    char symbol[4];
} timer_result;

/*******************************************************************************
* NAME: TimerElapsedTime
* DESC: convert total elapsed cycles to an estimated elapsed wall time
* OUTP: struct timer_result
* NOTE: Typically called as TimerElapsedTime(TimerElapsedCycles())
*******************************************************************************/
timer_result TimerElapsedTime(unsigned long long cycles);

/*******************************************************************************
* NAME: TimerGetHz
* DESC: Get the estimated frequency of the underlying time stamp counter
* OUTP: in units Hertz
* NOTE: On first call, this function may take > 10 seconds to execute
*******************************************************************************/
unsigned long long TimerGetHz(void);

#endif
