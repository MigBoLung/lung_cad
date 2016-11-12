#ifndef __MIG_UT_FS_H__
#define __MIG_UT_FS_H__

#include "mig_config.h"
#include "mig_defs.h"

MIG_C_LINKAGE_START

/*************************/
/* FILE OPENING */
/*************************/
#if defined(WIN32)

#define mig_open(a,b)       _open(a,b)
#define mig_close(a)        _close(a)
#define mig_write(a,b,c)    _write(a,b,c)
#define mig_read(a,b,c)     _read(a,b,c)

#define MIG_O_BINARY        _O_BINARY
#define MIG_O_TEXT          _O_TEXT

#define MIG_O_APPEND        _O_APPEND
#define MIG_O_CREAT         _O_CREAT
#define MIG_O_TRUNC         _O_TRUNC

#define MIG_O_WRONLY        _O_WRONLY
#define MIG_O_RDONLY        _O_RDONLY
#define MIG_O_RDWR	    _O_RDWR

#else

#define mig_open   	    open
#define mig_close    	    close
#define mig_write(a,b,c)    write(a,b,c)
#define mig_read(a,b,c)     read(a,b,c)

#define MIG_O_BINARY	0
#define MIG_O_TEXT	0

#define MIG_O_APPEND        O_APPEND
#define MIG_O_CREAT         O_CREAT
#define MIG_O_TRUNC         O_TRUNC

#define MIG_O_WRONLY        O_WRONLY
#define MIG_O_RDONLY        O_RDONLY
#define MIG_O_RDWR	    O_RDWR

#endif

/*************************/
/* FILE LISTS */
/*************************/
extern int
mig_ut_fs_rlist ( char *fname ,
                  char **contents , int *num );

/************************/
/* DIRECTORY PROCESSING */
/************************/
extern int
mig_ut_fs_mkdir ( const char *path_to_create );

extern int
mig_ut_fs_rmdir ( const char *path );

extern int
mig_ut_fs_isdir ( char *path , int *isdir );

extern int
mig_ut_fs_isfile ( char *path , int *isfile );

extern int
mig_ut_fs_rdir ( char *path , char *ext ,
                 char **dir_contents );

extern int
mig_uf_fs_isreadable ( char *path , int *isreadable );

extern int
mig_uf_fs_iswritable ( char *path , int *iswritable );

MIG_C_LINKAGE_END


#endif /* __MIG_UT_FS_H__ */


