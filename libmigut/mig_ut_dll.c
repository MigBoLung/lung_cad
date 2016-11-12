#include "mig_ut_dll.h"

#if !defined(WIN32)
# include <dlfcn.h>
#endif

/******************************************************************/
/* EXPORTS */
/******************************************************************/
mig_handle
mig_dlopen ( const char *dll_name )
{
        mig_handle handleLib;

#if defined(WIN32)
        handleLib = LoadLibrary ( dll_name );
#else        
        handleLib = dlopen ( dll_name , RTLD_LAZY );
#endif
        
        return handleLib;
}

/******************************************************************/
void*
mig_dlsym ( mig_handle handle , 
            const char *func_name )
{
#if defined(WIN32)
        return (void*) GetProcAddress( handle , func_name );
#else
        return dlsym ( handle , func_name );
#endif  
}

/******************************************************************/
int
mig_dlclose ( mig_handle handle )
{
#if defined(WIN32)

        if ( FreeLibrary( handle ) != 0 )
                return 0;
        else
                return -1;

#else
        
        if ( dlclose ( handle ) == 0 )
                return 0;
        else
                return -1;

#endif
}

/******************************************************************/
char*
mig_dlerror ()
{
#if defined(WIN32)
        
        LPVOID Buffer;        
        DWORD ErrorCode = 
                GetLastError(); 

        FormatMessage (
                FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS ,
                NULL ,
                ErrorCode ,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT) ,
                (LPTSTR) &Buffer ,
                0 , NULL );
        return ( (char*) Buffer );

#else        
        return dlerror();

#endif
}

