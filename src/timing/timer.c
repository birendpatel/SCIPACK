/*
* NAME: Copyright (C) 2020, Biren Patel
* DESC: Precision timer internals for elapsed-time microbenchmarking via TSC
* LICS: MIT License
*/

#include "timer.h"

#include <assert.h>
#include <stdlib.h> //qsort, size_t
#include <string.h> //strcpy
#include <time.h> //nanosleep

/*******************************************************************************
Reference units for struct timer_result to aid user in text formatting.
*/

static const char * symbol_lookup[TIMER_NANOSECONDS + 1] =
{
    [TIMER_SECONDS]         = "sec",
    [TIMER_MILLISECONDS]    = "ms",
    [TIMER_MICROSECONDS]    = "us",
    [TIMER_NANOSECONDS]     = "ns"
};

/*******************************************************************************
Use a global write-once use-everywhere variable for the TSC frequency. This is
possible since modern hardware uses a stable constant TSC locked to some unknown
reference frequency.
*/

#define INIT_HZ 0
static unsigned long long tsc_hz = INIT_HZ;

/*******************************************************************************
Estimate the TSC frequency. This is just a temporary placeholder that works just
well enough to continue development on other areas of SCIPACK. For now, we fetch
an average frequency and then subtract the average overhead due to the nanosleep
call. I haven't found a good OS-portable method for TSC frequency through online
or the Intel manuals.
*/

#define ULLCAST(x) *((const unsigned long long *) (x))
#define MAX_NANOSLEEP_ITER ((size_t) 100)
#define MAX_TICKS_ITER ((size_t) 10)

int cmp(const void *a, const void *b)
{
    unsigned long long num1 = ULLCAST(a), num2 = ULLCAST(b);
    
    if (num1 < num2) return -1;
    else if (num1 == num2) return 0;
    else return 1;
}

static void TimerSetHz(void)
{
    if (tsc_hz != INIT_HZ) return;
    
    //establish median nanosleep overhead    
    unsigned long long overhead_data[MAX_NANOSLEEP_ITER] = {0ULL};
    unsigned long long overhead = 0ULL;
    
    struct timespec ts = {0, 1};
    
    for (size_t i = 0; i < MAX_NANOSLEEP_ITER; i++)
    {
        TimerStart();
        nanosleep(&ts, NULL);
        TimerStop();
        
        assert(TimerElapsedCycles() > 0 && "rdtsc delta is negative");
        
        overhead_data[i] = TimerElapsedCycles();
    }
    
    qsort(overhead_data, MAX_NANOSLEEP_ITER, sizeof(unsigned long long), cmp);
    overhead = overhead_data[MAX_NANOSLEEP_ITER/2];
    
    //establish ticks across 1 second, without nanosleep overhead
    unsigned long long ticks_data[MAX_TICKS_ITER] = {0ULL};
    
    ts.tv_sec = 1;
    ts.tv_nsec = 0;
    
    for (size_t i = 0; i < MAX_TICKS_ITER; i++)
    {
        TimerStart();
        nanosleep(&ts, NULL);
        TimerStop();
        
        assert(TimerElapsedCycles() > 0 && "rdtsc delta is negative");
        
        ticks_data[i] = TimerElapsedCycles() - overhead;
    }
    
    qsort(ticks_data, MAX_TICKS_ITER, sizeof(unsigned long long), cmp);
    
    //estimated frequency is just the median ticks
    tsc_hz = ticks_data[MAX_TICKS_ITER/2];
    assert(tsc_hz != INIT_HZ && "estimated TSC frequency is zero");
    assert(tsc_hz > INIT_HZ && "estimated TSC frequency is negative");
}


/*******************************************************************************
API access to estimated TSC frequency
*/

unsigned long long TimerGetHz(void)
{
    TimerSetHz();
    
    return tsc_hz;
}

/******************************************************************************/

timer_result TimerElapsedTime(unsigned long long cycles)
{
    timer_result tr;
    
    TimerSetHz();
    
    tr.elapsed = (double) cycles / (double) tsc_hz;
    tr.resolution = TIMER_SECONDS;
    
    while (tr.elapsed < 1.0)
    {
        tr.elapsed *= 1000.0;
        tr.resolution += 1;
        
        assert(tr.resolution <= TIMER_NANOSECONDS && "timer formatting");
    }
    
    strcpy(tr.symbol, symbol_lookup[tr.resolution]);
    
    return tr;
}
