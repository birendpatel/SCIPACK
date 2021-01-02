/*
* NAME: Copyright (C) 2021, Biren Patel
* DESC: SCIPACK configurations and hardware/compiler checks
* LICS: MIT License
*/

#ifndef SCIPACK_CONFIG_H
#define SCIPACK_CONFIG_H

/*******************************************************************************
* version info
*******************************************************************************/
#define SCIPACK_ALPHA   0
#define SCIPACK_BETA    1
#define SCIPACK_PROD    2

#define SCIPACK_MAJOR   0
#define SCIPACK_MINOR   1
#define SCIPACK_PATCH   0
#define SCIPACK_STAGE   SCIPACK_PROD

/*******************************************************************************
* Hardware and compiler checks
*******************************************************************************/

#ifndef __x86_64__
    #error "SCIPACK requires x86-64"
#endif

#ifndef __GNUC__
    #error "SCIPACK requires a GNU C compiler"
#endif

//submodules: random_sisd (seeding), timer (guarantees a constant TSC)
#ifndef __RDRND__
    #error "SCIPACK requires x86 RDRAND instruction"
#endif

//submodules: timer (fence instructions)
#ifndef __SSE2__
    #error "SCIPACK requires SSE2 instruction set"
#endif

/*******************************************************************************
* Library error codes
*******************************************************************************/

#define SCIPACK_ERROR_SUCCESS           0       /* no error has occured       */
#define SCIPACK_ERROR_STDMALLOC         1       /* stdlib malloc fail         */
#define SCIPACK_ERROR_STDCALLOC         2       /* stdlib calloc fail         */
#define SCIPACK_ERROR_STDREALLOC        3       /* stdlib realloc fail        */
#define SCIPACK_ERROR_RDRAND            4       /* rdrand retry loop fail     */
#define SCIPACK_ERROR_UNDEFINED         999     /* no error has been set      */

#endif