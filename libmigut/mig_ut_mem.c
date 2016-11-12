#include "mig_ut_mem.h"

#if defined(MIG_USE_VECTOR_INSTRUCTIONS)

void*
mig_calloc ( unsigned int nitems , size_t size )
{
	void *p = _aligned_malloc ( size * nitems , MIG_MEM_ALIGN );
	if (!p)
		return NULL;

	memset ( p , 0x00 , size * nitems );
	return p;
}

#endif /* MIG_USE_VECTOR_INSTRUCTIONS */


