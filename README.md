# SCIPACK
SCIPACK is a scientific computing package targeted towards C applications on Intel/AMD hardware. 
It includes modules for psuedo random number generation, optimization, array processing, and more. 
It is currently in version 0.2.6-alpha.

# Requirements
To build SCIPACK on Linux you need the GNU C compiler and GNU Make. Windows users can build SCIPACK via Cygwin.

You also need a modern x86 64-bit processor. SCIPACK uses x86 instructions such as RDRAND, RDTSCP, and LFENCE. 
It also relies heavily on the AVX/AVX2 instruction sets. 
During the build, the preprocessor will notify you of any missing requirements. 
Additionally, you can check for these requirements through the Intel/AMD online product specifications, or with the `cpuid.h` interface available via GCC.

# Build
SCIPACK is offered as a static library. Just run `make` from the root directory. 
The static library `libscipack.a` will be placed in the newly created `./build/lib` directory. 
You may also access the individual object files for each submodule in `./build/obj`. 

The public API is located in the `./include` directory. 
In your own programs, you can either include just `scipack.h` as a convenience, or all of the specific submodule headers.

# Testing
You can check that SCIPACK is functioning correctly on your machine by creating the test suite via `make tests`. 
Executables will be placed in `./build`. Use `make tests only=...` if you want to test a specific module or submodule.

# Benchmarking
To microbenchmark the core SCIPACK functions, execute `make benchmarks` in the root directory. Then run `./build/benchmarks`.
 
These microbenchmarks are performed using the SCIPACK timer submodule, which uses the time stamp counter (TSC) as its underlying wall clock. 
For portability, the benchmarks program estimates the TSC frequency on startup. 
This may take several seconds. 
Typically, the estimate is within 10 megahertz of the true value.

If you know the true TSC frequency and the TSC estimate is inaccurate, you can override the internal sampling algorithm. 
Open the `./src/timing/timer.c` file and hardcode the known frequency into the `tsc_hz` global variable. 
Do not forget to reset `tsc_hz` to zero if you port your local files across different processors.
