/*
* NAME: Copyright (C) 2021, Biren Patel
* DESC: Unit tests for include/timer.h
* LICS: MIT License
*/

#include "timer.h"
#include "unity.h"

#include <time.h>

/*******************************************************************************
time.h nanosleep is used for tests that don't mock the elapsed time. so, in
these scenarios, a small millisecond delay occurs after the sleep terminates. To
account for this delay, a +/- 5% default delta is used. modify the threshold if
necessary.
*/

#define THRESHOLD 0.05
#define DELTA(x) (x * THRESHOLD)


/******************************************************************************/

void test_request_for_frequency_is_nonzero(void)
{
    //act
    unsigned long long freq = spk_TimerGetFrequency();
    
    //assert
    TEST_ASSERT_TRUE(freq != 0);
}

/******************************************************************************/

void test_timer_for_seconds_task(void)
{
    //arrange
    struct timespec ts = {1, 500000000};
    int error = 0;
    struct spk_timer_result tr;
    
    //act
    spk_TimerStart();
    error = nanosleep(&ts, NULL);
    spk_TimerStop();
    tr = spk_TimerElapsedTime(spk_TimerElapsedCycles());
    
    //assert
    TEST_ASSERT_EQUAL_INT(0, error);
    TEST_ASSERT_DOUBLE_WITHIN(DELTA(1.5), 1.5, tr.elapsed);
    TEST_ASSERT_EQUAL_INT(SPK_TIMER_SECONDS, tr.resolution);
    TEST_ASSERT_EQUAL_STRING("sec", tr.symbol);
}

/******************************************************************************/

void test_timer_for_milliseconds_task(void)
{
    //arrange
    struct timespec ts = {0, 500000000};
    int error = 0;
    struct spk_timer_result tr;
    
    //act
    spk_TimerStart();
    error = nanosleep(&ts, NULL);
    spk_TimerStop();
    tr = spk_TimerElapsedTime(spk_TimerElapsedCycles());
    
    //assert
    TEST_ASSERT_EQUAL_INT(0, error);
    TEST_ASSERT_DOUBLE_WITHIN(DELTA(500), 500, tr.elapsed);
    TEST_ASSERT_EQUAL_INT(SPK_TIMER_MILLISECONDS, tr.resolution);
    TEST_ASSERT_EQUAL_STRING("ms", tr.symbol);
}

/*******************************************************************************
Because of post-nanosleep delay in the CPU returning to the calling thread, we
mock 100 us of elapsed time between t1 and t2.
*/

void test_timer_for_microseconds_task(void)
{
    //arrange
    double ticks_per_100us = (double) spk_TimerGetFrequency() * 0.0001;
    spk_TimerStart();
    spk_TimerStop();
    spk_tsc_start = 0;
    spk_tsc_end = (unsigned long long) ticks_per_100us;
    struct spk_timer_result tr;
    tr = spk_TimerElapsedTime(spk_TimerElapsedCycles());
    
    //assert
    TEST_ASSERT_DOUBLE_WITHIN(DELTA(100), 100, tr.elapsed);
    TEST_ASSERT_EQUAL_INT(SPK_TIMER_MICROSECONDS, tr.resolution);
    TEST_ASSERT_EQUAL_STRING("us", tr.symbol);
}

/*******************************************************************************
Mock at 900 nanoseconds
*/

void test_timer_for_nanoseconds_task(void)
{
    //arrange
    double ticks_per_900ns = (double) spk_TimerGetFrequency() * 0.0000009;
    spk_TimerStart();
    spk_TimerStop();
    spk_tsc_start = 0;
    spk_tsc_end = (unsigned long long) ticks_per_900ns;
    struct spk_timer_result tr;
    tr = spk_TimerElapsedTime(spk_TimerElapsedCycles());
    
    //assert
    TEST_ASSERT_DOUBLE_WITHIN(DELTA(900), 900, tr.elapsed);
    TEST_ASSERT_EQUAL_INT(SPK_TIMER_NANOSECONDS, tr.resolution);
    TEST_ASSERT_EQUAL_STRING("ns", tr.symbol);
}

/******************************************************************************/

int main(void)
{
    UNITY_BEGIN();
        RUN_TEST(test_request_for_frequency_is_nonzero);
        RUN_TEST(test_timer_for_seconds_task);
        RUN_TEST(test_timer_for_milliseconds_task);
        RUN_TEST(test_timer_for_microseconds_task);
        RUN_TEST(test_timer_for_nanoseconds_task);
    return UNITY_END();
}
