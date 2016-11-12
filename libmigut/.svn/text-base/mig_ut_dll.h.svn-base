#ifndef __MIG_UT_DLL_H__
#define __MIG_UT_DLL_H__

#include "mig_config.h"
#include "mig_defs.h"

#if defined(WIN32)
# define mig_handle HINSTANCE
#else
# define mig_handle void*
#endif

MIG_C_LINKAGE_START

extern mig_handle
mig_dlopen ( const char *dll_name );

extern void*
mig_dlsym ( mig_handle handle , 
            const char *func_name );

extern int
mig_dlclose ( mig_handle handle );

extern char*
mig_dlerror ();

MIG_C_LINKAGE_END

#endif /* __MIG_UT_DLL_H__ */
