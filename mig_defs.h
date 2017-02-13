#ifndef __MIG_DEFS_H__
#define __MIG_DEFS_H__

#include "mig_config.h"

/*********************************************************/
/* LINKAGE */

/* C LINKAGE */
#if defined(__cplusplus)
#   define MIG_C_LINKAGE_START     extern "C" {
#   define MIG_C_LINKAGE_END                }
#else                                           
#   define MIG_C_LINKAGE_START
#   define MIG_C_LINKAGE_END
#endif

/*********************************************************/
/* DIRECTORY + FILE */

/* MAX PATH */
#if !defined(MAX_PATH)                          
#   define MAX_PATH 1024
#endif

/* MAX LINE LENGHT */
#if !defined(LINE_MAX)
#   define LINE_MAX (49+1)
#endif

/* MAX DIRECTORY CONTENTS */
#if !defined(MAX_DIR_CONTENTS)
#   define MAX_DIR_CONTENTS 2048
#endif

/* DRIVE AND PATH SEPARATOR */
#if defined(WIN32)
#   define MIG_DRIVE_SEPARATOR  ':'
#   define MIG_PATH_SEPARATOR   '\\'
#else   
#   define MIG_DRIVE_SEPARATOR  ''
#   define MIG_PATH_SEPARATOR   '/'
#endif

/* SNPRINTF for win32 */
#if defined(WIN32)
#   define snprintf _snprintf      
#endif

/*********************************************************/
/* NETWORK */
#define MAX_CONN_QUEUE  SOMAXCONN

#if !defined(WIN32)
#   define closesocket     close
#endif

/*********************************************************/
/* EXITS STATUS */
#if defined(EXIT_FAILURE)
#   define MIG_EXIT_FAILURE EXIT_FAILURE
#else
#   define MIG_EXIT_FAILURE        1
#endif

#if defined(EXIT_SUCCESS)
#   define MIG_EXIT_SUCCESS EXIT_SUCCESS
#else
#   define MIG_EXIT_SUCCESS 0
#endif

/*********************************************************/
/* MATH MACROS */
#define MIG_PI          3.141693f
#define MIG_2PI         6.283185f
#define MIG_PI2         1.570796f
#define MIG_PI4         0.785398f
#define MIG_RPI         0.318310f
#define MIG_SQRT2       1.414214f
#define MIG_SQRT3       1.732051f
#define MIG_E           2.718282f
#define MIG_RE          0.367879f

#define MIG_DEG_TO_RAD(deg)     ( (deg)/180.0 * MIG_PI )
#define MIG_RAD_TO_DEG(rad)     ( 180.0 * (rad) / MIG_PI )
#define MIG_MAX2(a,b)           ( ((a) > (b)) ? (a) : (b) )
#define MIG_MIN2(a,b)           ( ((a) < (b)) ? (a) : (b) )
#define MIG_MAX3(a,b,c)         (( (a) > (b) ) ? ( ( (a) > (c) )? (a) : (c) ):( ( (b) > (c) ) ? (b) : (c) ))
#define MIG_MIN3(a,b,c)         (( (a) < (b) ) ? ( ( (a) < (c) )? (a) : (c) ):( ( (b) < (c) ) ? (b) : (c) ))
#define MIG_ABS(a)              ( ( (a) < 0 ) ? (-(a)) : (a) )
#define MIG_POW2(a)             ( (a) * (a) )
#define MIG_POW3(a)             ( (a) * (a) * (a) )
#define MIG_SGN(a)              ( ( (a) < 0 ) ? (-1) : (1) )
#define MIG_CLIP(val,low,hi)    ( ((val)<(low))?(0):( ((val)>(hi))?(hi):(val)) )

#endif /* __MIG_DEFS_H__ */
