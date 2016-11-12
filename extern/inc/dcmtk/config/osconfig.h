#ifdef __cplusplus
#define BEGIN_EXTERN_C extern "C" {
#define END_EXTERN_C }
#else
#define BEGIN_EXTERN_C
#define END_EXTERN_C
#endif

#define HAVE_CONFIG_H

#if defined(WIN32)  		/* WINDOWS*/
 #if defined(X86_32)
  #include "dcmtk/config/cfwin32.h"
 #else
  #include "dcmtk/config/cfwin64.h"
 #endif
#else				/* LINUX */
 #if defined(X86_32)
  #include "dcmtk/config/cfunix32.h"
 #else
  #include "dcmtk/config/cfunix64.h"
 #endif
#endif

