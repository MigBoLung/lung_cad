#ifndef __MIG_TYPES_H__
#define __MIG_TYPES_H__

#include "mig_config.h"

/*********************************************************/
/* DATA TYPES  -> MIG */
typedef unsigned char   Mig8u;
typedef unsigned short  Mig16u;
typedef unsigned int    Mig32u;
typedef signed char     Mig8s;
typedef signed short    Mig16s;
typedef signed int      Mig32s;
typedef float           Mig32f;
typedef double          Mig64f;

/*********************************************************/
/* DATA TYPES  -> STANDARD */
#if defined(_MSC_VER)

# define mig_int8_t   char
# define mig_uint8_t  unsigned char
# define mig_int16_t  short
# define mig_uint16_t unsigned short
# define mig_int32_t  int
# define mig_uint32_t unsigned int
# define mig_int64_t  __int64
# define mig_uint64_t unsigned __int64

#else

# include <inttypes.h>

#endif

/*********************************************************/
/* DATA TYPES  -> DEPENDENT ON BUS WIDTH */
#if defined(MIG_ARCH_IS_32BIT)

# define mig_ptr_t uint32_t
# define mig_intptr_t int32_t

# if defined(_MSC_VER) && _MSC_VER >= 1300 && !defined(__INTEL_COMPILER)
#  include <stdarg.h>
# else
#  define uintptr_t uint32_t
# endif

#elif defined(MIG_ARCH_IS_64BIT)

# define mig_ptr_t uint64_t
# define mig_intptr_t int64_t

# if defined (_MSC_VER) && _MSC_VER >= 1300 && !defined(__INTEL_COMPILER)
#  include <stdarg.h>
# else
#  define uintptr_t uint64_t
# endif

#else

# error Must define address bus size as ARCH_IS_32BIT or ARCH_IS_64BIT

#endif

/*********************************************************/
/* DATA TYPE LIMITS */

#define MIG_MAX_8U      ( 0xFF )
#define MIG_MAX_16U     ( 0xFFFF )
#define MIG_MAX_32U     ( 0xFFFFFFFF )

#define MIG_MIN_8U      ( 0 )
#define MIG_MIN_16U     ( 0 )
#define MIG_MIN_32U     ( 0 )
#define MIG_MIN_8S      (-128 )

#define MIG_MAX_8S      ( 127 )
#define MIG_MAX_16S     ( 32767 )
#define MIG_MAX_32S     ( 2147483647 )

#define MIG_MIN_8S      (-128 )
#define MIG_MIN_16S     (-32768 )
#define MIG_MIN_32S     (-2147483647 - 1 )

/* double and floating point values */
/* float epsilon value */
#if !defined(FLT_EPSILON)
#	define MIG_EPS_32F	(1.192092896e-07F)
#else
#	define MIG_EPS_32F	(FLT_EPSILON)
#endif

/* double epsilon value */
#if !defined(DBL_EPSILON)
#	define MIG_EPS_64F	(2.2204460492503131e-016)
#else
#	define MIG_EPS_64F	(DBL_EPSILON)
#endif

/* float absolute min value */
#if !defined(FLT_MIN)
#	define MIG_MINABS_32F	(1.175494351e-38F)
#else
#	define MIG_MINABS_32F	(FLT_MIN)
#endif

/* double absolute min value */
#if !defined(DBL_MIN)
#	define MIG_MINABS_64F	(2.2250738585072014e-308)
#else
#	define MIG_MINABS_64F	(DBL_MIN)
#endif

/* float absolute max value */
#if !defined(FLT_MAX)
#	define MIG_MAX_32F	(3.402823466e+38F)
#else
#	define MIG_MAX_32F	(FLT_MAX)
#endif

/* double absolute max value */
#if !defined(DBL_MAX)
#	define MIG_MAX_64F	(1.7976931348623158e+308)
#else
#	define MIG_MAX_64F	(DBL_MAX)
#endif

/* float minimum value - negative */
#define MIG_MIN_32F		(-1.70141173e+38F)

/* double minimum value - negative */
#define MIG_MIN_64F		(-1.7976931348623157e+308)

#endif /* __MIG_TYPES_H__ */


/*******************************************************************/
/* DIXYGEN DOCUMENTATION */
/*******************************************************************/

/** \file mig_data_types.h
    \brief System wide data types.
*/

/** \def MIG_MAX_8U
    \brief 8 bit unsigned maximum value.
*/

/** \def MIG_MAX_16U
    \brief 16 bit unsigned maximum value.
*/

/** \def MIG_MAX_32U
    \brief 32 bit unsigned maximum value.
*/

/** \def MIG_MIN_8U
    \brief 8 bit unsigned minimum value.
*/

/** \def MIG_MIN_16U
    \brief 16 bit unsigned minimum value.
*/

/** \def MIG_MIN_32U
    \brief 32 bit unsigned minimum value.
*/

/** \def MIG_MIN_8S
    \brief 8 bit signed minimum value.
*/

/** \def MIG_MAX_8S
    \brief 8 bit signed maximum value.
*/

/** \def MIG_MAX_16S
    \brief 16 bit signed maximum value.
*/

/** \def MIG_MAX_32S
    \brief 32 bit signed maximum value.
*/

/** \def MIG_MIN_8S
    \brief 8 bit signed minimum value.
*/

/** \def MIG_MIN_16S
    \brief 16 bit signed minimum value.
*/

/** \def MIG_MIN_32S
    \brief 32 bit signed minimum value.
*/
