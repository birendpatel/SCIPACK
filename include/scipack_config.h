/*
* NAME: Copyright (C) 2020, Biren Patel
* DESC: SCIPACK configurations and hardware/compiler checks
* LICS: MIT License
*/

#ifndef SCIPACK_CONFIG_H
#define SCIPACK_CONFIG_H

#ifndef __x86_64__
    #error "SCIPACK requires x86-64"
#endif

#ifndef __GNUC__
    #error "SCIPACK requires a GNU C compiler"
#endif

//submodules: random_sisd
#ifndef __RDRND__
    #error "SCIPACK requires x86 RDRAND instruction"
#endif

//submodules: timer
#ifndef __SSE2__
    #error "SCIPACK requires SSE2 instruction set"
#endif

#endif