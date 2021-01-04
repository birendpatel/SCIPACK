# SCIPACK
SCIPACK is a scientific computing package targeted towards C applications on Intel/AMD hardware. 
It includes modules for psuedo random number generation, optimization, probability, and more.

The project is in the initial development phase. 
The current version is 0.2.9-beta. 
Versions of the form 0.x.y imply that:
* Any source code may change at any time.
* Backwards compatibility is not guaranteed.
* The API is not stable.

# What's New
Version 0.2 features many updates to the `generator_sisd` submodule for low-level random number generation. 
The API now exposes the `spk_generator` struct as a generic interface for all supported PRNGs. 

```C
//initialize a 64-bit insecure permuted congruential generator
struct spk_generator *rng1;
spk_GeneratorNew(&rng1, SPK_GENERATOR_PCG64i, 0);

//or use the default generator if you don't know your PCG's from your LCG's
struct spk_generator *rng2;
spk_GeneratorNew(&rng2, SPK_GENERATOR_DEFAULT, 0);

//fill a data buffer with raw generator output
uint64_t buffer[100];
rng1->next(rng1, buffer, 100);

//get random points in the unit interval
double uniform_values[10];
rng2->unid(rng2, uniform_values, 10);

//don't forget to clean up to avoid memory leaks
spk_GeneratorDelete(&rng1);
spk_GeneratorDelete(&rng2);
```

Version 0.3 will mark the completion of optimizations and unit tests for the `generator_sisd` submodule. 
From there, its API will remain stable until 1.0.0 is released.

# Requirements
To build SCIPACK on Linux you need the GNU C compiler and GNU Make. Windows users can build SCIPACK via Cygwin.

You also need a modern x86 64-bit processor. SCIPACK uses x86 instructions such as RDRAND, RDTSCP, and LFENCE. 
It also relies heavily on the AVX/AVX2 instruction sets. 
During the build, the preprocessor will notify you of any missing requirements.

# Build
SCIPACK is available as a static library. Just run `make` from the root directory. 
The static library `libscipack.a` will be placed in the newly created `./build/lib` directory. 

The public API is located in the `./include` directory. 
In your own programs, you can either include the convenience header `scipack`, or all of the specific submodule headers.

# Testing
You can check that SCIPACK is functioning correctly on your machine by creating the test suite via `make tests`. 
Executables will be placed in `./build`.

Use `make tests only=testname` if you want to test a specific module or submodule. 
You may substitute `testname` with any file or subdirectory listed in `./src`.
However, in the initial development phase, and particularly in alpha versions, some unit test executables will be empty.

# Benchmarking
To microbenchmark the core SCIPACK functions, execute `make benchmarks` in the root directory. Then run `./build/benchmarks`.
 
These microbenchmarks are performed using the SCIPACK timer submodule, which uses the time stamp counter (TSC) as its underlying wall clock. 
For portability, the benchmarks program estimates the TSC frequency on startup. 
Typically, the estimate is within 10 megahertz of the true value.

If you know the true TSC frequency of your processor, you can override the internal sampling algorithm. 
Simply hardcode the known frequency into the `tsc_hz` global variable in `./src/timing/timer.c`.
