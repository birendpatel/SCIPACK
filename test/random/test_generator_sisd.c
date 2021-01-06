/*
* NAME: Copyright (C) 2021, Biren Patel
* DESC: Unit tests for src/random/generator_sisd.c
* LICS: MIT License
*/

#include "generator_sisd.h"
#include "unity.h"

#include <stdlib.h>
#include <stdio.h>

/******************************************************************************/

#define CHECK(x)                                                               \
        if (x)                                                                 \
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
    SUT->rand(SUT, SUT_output, 1000, 0, 1);
    
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
    SUT->rand(SUT, SUT_output, 1000, 0, 1);
    
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
    SUT->rand(SUT, SUT_output, 1000, 0, 1);
    
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
    SUT->rand(SUT, SUT_output, 1000, 0, 1);
    
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
    SUT1->next(SUT1->state, SUT1_output, 100);
    SUT2->rand(SUT2, SUT2_output, 100, 0, UINT64_MAX);
    
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
    SUT1->next(SUT1->state, SUT1_output, 100);
    SUT2->rand(SUT2, SUT2_output, 100, 0, UINT64_MAX);
    
    //assert
    TEST_ASSERT_EQUAL_UINT64_ARRAY(SUT1_output, SUT2_output, 100);
    
    //teardown
    spk_GeneratorDelete(SUT1);
    spk_GeneratorDelete(SUT2);
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
    return UNITY_END();
}
