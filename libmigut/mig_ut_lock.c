#include "mig_ut_lock.h"
#include "mig_ut_fs.h"
#include "mig_error_codes.h"

#if defined(WIN32)		/* WIN32 */
# include <io.h>
# include <sys/locking.h>
#else				/* LINUX */
# include <fcntl.h>
# include <errno.h>
#endif				/* WIN32 */

/*****************************************************************************/
/* PRIVATE FUNCTIONS */
/*****************************************************************************/

#if defined(WIN32)

static int
_flock_adjust ( mig_flock_t *lock , short whence , off_t *start , off_t *len )
{
	HANDLE h = (HANDLE) _get_osfhandle ( lock->fd );
	if ( h == (HANDLE) INVALID_HANDLE )
		return MIG_ERROR_INVALID_HANDLE;
	
	switch ( whence )
	{
    		case SEEK_SET :
			break;

		case SEEK_CUR :
			*start += SetFilePointer ( h ,
						   0 , 0 ,
						   FILE_CURRENT );
      			break;

		case SEEK_END :
      			*start += GetFileSize ( h , 0 );
			break;
	}

  	lock->overlapped.Offset = *start;
	if ( *len == 0 )
		*len = GetFileSize ( h , 0 ) - *start;

	return MIG_OK;
}
#endif /* WIN32 */

/*****************************************************************************/
/* EXPORTED FUNCTIONS */
/*****************************************************************************/

int
mig_ut_lock_init ( mig_flock_t *lock , int fd , char *name )
{

#if defined(WIN32)

  	lock->overlapped.Internal     = 0;
  	lock->overlapped.InternalHigh = 0;
  	lock->overlapped.OffsetHigh   = 0;
  	lock->overlapped.hEvent       = 0;

#endif

  	lock->fd = fd;
  	lock->lock_name = NULL;

	if ( name != NULL )
	{
		lock->lock_name = strdup ( name );
		if ( lock->lock_name == NULL )
		{
			lock->fd = INVALID_HANDLE;
			return MIG_ERROR_MEMORY;
		}
	}

	return MIG_OK;
}

/*****************************************************************************/
int
mig_ut_lock_init_f ( mig_flock_t *lock , int flags , char *name , int perms )
{

#if defined(WIN32)

  	lock->overlapped.Internal     = 0;
  	lock->overlapped.InternalHigh = 0;
  	lock->overlapped.OffsetHigh   = 0;
  	lock->overlapped.hEvent       = 0;

#endif

  	lock->fd = INVALID_HANDLE;
  	lock->lock_name = NULL;

	if ( name != NULL )
	{
#if defined(WIN32)
		lock->fd = mig_open ( name , flags );
#else
                lock->fd = mig_open ( name , flags , perms );
#endif
		
                if ( lock->fd != INVALID_HANDLE )
		{
        		lock->lock_name = strdup ( name );
			if ( lock->lock_name == NULL )
			{
				mig_close ( lock->fd );
				lock->fd = INVALID_HANDLE;
				return MIG_ERROR_MEMORY;
			}
		}

		return ( lock->fd == INVALID_HANDLE ? MIG_ERROR_INVALID_HANDLE : MIG_OK );
	}

	return MIG_OK;
}

/*****************************************************************************/
int
mig_ut_lock_destroy ( mig_flock_t *lock , int unlink_file )
{
	if ( lock->fd != INVALID_HANDLE )
	{
		mig_close ( lock->fd );
		lock->fd = INVALID_HANDLE;

		if ( lock->lock_name != 0 )
		{
			if ( unlink_file )
            			unlink ( lock->lock_name );
          		free ( lock->lock_name );
		}

		lock->lock_name = NULL;
	}

	return MIG_OK;
}

/*****************************************************************************/
int
mig_ut_lock_unlock ( mig_flock_t *lock , short whence , off_t start , off_t len )
{
	int status;

#if defined(WIN32)	/* WIN32 */
	
	status = _flock_adjust ( lock , whence , &start , &len );
	if ( status != MIG_OK )
		return status;

  	status = UnlockFile ( (HANDLE)_get_osfhandle ( lock->fd ) , 
		              lock->overlapped.Offset ,
		              0 , len , 0 );
	if ( status == 0 )
	{
#if defined(DEBUG)
		status = GetLastError();
#endif
		return MIG_ERROR_UNLOCK;
	}
	else
	{
		return MIG_OK;
	}

#else			/* LINUX */

  	lock->lock.l_whence = whence;
  	lock->lock.l_start  = start;
	lock->lock.l_len    = len;
	lock->lock.l_type   = F_UNLCK;
	
	status = fcntl ( lock->fd , F_SETLK , &( lock->lock ) );
	return ( ( status == -1 ) ? MIG_ERROR_LOCK : MIG_OK );

#endif			/* WIN32 */

}

/*****************************************************************************/
int
mig_ut_lock_r ( mig_flock_t *lock , short whence , off_t start , off_t len )
{
	int status;

#if defined(WIN32)	/* WINDOWS */

	status = _flock_adjust ( lock , whence , &start , &len );
	if ( status != MIG_OK )
		return status;

	status = LockFileEx ( (HANDLE)_get_osfhandle ( lock->fd ) , 
			      LOCKFILE_FAIL_IMMEDIATELY ,
		              0 , len , 0 , &lock->overlapped );
	if ( status == 0 )
	{
		status = GetLastError();
		return ( ( status == ERROR_BUSY ) ? MIG_ERROR_BUSY : MIG_ERROR_LOCK );
	}
	else
		return MIG_OK;

#else			/* LINUX */

	lock->lock.l_whence = whence;
	lock->lock.l_start  = start;
	lock->lock.l_len    = len;
	lock->lock.l_type   = F_RDLCK;

	status = fcntl ( lock->fd , F_SETLK , &( lock->lock ) );
	if ( status == -1  &&
	     ( errno == EACCES || 
	       errno == EAGAIN ) )
		return MIG_ERROR_BUSY;

	return (( status == -1 ) ? MIG_ERROR_LOCK : MIG_OK );

#endif /* WIN32 */

}

/*****************************************************************************/
int
mig_ut_lock_rb ( mig_flock_t *lock , short whence , off_t start , off_t len )
{
	int status;

#if defined (WIN32)			/* WIN32 */

	status = _flock_adjust ( lock , whence , &start , &len );
	if ( status != MIG_OK )
		return status;

	status = LockFileEx ( (HANDLE)_get_osfhandle ( lock->fd ) , 
		              0 , 0 , len , 0 , &lock->overlapped );
	if ( status == 0 )
	{
#if defined(DEBUG)
		status = GetLastError();
#endif
		return MIG_ERROR_LOCK;
	}
	else
	{
		return MIG_OK;
	}

#else					/* LINUX */

  	lock->lock.l_whence = whence;
  	lock->lock.l_start  = start;
  	lock->lock.l_len    = len;
  	lock->lock.l_type   = F_RDLCK;

	status = fcntl ( lock->fd , F_SETLKW , &( lock->lock ) );
	return ( ( status == -1 ) ? MIG_ERROR_LOCK : MIG_OK );

#endif /* WIN32 */

}

/*****************************************************************************/
int
mig_ut_lock_w ( mig_flock_t *lock , short whence , off_t start , off_t len )
{
	int status;

#if defined(WIN32)			/* WIN32 */

	status = _flock_adjust ( lock , whence , &start , &len );
	if ( status != MIG_OK )
		return status;

        status = LockFileEx ( (HANDLE)_get_osfhandle ( lock->fd ) ,
        		      LOCKFILE_FAIL_IMMEDIATELY |
        		      LOCKFILE_EXCLUSIVE_LOCK ,
			      0 , len , 0 , &lock->overlapped );
	if ( status == 0 )
	{
		status = GetLastError();
		return ( ( status == ERROR_BUSY ) ? MIG_ERROR_BUSY : MIG_ERROR_LOCK );
	}
	else
		return MIG_OK;

#else					/* LINUX */

	lock->lock.l_whence = whence;
	lock->lock.l_start  = start;
	lock->lock.l_len    = len;
	lock->lock.l_type   = F_WRLCK;

	status = fcntl ( lock->fd ,
                         F_SETLK ,
			 ( &lock->lock ) );

	if ( status == -1  &&
	     ( errno == EACCES || 
	       errno == EAGAIN ) )
		return MIG_ERROR_BUSY;

	return (( status == -1 ) ? MIG_ERROR_LOCK : MIG_OK );

#endif /* WIN32 */
}

/*****************************************************************************/
int
mig_ut_lock_wb ( mig_flock_t *lock , short whence , off_t start , off_t len )
{
	int status;

#if defined (WIN32)				/* WIN32 */

	status = _flock_adjust ( lock , whence , &start , &len );
	if ( status != MIG_OK )
		return status;

	status = LockFileEx ( (HANDLE)_get_osfhandle ( lock->fd ) ,
			     LOCKFILE_EXCLUSIVE_LOCK ,
			     0 , len , 0 , &lock->overlapped );
	if ( status == 0 )
	{
#if defined(DEBUG)
		status = GetLastError();
#endif

		return MIG_ERROR_LOCK;
	}
	else
	{
		return MIG_OK;
	}

#else						/* LINUX */

	lock->lock.l_whence = whence;
	lock->lock.l_start  = start;
	lock->lock.l_len    = len;
	lock->lock.l_type   = F_WRLCK;

	status = fcntl ( lock->fd , F_SETLKW , &( lock->lock ) );
  	return ( ( status == -1 ) ? MIG_ERROR_LOCK : MIG_OK );

#endif						/* WIN32 */

}



