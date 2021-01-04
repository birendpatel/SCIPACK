/*
* NAME: Copyright (C) 2020, Biren Patel
* DESC: high precision benchmarking for core subroutines
* LICS: MIT License
*/

#include "scipack.h"

#include <stdbool.h>    //cycles_stats
#include <stdlib.h>     //malloc, size_t
#include <stdio.h>      //puts, printf, fprintf, fflush
#include <string.h>     //memcpy

/*******************************************************************************
Gather statistical measures from an input array of raw clock ticks, where each
array element represents the results of a single simulation. Due to operating
system interference, we use median and median absolute deviation instead of
average and standard deviation. This avoids data pollution due to OS preemption.
*/

#define ULLCAST(x) *((const unsigned long long *) (x))
#define ULLSIZE sizeof(unsigned long long)

static int ullcmp(const void *a, const void *b)
{
    unsigned long long num1 = ULLCAST(a), num2 = ULLCAST(b);
    
    if (num1 < num2) return -1;
    else if (num1 == num2) return 0;
    else return 1;
}

//returns true on error
static bool cycles_stats
(
    const unsigned long long *data,
    const size_t n,
    unsigned long long *median,
    unsigned long long *mad,
    unsigned long long *min,
    unsigned long long *max
)
{
    //don't mess with the underlying array in case we need the original order
    unsigned long long *datacpy = malloc(n * ULLSIZE);
    if (!datacpy) return true;
    memcpy(datacpy, data, n * ULLSIZE);
    
    //first three measures are derived via quick sort
    qsort(datacpy, n, ULLSIZE, ullcmp);
    *median = datacpy[n/2];
    *min = datacpy[0];
    *max = datacpy[n - 1];
    
    //now get the median absolute deviation
    //long long cast okay, ticks don't get large enough on microbenchmarks
    for (size_t i = 0; i < n; i++) 
    {
        long long tmp = (long long) datacpy[i] - (long long) *median;
        datacpy[i] = (unsigned long long) (tmp >= 0 ? tmp : -tmp);
    }
    
    qsort(datacpy, n, ULLSIZE, ullcmp);
    *mad = datacpy[n/2];
    
    free(datacpy);
    return false;    
}

/*******************************************************************************
Set up TSC timer, establish reference frequency. This take a long time so we
let the user know whats happening before the benchmarks start to roll out.
TODO: pthreads implementation for progress bar
*/

static void timer_setup(void)
{
    printf("Estimating TSC Frequency...");
    fflush(stdout);
    
    unsigned long long freq = TimerGetHz();
    
    printf("\r                            ");
    printf("\rEstimated TSC Frequency: %4.2f GHz\n", (double) freq / 1.0E9);
}

/*******************************************************************************
Unity-style benchmarking API.
*/

#define BENCHMARKS_BEGIN()                                                     \
        system("clear");                                                       \
        puts("SCIPACK Benchmarks - Copyright (C) 2020, Biren Patel\n");        \
        printf("Compile Date: %s\n", __DATE__);                                \
        printf("Compile Time: %s\n", __TIME__);                                \
        timer_setup()

#define BENCHMARKS_MODULE(str) puts("\n### " str " ###")

static int total = 0;

#define RUN_BENCHMARK(x) (*x)(); total += 1

#define BENCHMARKS_END()                                                       \
        printf("\nfinished. %d benchmarks executed.\n", total);                \
        return EXIT_SUCCESS


/*******************************************************************************
Preset simulation size options
*/

#define TINY_SIM    10
#define SMALL_SIM   100
#define MEDIUM_SIM  1000
#define LARGE_SIM   10000
#define MASSIVE_SIM 100000


/*******************************************************************************
Microbenchmarking core function. Execute "test" across "sim_limit" total
simulations where test repeats itself "instr_limit" times within a single
simulation. "testname" is printed to the console. Also include a rough and small
warm up period which mimics the test loop with 10x iterations.
*/

#define ANALYZE(testname, test, sim_limit, instr_limit)                        \
do                                                                             \
{                                                                              \
    int error = 0;                                                             \
                                                                               \
    unsigned long long med = 0;                                                \
    unsigned long long mad = 0;                                                \
    unsigned long long min = 0;                                                \
    unsigned long long max = 0;                                                \
                                                                               \
    unsigned long long *data = malloc(sim_limit * ULLSIZE);                    \
                                                                               \
    if (!data)                                                                 \
    {                                                                          \
        fprintf(stderr, "malloc failure during benchmarks\n");                 \
        exit(EXIT_FAILURE);                                                    \
    }                                                                          \
                                                                               \
    for (size_t j = 0; j < instr_limit; j++)                                   \
    {                                                                          \
        (test);                                                                \
        __asm__ volatile ("");                                                 \
    }                                                                          \
                                                                               \
    for (size_t i = 0; i < sim_limit; i++)                                     \
    {                                                                          \
        TimerStart();                                                          \
                                                                               \
        for (size_t j = 0; j < instr_limit; j++)                               \
        {                                                                      \
            (test);                                                            \
            __asm__ volatile ("");                                             \
        }                                                                      \
                                                                               \
        TimerStop();                                                           \
                                                                               \
        data[i] = TimerElapsedCycles();                                        \
    }                                                                          \
                                                                               \
    error = cycles_stats(data, sim_limit, &med, &mad, &min, &max);             \
                                                                               \
    if (error)                                                                 \
    {                                                                          \
        fprintf(stderr, "cycles statistics failure\n");                        \
    }                                                                          \
                                                                               \
    timer_result med_tr = TimerElapsedTime(med);                               \
    timer_result mad_tr = TimerElapsedTime(mad);                               \
    timer_result min_tr = TimerElapsedTime(min);                               \
    timer_result max_tr = TimerElapsedTime(max);                               \
                                                                               \
    printf("\n" testname ", %d iterations:\n", instr_limit);                   \
    printf("    min:  %-6.2f %s\n", min_tr.elapsed, min_tr.symbol);            \
    printf("    med:  %-6.2f %s\n", med_tr.elapsed, med_tr.symbol);            \
    printf("    max:  %-6.2f %s\n", max_tr.elapsed, max_tr.symbol);            \
    printf("    mad:  %-6.2f %s\n", mad_tr.elapsed, mad_tr.symbol);            \
                                                                               \
    free(data);                                                                \
}                                                                              \
while (0)                                                                      \


/******************************************************************************/

void benchmark_generator_sisd_pcg64_insecure_next(void)
{
    int error = 0;
    
    struct spk_generator *rng;
    error = spk_GeneratorNew(&rng, SPK_GENERATOR_PCG64i, 0)
    
    if (error)
    {
        fprintf(stderr, "pcg64 insecure init failure: code %d\n", error);
        exit(EXIT_FAILURE);
    }
    
    uint64_t *buffer = malloc(1000 * sizeof(uint64_t));
    if (!buffer)
    {
        fprintf(stderr, "pcg64 insecure malloc failure\n");
        exit(EXIT_FAILURE);
    }
    
    ANALYZE("PCG 64-bit insecure next, 1000 iter", rng->next(rng, buffer, 1000), MEDIUM_SIM, 1);
    
    spk_GeneratorDelete(&rng);
}

/******************************************************************************/

void benchmark_generator_sisd_xorshift64_next(void)
{
    int error = 0;
    
    struct spk_generator *rng;
    error = spk_GeneratorNew(&rng, SPK_GENERATOR_XSH64, 0)
    
    if (error)
    {
        fprintf(stderr, "xsh64 init failure: code %d\n", error);
        exit(EXIT_FAILURE);
    }
    
    uint64_t *buffer = malloc(1000 * sizeof(uint64_t));
    if (!buffer)
    {
        fprintf(stderr, "xsh64 malloc failure\n");
        exit(EXIT_FAILURE);
    }
    
    ANALYZE("Xorshift 64-bit next, 1000 iter", rng->next(rng, buffer, 1000), MEDIUM_SIM, 1);
    
    spk_GeneratorDelete(&rng);
}

/******************************************************************************/

void benchmark_generator_sisd_pcg64_insecure_bias(void)
{
    int error = 0;
    
    struct spk_generator *rng;
    error = spk_GeneratorNew(&rng, SPK_GENERATOR_PCG64i, 0)
    
    if (error)
    {
        fprintf(stderr, "pcg64 insecure init failure: code %d\n", error);
        exit(EXIT_FAILURE);
    }
    
    uint64_t *buffer = malloc(1000 * sizeof(uint64_t));
    if (!buffer)
    {
        fprintf(stderr, "pcg64 insecure malloc failure\n");
        exit(EXIT_FAILURE);
    }
    
    ANALYZE("PCG 64-bit insecure bias, 1000 iter", rng->bias(rng, buffer, 1000, 1, 8), MEDIUM_SIM, 1);
    
    spk_GeneratorDelete(&rng);
}

/******************************************************************************/

int main(void)
{    
    BENCHMARKS_BEGIN();
        BENCHMARKS_MODULE("psuedo random number generators");
            RUN_BENCHMARK(benchmark_generator_sisd_pcg64_insecure_next);
            RUN_BENCHMARK(benchmark_generator_sisd_xorshift64_next);
            RUN_BENCHMARK(benchmark_generator_sisd_pcg64_insecure_bias);
    BENCHMARKS_END();
}
