#ifndef __MIG_UT_LOCK__
#define __MIG_UT_LOCK__

#include "mig_config.h"
#include "mig_defs.h"

MIG_C_LINKAGE_START

#if !defined(INVALID_HANDLE)
#	define INVALID_HANDLE	-1
#endif

typedef struct
{
#if defined(WIN32)			/* WIN32 */
	OVERLAPPED overlapped;
#else					/* LINUX */
    	struct flock lock;
#endif					/* WIN32 */

	char *lock_name;
	
	int fd;

} mig_flock_t;

extern int
mig_ut_lock_init ( mig_flock_t *lock , int fd , char *name );

extern int
mig_ut_lock_init_f ( mig_flock_t *lock , int flags , char *name , int perms );

extern int
mig_ut_lock_destroy ( mig_flock_t *lock , int unlink_file );

extern int
mig_ut_lock_unlock ( mig_flock_t *lock , short whence , off_t start , off_t len );

extern int
mig_ut_lock_r ( mig_flock_t *lock , short whence , off_t start , off_t len );

extern int
mig_ut_lock_rb ( mig_flock_t *lock , short whence , off_t start , off_t len );

extern int
mig_ut_lock_w ( mig_flock_t *lock , short whence , off_t start , off_t len );

extern int
mig_ut_lock_wb ( mig_flock_t *lock , short whence , off_t start , off_t len );

MIG_C_LINKAGE_END

#endif /* __MIG_UT_LOCK__ */


