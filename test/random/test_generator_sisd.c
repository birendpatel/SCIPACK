/*
* NAME: Copyright (C) 2021, Biren Patel
* DESC: Unit tests for src/random/generator_sisd.c
* LICS: MIT License
*/

#include "generator_sisd.h"
#include "unity.h"

#include <math.h> //inverse cosine
#include <stdlib.h> //malloc, exit_failure
#include <stdio.h> //fprintf

/******************************************************************************/

//simplify unit test readability
#define CHECK(x)                                                               \
        if ((x))                                                               \
        {                                                                      \
            fprintf(stderr, "error %s, %d, %s", __FILE__, __LINE__, __func__); \
            exit(EXIT_FAILURE);                                                \
        }                                                                      \

/*******************************************************************************
Seeding tests
*******************************************************************************/

void test_deterministic_seed_for_PCG64i(void)
{
    //arrange
    spk_generator SUT1;
    spk_generator SUT2;
    
    CHECK(spk_GeneratorNew(&SUT1, SPK_GENERATOR_PCG64i, 1));
    CHECK(spk_GeneratorNew(&SUT2, SPK_GENERATOR_PCG64i, 1));
    
    uint64_t SUT1_output[100] = {0};
    uint64_t SUT2_output[100] = {1};
    
    //act
    CHECK(SUT1->next(SUT1->state, SUT1_output, 100));
    CHECK(SUT2->next(SUT2->state, SUT2_output, 100));
    
    //assert
    TEST_ASSERT_EQUAL_UINT64_ARRAY(SUT1_output, SUT2_output, 100);
    
    //teardown
    spk_GeneratorDelete(SUT1);
    spk_GeneratorDelete(SUT2);
}


/******************************************************************************/

void test_deterministic_seed_for_XSH64(void)
{
    //arrange
    spk_generator SUT1;
    spk_generator SUT2;
    
    CHECK(spk_GeneratorNew(&SUT1, SPK_GENERATOR_XSH64, 1));
    CHECK(spk_GeneratorNew(&SUT2, SPK_GENERATOR_XSH64, 1));
    
    uint64_t SUT1_output[100] = {0};
    uint64_t SUT2_output[100] = {1};
    
    //act
    CHECK(SUT1->next(SUT1->state, SUT1_output, 100));
    CHECK(SUT2->next(SUT2->state, SUT2_output, 100));
    
    //assert
    TEST_ASSERT_EQUAL_UINT64_ARRAY(SUT1_output, SUT2_output, 100);
    
    //teardown
    spk_GeneratorDelete(SUT1);
    spk_GeneratorDelete(SUT2);
}

/*******************************************************************************
Rand tests
*******************************************************************************/

void test_bounded_random_integers_in_zero_one_stay_in_zero_one_PCG64i(void)
{
    //arrange
    spk_generator SUT;
    CHECK(spk_GeneratorNew(&SUT, SPK_GENERATOR_PCG64i, 0));
    uint64_t SUT_output[1000] = {2};
    
    //act
    CHECK(SUT->rand(SUT, SUT_output, 1000, 0, 1));
    
    //assert
    for (size_t i = 0; i < 1000; i++)
    {
        TEST_ASSERT_LESS_OR_EQUAL_UINT64(1, SUT_output[i]);
        TEST_ASSERT_GREATER_OR_EQUAL_UINT64(0, SUT_output[i]);
    }
    
    //teardown
    spk_GeneratorDelete(SUT);    
}

/******************************************************************************/

void test_bounded_random_integers_in_zero_one_stay_in_zero_one_XSH64(void)
{
    //arrange
    spk_generator SUT;
    CHECK(spk_GeneratorNew(&SUT, SPK_GENERATOR_XSH64, 0));
    uint64_t SUT_output[1000] = {2};
    
    //act
    CHECK(SUT->rand(SUT, SUT_output, 1000, 0, 1));
    
    //assert
    for (size_t i = 0; i < 1000; i++)
    {
        TEST_ASSERT_LESS_OR_EQUAL_UINT64(1, SUT_output[i]);
        TEST_ASSERT_GREATER_OR_EQUAL_UINT64(0, SUT_output[i]);
    }
    
    //teardown
    spk_GeneratorDelete(SUT);    
}

/******************************************************************************/

void test_bounded_random_integers_upper_bound_is_inclusive_PCG64i(void)
{
    //arrange
    spk_generator SUT;
    CHECK(spk_GeneratorNew(&SUT, SPK_GENERATOR_PCG64i, 0));
    uint64_t SUT_output[1000] = {0};
    uint64_t sum = 0;
    
    //act
    CHECK(SUT->rand(SUT, SUT_output, 1000, 0, 1));
    
    for (size_t i = 0; i < 1000; i++)
    {
        sum += SUT_output[i];
    }
    
    //assert
    TEST_ASSERT_LESS_OR_EQUAL_UINT64(1000, sum);
    TEST_ASSERT_GREATER_THAN_UINT64(0, sum);
    
    //teardown
    spk_GeneratorDelete(SUT);  
}

/******************************************************************************/

void test_bounded_random_integers_upper_bound_is_inclusive_XSH64(void)
{
    //arrange
    spk_generator SUT;
    CHECK(spk_GeneratorNew(&SUT, SPK_GENERATOR_XSH64, 0));
    uint64_t SUT_output[1000] = {0};
    uint64_t sum = 0;
    
    //act
    CHECK(SUT->rand(SUT, SUT_output, 1000, 0, 1));
    
    for (size_t i = 0; i < 1000; i++)
    {
        sum += SUT_output[i];
    }
    
    //assert
    TEST_ASSERT_LESS_OR_EQUAL_UINT64(1000, sum);
    TEST_ASSERT_GREATER_THAN_UINT64(0, sum);
    
    //teardown
    spk_GeneratorDelete(SUT);  
}


/******************************************************************************/

void test_bounded_with_max_limits_is_identical_to_raw_output_PCG64i(void)
{
    //arrange
    spk_generator SUT1;
    spk_generator SUT2;
    
    CHECK(spk_GeneratorNew(&SUT1, SPK_GENERATOR_PCG64i, 1));
    CHECK(spk_GeneratorNew(&SUT2, SPK_GENERATOR_PCG64i, 1));
    
    uint64_t SUT1_output[100] = {0};
    uint64_t SUT2_output[100] = {1};
    
    //act
    CHECK(SUT1->next(SUT1->state, SUT1_output, 100));
    CHECK(SUT2->rand(SUT2, SUT2_output, 100, 0, UINT64_MAX));
    
    //assert
    TEST_ASSERT_EQUAL_UINT64_ARRAY(SUT1_output, SUT2_output, 100);
    
    //teardown
    spk_GeneratorDelete(SUT1);
    spk_GeneratorDelete(SUT2);
}

/******************************************************************************/

void test_bounded_with_max_limits_is_identical_to_raw_output_XSH64(void)
{
    //arrange
    spk_generator SUT1;
    spk_generator SUT2;
    
    CHECK(spk_GeneratorNew(&SUT1, SPK_GENERATOR_XSH64, 1));
    CHECK(spk_GeneratorNew(&SUT2, SPK_GENERATOR_XSH64, 1));
    
    uint64_t SUT1_output[100] = {0};
    uint64_t SUT2_output[100] = {1};
    
    //act
    CHECK(SUT1->next(SUT1->state, SUT1_output, 100));
    CHECK(SUT2->rand(SUT2, SUT2_output, 100, 0, UINT64_MAX));
    
    //assert
    TEST_ASSERT_EQUAL_UINT64_ARRAY(SUT1_output, SUT2_output, 100);
    
    //teardown
    spk_GeneratorDelete(SUT1);
    spk_GeneratorDelete(SUT2);
}


/******************************************************************************/

void test_unid_values_stay_in_unit_interval_PCG64i(void)
{
    //arrange
    spk_generator SUT;
    CHECK(spk_GeneratorNew(&SUT, SPK_GENERATOR_PCG64i, 0));
    double SUT_output[1000] = {2.0, -2.0, 3.14159, - 3.14159, 998529.2398745452};
    
    //act
    CHECK(SUT->unid(SUT, SUT_output, 1000));
    
    //assert
    for (size_t i = 0; i < 1000; i++)
    {
        TEST_ASSERT_TRUE(SUT_output[i] >= 0.0);
        TEST_ASSERT_TRUE(SUT_output[i] <= 1.0);
    }
    
    //teardown
    spk_GeneratorDelete(SUT);
}

/******************************************************************************/

void test_unid_values_stay_in_unit_interval_XSH64(void)
{
    //arrange
    spk_generator SUT;
    CHECK(spk_GeneratorNew(&SUT, SPK_GENERATOR_XSH64, 0));
    double SUT_output[1000] = {2.0, -2.0, 3.14159, - 3.14159, 998529.2398745452};
    
    //act
    CHECK(SUT->unid(SUT, SUT_output, 1000));
    
    //assert
    for (size_t i = 0; i < 1000; i++)
    {
        TEST_ASSERT_TRUE(SUT_output[i] >= 0.0);
        TEST_ASSERT_TRUE(SUT_output[i] <= 1.0);
    }
    
    //teardown
    spk_GeneratorDelete(SUT);
}

/*******************************************************************************
Test the unid method by attempting to derive the value of pi via simulation.
This is a standard test typically used to examine random number quality. Since
we already know the internal generators are high quality, what we're really
trinyg to check here is just that unid is not broken. It would be better to mock
out the generator entirely. (consider that a complicated TODO)

The test is dead simple. With a bit of math comparing a square to an embedded
unit circle, and a little help from the pythagorean theorem, we can show that
pi is approx 4 * (# x-y pairs inside circle)/(# x-y pairs in total), in the limit
of n -> inf. 

A more efficient test can compare pairs of generator output and count up the 
coprime pairs, but its not a big deal to use the first test as long as n is 
quite large. Here we use 20 million samples, which will only take a second, and
any half-decent generator should have pi to 2 points of precision.
*/

void test_pairs_of_unid_values_can_estimate_value_of_pi_PCG64i(void)
{
    //arrange
    spk_generator SUT;
    CHECK(spk_GeneratorNew(&SUT, SPK_GENERATOR_PCG64i, 0));
    
    double *x = malloc(sizeof(double) * 10000000); CHECK(x == NULL);
    double *y = malloc(sizeof(double) * 10000000); CHECK(y == NULL);
    
    uint64_t inside = 0;
    double pi_approx = 0.0;
    
    //act (on the unit circle radius = 1)
    CHECK(SUT->unid(SUT, x, 10000000));
    CHECK(SUT->unid(SUT, y, 10000000));
    
    for (size_t i = 0; i < 10000000; i++)
    {
        if ((x[i] * x[i]) + (y[i] * y[i]) <= 1.0) inside++;
    };
    
    pi_approx = 4.0 * ((double) inside / 10000000.0);
    
    //assert
    TEST_ASSERT_DOUBLE_WITHIN(5.0E-3, acos(-1.0), pi_approx);
    
    //teardown
    spk_GeneratorDelete(SUT);
    free(x);
    free(y);
}

/******************************************************************************/

void test_pairs_of_unid_values_can_estimate_value_of_pi_XSH64(void)
{
    //arrange
    spk_generator SUT;
    CHECK(spk_GeneratorNew(&SUT, SPK_GENERATOR_XSH64, 0));
    
    double *x = malloc(sizeof(double) * 10000000); CHECK(x == NULL);
    double *y = malloc(sizeof(double) * 10000000); CHECK(y == NULL);
    
    uint64_t inside = 0;
    double pi_approx = 0.0;
    
    //act (on the unit circle radius = 1)
    CHECK(SUT->unid(SUT, x, 10000000));
    CHECK(SUT->unid(SUT, y, 10000000));
    
    for (size_t i = 0; i < 10000000; i++)
    {
        if ((x[i] * x[i]) + (y[i] * y[i]) <= 1.0) inside++;
    };
    
    pi_approx = 4.0 * ((double) inside / 10000000.0);
    
    //assert
    TEST_ASSERT_DOUBLE_WITHIN(5.0E-3, acos(-1.0), pi_approx);
    
    //teardown
    spk_GeneratorDelete(SUT);
    free(x);
    free(y);
}

/*******************************************************************************
Another complicated and long monte carlo unit test. Like the unit circle test,
we need to confirm within some delta that the bias method is actually generating
64 i.i.d (independent and identically distributed) bits. At 8 bits of resolution
we can fully check every possible input value of p, since p will be forced to
map down to a set of 256 discrete values. Starting at 0.00390625 and working
our way up in that increment, each inner loop runs its own simulation and then
checks each of the 64 bit positions. This is a LOOOONG test.
*/

void test_bias_at_all_256_probabilities_in_8bit_resolution_PCG64i(void)
{
    //arrange
    spk_generator SUT;
    CHECK(spk_GeneratorNew(&SUT, SPK_GENERATOR_PCG64i, 0));
    
    //fix the resolution to 8 bits
    const int resolution = 8;
    
    //prob array holds the expected probability on each bit on each experiment
    double px = 0.0;
    double prob[256] = {0.0};
    
    for (size_t i = 0; i < 256; i++)
    {
        prob[i] = px;
        px += 0.00390625;
    }
    
    //in each experiment use this to track the total success (1) per position
    uint64_t bitpos[64] = {0};
    
    //to speed up the most inner loop, we'll have bias fill an array in one call
    uint64_t *raw = malloc(sizeof(uint64_t) * 1000000); CHECK(raw == NULL);
    
    //act + assert
    for (size_t i = 0; i < 256; i++)
    {
        //fill for current round
        CHECK(SUT->bias(SUT, raw, 1000000, prob[i], resolution));
        
        //for each raw output, for each of its bits, check if it is a 1 and tally
        for (size_t j = 0; j < 1000000; j++)
        {
            for (size_t k = 0; k < 64; k++)
            {
                if ((raw[j] >> k) & 0x1ULL)
                {
                    bitpos[k]++;
                }
            }
        }
        
        //assert probability is in the ideal bound
        //also reset bitpos after check to prepare for next round
        for (size_t k = 0; k < 64; k++)
        {
            TEST_ASSERT_DOUBLE_WITHIN(5.0E-3, prob[i], (double) bitpos[k]/1000000.0);
            bitpos[k] = 0;
        }
    }
    
    //teardown
    spk_GeneratorDelete(SUT);
    free(raw);
}

/******************************************************************************/

void test_bias_at_all_256_probabilities_in_8bit_resolution_XSH64(void)
{
    //arrange
    spk_generator SUT;
    CHECK(spk_GeneratorNew(&SUT, SPK_GENERATOR_XSH64, 0));
    
    //fix the resolution to 8 bits
    const int resolution = 8;
    
    //prob array holds the expected probability on each bit on each experiment
    double px = 0.0;
    double prob[256] = {0.0};
    
    for (size_t i = 0; i < 256; i++)
    {
        prob[i] = px;
        px += 0.00390625;
    }
    
    //in each experiment use this to track the total success (1) per position
    uint64_t bitpos[64] = {0};
    
    //to speed up the most inner loop, we'll have bias fill an array in one call
    uint64_t *raw = malloc(sizeof(uint64_t) * 1000000); CHECK(raw == NULL);
    
    //act + assert
    for (size_t i = 0; i < 256; i++)
    {
        //fill for current round
        CHECK(SUT->bias(SUT, raw, 1000000, prob[i], resolution));
        
        //for each raw output, for each of its bits, check if it is a 1 and tally
        for (size_t j = 0; j < 1000000; j++)
        {
            for (size_t k = 0; k < 64; k++)
            {
                if ((raw[j] >> k) & 0x1ULL)
                {
                    bitpos[k]++;
                }
            }
        }
        
        //assert probability is in the ideal bound
        //also reset bitpos after check to prepare for next round
        for (size_t k = 0; k < 64; k++)
        {
            TEST_ASSERT_DOUBLE_WITHIN(5.0E-3, prob[i], (double) bitpos[k]/1000000.0);
            bitpos[k] = 0;
        }
    }
    
    //teardown
    spk_GeneratorDelete(SUT);
    free(raw);
}

/******************************************************************************/

int main(void)
{
    UNITY_BEGIN();
        //seeding tests
        RUN_TEST(test_deterministic_seed_for_PCG64i);
        RUN_TEST(test_deterministic_seed_for_XSH64);
        
        //rand tests
        RUN_TEST(test_bounded_random_integers_in_zero_one_stay_in_zero_one_PCG64i);
        RUN_TEST(test_bounded_random_integers_in_zero_one_stay_in_zero_one_XSH64);        
        RUN_TEST(test_bounded_random_integers_upper_bound_is_inclusive_PCG64i);
        RUN_TEST(test_bounded_random_integers_upper_bound_is_inclusive_XSH64);
        RUN_TEST(test_bounded_with_max_limits_is_identical_to_raw_output_PCG64i);
        RUN_TEST(test_bounded_with_max_limits_is_identical_to_raw_output_XSH64);
        
        //unid tests
        RUN_TEST(test_unid_values_stay_in_unit_interval_PCG64i);
        RUN_TEST(test_unid_values_stay_in_unit_interval_XSH64);
        RUN_TEST(test_pairs_of_unid_values_can_estimate_value_of_pi_PCG64i);
        RUN_TEST(test_pairs_of_unid_values_can_estimate_value_of_pi_XSH64);
        
        //bias tests
        RUN_TEST(test_bias_at_all_256_probabilities_in_8bit_resolution_PCG64i);
        RUN_TEST(test_bias_at_all_256_probabilities_in_8bit_resolution_XSH64);
    return UNITY_END();
}
