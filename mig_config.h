#ifndef __MIG_CONFIG_H__
#define __MIG_CONFIG_H__

/* WINDOWS */
#if defined(WIN32) || defined(_WIN32)


/* leak detection code */
/*#define _MIG_TRACK_LEAKS*/
#ifdef _MIG_TRACK_LEAKS
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif _MIG_TRACK_LEAKS

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

/* Eigen requires min and max are not defined by windows.h */
#define NOMINMAX

#pragma warning( disable : 4996 )
#pragma warning( disable : 4018 )
#pragma warning( disable : 4127 )
#pragma warning( disable : 4267 )
#pragma warning( disable : 4204 )
#pragma warning( disable : 4244 )
#pragma warning( disable : 4530 )

#include <windows.h>
#include <winsock.h>
#include <process.h>
#include <direct.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <io.h>
#include <stdlib.h>
#include <stdio.h>
#include <share.h>

#else /* LINUX */

#include <unistd.h>
#include <stdint.h>
#include <dirent.h>
#include <signal.h>
#include <sys/time.h>
#ifdef __APPLE__
        #include <sys/uio.h>
#else
        #include <sys/io.h>
#endif

#endif /* LINUX */

/* ALL */

#define _USE_MATH_DEFINES

#if defined(__cplusplus)    /* C++ wrappers */

#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cstring>
#include <cctype>
#include <ctime>
#include <climits>
#include <cfloat>
#include <cassert>

#else               /* Standard C */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <limits.h>
#include <float.h>
#include <assert.h>

#endif              /* C++ Wrappers */

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

/* MATLAB
   Uncomment to use MATLAB.
   matlab include directories should be present in the environment
   variable INCLUDE
   matlab library directories should be present in the environment
   variable LIB
*/

/*#define MATLAB*/

/* MMX SSE SSE2 SSE3 */
/* To use vector instructions code
   define MIG_USE_VECTOR_INSTRUCTIONS.
   Vectore code uses only SSE2 instructions
   as these are supported on most modern
   CPUs. */

//#define MIG_USE_VECTOR_INSTRUCTIONS
#if defined(MIG_USE_VECTOR_INSTRUCTIONS)
# define __SSE__
# define SSE2
# include <xmmintrin.h>
# include <emmintrin.h>
# include <malloc.h>
# define MIG_MEM_ALIGN  16
#endif /* MIG_USE_VECTOR_INSTRUCTIONS */

/* Under Linux the following are
   defined via Makefile.linux
*/
#if defined(WIN32)

#define MIG_ARCH_IS_32BIT
//#define MIG_ARCH_IS_64BIT
#define MIG_LITTLE_ENDIAN
//#define MIG_BIG_ENDIAN

#define DLLEXPORT __declspec(dllexport)

#else

#define DLLEXPORT

#endif /* WIN32 */

#endif /* __MIG_CONFIG_H__ */

/*******************************************************************/
/* DOXYGEN DOCUMENTATION */
/*******************************************************************/

/** \file mig_config.h
    \brief System configuration file.

    This file includes system header used
    by all libraries and programs. Here you can :

        1. Choose to use MATLAB I/O.

        2. Choose to use SSE instructions.

        3. Choose between 32/64 bit architecture.

        4. Choose between little/big endian architecture.
*/

/** \def MIG_USE_MATLAB
    \brief Compile MATLAB I/O code.
*/

/** \def MATLAB
    \brief Compile MATLAB I/O code.
*/

/** \def MIG_USE_VECTOR_INSTRUCTIONS
    \brief Compile MMX/SSE/SSE2 instructions.
*/

/** \def MIG_ARCH_IS_32BIT
    \brief Define under windows if architecture is 32bit.
*/

/** \def MIG_ARCH_IS_64BIT
    \brief Define under windows if architecture is 64bit.
*/

/** \def MIG_LITTLE_ENDIAN
    \brief Define under windows if architecture is little endian.
*/

/** \def MIG_BIG_ENDIAN
    \brief Define under windows if architecture is big endian.
*/
