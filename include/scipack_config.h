/*
* NAME: Copyright (C) 2021, Biren Patel
* DESC: SCIPACK configurations and hardware/compiler checks
* LICS: MIT License
*/

#ifndef SPK_CONFIG_H
#define SPK_CONFIG_H

/*******************************************************************************
* version info
*******************************************************************************/
#define SPK_ALPHA   0
#define SPK_BETA    1
#define SPK_PROD    2

#define SPK_MAJOR   0
#define SPK_MINOR   2
#define SPK_PATCH   2
#define SPK_STAGE   SPK_ALPHA

/*******************************************************************************
* Hardware and compiler checks
*******************************************************************************/

#ifndef __x86_64__
    #error "SCIPACK requires x86-64"
#endif

#ifndef __GNUC__
    #error "SCIPACK requires a GNU C compiler"
#endif

//submodules: generator_sisd (seeding), timer (rdrand guarantees a constant TSC)
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

#define SPK_ERROR_SUCCESS           0       /* no error has occured           */
#define SPK_ERROR_STDMALLOC         1       /* stdlib malloc fail             */
#define SPK_ERROR_STDCALLOC         2       /* stdlib calloc fail             */
#define SPK_ERROR_STDREALLOC        3       /* stdlib realloc fail            */
#define SPK_ERROR_RDRAND            4       /* rdrand retry loop fail         */
#define SPK_ERROR_ARGBOUNDS         5       /* fx argument is out of bounds   */
#define SPK_ERROR_UNDEFINED         999     /* no error has been set          */

//TODO: function to fetch verbose error description

/*******************************************************************************
* Logging mechanism
*******************************************************************************/

//TODO: generic scipack callback to allow struct hooks
//TODO: dummy callback when end user does not hook in a logger

#endif