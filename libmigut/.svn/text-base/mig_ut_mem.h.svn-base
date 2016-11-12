#ifndef __MIG_UT_MEM_H__
#define __MIG_UT_MEM_H__

#include "mig_config.h"
#include "mig_defs.h"
#include "mig_data_types.h"

MIG_C_LINKAGE_START

/* use MMX , SSE , SSE2 , SSE3 */
#if defined(MIG_USE_VECTOR_INSTRUCTIONS)

# define mig_malloc(size) \
	_aligned_malloc((size),MIG_MEM_ALIGN)

# define mig_free(ptr) \
	_aligned_free((ptr))

/* the following is needed when trying
   to get the adress of the function we are using
*/
# define mig_free_f   _aligned_free

# define mig_realloc(ptr,size) \
	_aligned_realloc((ptr),(size),MIG_MEM_ALIGN)

# if defined(WIN32) /* we have memcopy in assembly only for windows */

#  define mig_memcpy(src,dst,size) \
	mig_memcpy_mmx((dst),(src),(size))

# else	/* linux */

#  define mig_memcpy(src,dst,size) \
	memcpy((dst),(src),(size))

# endif	/* WIN32 */

extern void
mig_memcpy_mmx( void *dst , const void *src , int size );

extern void*
mig_calloc( unsigned int nitems , unsigned int size );

#else			/* do not USE MMX , SSE , SSE2 , SSE3 */

# define mig_malloc(size)	\
	malloc((size))

# define mig_free(ptr) \
	free((ptr))

/* the following is needed when trying
   to get the adress of the function we are using
*/
# define mig_free_f      free

# define mig_realloc(ptr,size) \
	realloc((ptr),(size))

# define mig_memcpy(src,dst,size) \
	memcpy((dst),(src),(size))

# define mig_calloc(nitems,size) \
	calloc((nitems),(size))

#endif /* NOT USING MMX , SSE , SSE2 , SSE3 */

#define mig_memset(ptr,c,size) \
	memset((ptr),(c),(size))

#define mig_memz(ptr,size) \
        memset((ptr),0x00,(size))

#define mig_memz_fast(ptr,size) \
	memset((ptr),0x00,(size))

MIG_C_LINKAGE_END

#endif /* __MIG_UT_MEM_H__ */
