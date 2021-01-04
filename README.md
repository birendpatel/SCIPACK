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
Version 0.2 features many updates to the `random` module. 
Most importantly, the `spk_generator` struct is now available as an interface to all SISD psuedo random number generators.

```C
//initialize a 64-bit insecure permuted congruential generator
struct spk_generator *pcg;
spk_GeneratorNew(&pcg, SPK_GENERATOR_PCG64i, 0);

//or initialize a naive 64-bit xorshift with a deterministic seed
struct spk_generator *xsh;
spk_GeneratorNew(&xsh, SPK_GENERATOR_XSH64, 42);

//fill a data buffer with raw generator output
uint64_t buffer[100];
pcg->next(rng, buffer, 100);

//get random points in the unit interval
double uniform_values[10];
xsh->unid(rng, uniform_values, 10);

//don't forget to clean up
spk_GeneratorDelete(&pcg);
spk_GeneratorDelete(&xsh);
```

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
