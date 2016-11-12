#include "mig_ut_fs.h"
#include "mig_ut_mem.h"
#include "mig_error_codes.h"

#if !defined(WIN32)
#include <errno.h>
#endif

/************************************/
/* EXPORTED FUNCTIONS */
/************************************/

int
mig_ut_fs_rlist ( char *fname ,
                  char **contents ,
                  int *num )
{
	FILE *f = NULL;
	char *line = NULL;
	char *name = NULL;
	char *idx;
	int len;
	int rc;

        if ( fname == NULL )
                return MIG_ERROR_PARAM;

	*num = 0;


	f = fopen( fname , "r" );
        if ( f == NULL )
                return MIG_ERROR_IO;

	line = (char*)
		malloc ( MAX_PATH * sizeof( char ) );
	if ( line == NULL )
	{
		rc = MIG_ERROR_MEMORY;
		goto error;
	}

        /* zero contents array */
        mig_memz_fast ( contents , sizeof( char* ) * MAX_DIR_CONTENTS );

	/* start scanning file line by line */
	while ( fgets( line , MAX_PATH , f ) != NULL )
	{
		if ( ( line[0] == '#'  ) ||
		     ( line[0] == '\n' ) ||
		     ( line[0] == '\r' ) )
			continue;

		/* get rid of carriage return
		   and new line */
		
		idx = line;
		while ( ( *idx != '\0') &&
				( *idx != '\n' ) &&
		        ( *idx != '\r' ) )
			++idx;
		*idx =  0x00;
		len = strlen( line ) + 1;
		name = (char*)
			malloc ( len * sizeof( char ) );
		if ( name == NULL )
		{
			rc = MIG_ERROR_MEMORY;
			goto error;
		}

		strcpy ( name , line );
        *contents = name;
        contents ++;
        (*num) ++;

	}

	if ( ferror( f ) != 0 )
	{
		rc = MIG_ERROR_IO;
		goto error;
	}


	free ( line );
	fclose ( f );

    return MIG_OK;

error :
	if ( line )
		free ( line );

	if ( f )
		fclose ( f );

	return rc;
}

/***********************************************************/
#if defined(WIN32)

int
mig_ut_fs_mkdir ( const char *path_to_create )
{
        char *path , *slash , *drive;
        int rc = 0;

        path = strdup ( path_to_create );
        if ( path == NULL )
                return -1;

        drive = strchr ( path , MIG_DRIVE_SEPARATOR );
        if ( drive )
                slash = strchr ( drive + 2 , MIG_PATH_SEPARATOR );
        else
                slash = strchr ( path + 1 , MIG_PATH_SEPARATOR );

        while( 1 )
        {
                if ( slash )
                        *slash = '\0';

                if ( CreateDirectory ( path , NULL ) )
                        rc = 0;
                else
                {
                        if ( GetLastError() ==
                             ERROR_ALREADY_EXISTS )
                                rc = 0;
                        else
                                rc = -1;
                }

                if ( rc )
                        break;

                if ( slash == NULL )
                        break;

                *slash = MIG_PATH_SEPARATOR;
                slash = strchr ( slash + 1 , MIG_PATH_SEPARATOR );
      }

    free ( path );
    return ( rc );
}

/***********************************************************/
#else
/***********************************************************/

int
mig_ut_fs_mkdir ( const char *path_to_create )
{
        int rc = 0;
        char *opath;
        char *p;
        unsigned int len;

        opath = strdup ( path_to_create );
        if ( opath == NULL )
                return -1;

        len = strlen ( opath );
        
        if( opath[len - 1] == '/' )
                opath[len - 1] = '\0';
        
        for ( p = opath ; *p ; ++p )
        	if ( *p == '/' )
                {
                        *p = '\0';
                        if ( access ( opath , F_OK ) )
                                mkdir( opath , S_IRWXU );
                        *p = '/';
                }
        if ( access ( opath , F_OK ) )
                mkdir ( opath , S_IRWXU);
       
        free ( opath ); 
        return rc;
}

#endif

/***********************************************************/
#if defined(WIN32)
/***********************************************************/
int
mig_ut_fs_rmdir ( const char *path )
{
        int rc = 0;
        char *copy_path;

        copy_path = strdup ( path );
        if ( copy_path )
        {
                if ( RemoveDirectory ( copy_path ) )
                        rc = 0;
                else
                        rc = -1;

                free ( copy_path );
        }

        return ( rc );
}

/***********************************************************/
#else
/***********************************************************/
int
mig_ut_fs_rmdir ( const char *path )
{
        if ( rmdir ( path ) == 0 )
        	return MIG_OK;
	else
        	return -1;
}

#endif

/************************************/
#if defined(WIN32)
/************************************/
int
mig_ut_fs_isdir ( char *path , int *isdir )
{
        DWORD dwAttrs;

        if ( path == NULL )
                return MIG_ERROR_PARAM;

        dwAttrs = GetFileAttributes( path );

        if ( dwAttrs == 0xFFFFFFFF )
        {
                *isdir = 0;
                return MIG_ERROR_IO;
        }

        if ( dwAttrs & FILE_ATTRIBUTE_DIRECTORY )
                *isdir= 1;
         else
                *isdir = 0;

        return MIG_OK;
}

/************************************/
#else   /* LINUX */
/************************************/

int
mig_ut_fs_isdir ( char *path , int *isdir )
{
        DIR *d;

        if ( path == NULL )
                return MIG_ERROR_PARAM;

        d = opendir( path );
        if ( d == NULL )
                *isdir = 0;
        else
        {
                *isdir = 1;
                closedir( d );
        }

        return MIG_OK;
}

#endif /* LINUX */

/************************************/
#if defined(WIN32)
/************************************/

int
mig_ut_fs_isfile ( char *path , int *isfile )
{
        DWORD dwAttrs;

        if ( path == NULL )
                return MIG_ERROR_PARAM;

        dwAttrs = GetFileAttributes( path );

        if ( dwAttrs == 0xFFFFFFFF )
        {
                *isfile = 0;
                return MIG_ERROR_IO;
        }

        if ( dwAttrs & FILE_ATTRIBUTE_DIRECTORY )
                *isfile = 0;
         else
                *isfile = 1;

        return MIG_OK;        
}

/************************************/
#else   /* LINUX */
/************************************/

int
mig_ut_fs_isfile ( char *path , int *isfile )
{
        DIR *d;

        if ( path == NULL )
                return MIG_ERROR_PARAM;

        d = opendir( path );
        if ( d == NULL )
                *isfile = 1;
        else
        {
                *isfile = 0;
                closedir( d );
        }

        return MIG_OK;
}

#endif /* LINUX */

/************************************/
#if defined(WIN32)
/************************************/

int
mig_ut_fs_rdir ( char *path , char *ext ,
                 char **dir_contents )
{
        BOOL                    finished;
        HANDLE                  list;
        TCHAR                   dir_name[MAX_PATH];
        WIN32_FIND_DATA         file_data;
        int                     len;
        int                     num_read = 0;
        char                    *full_name = NULL;

        if ( ( path == NULL ) ||
             ( ext == NULL ) ||
             ( dir_contents == NULL ) )
                return MIG_ERROR_PARAM;

        /* zero dir_contents array */
        mig_memz( dir_contents , sizeof( char* ) * MAX_DIR_CONTENTS );

        /* read all files such that *.ext is in the name */
        sprintf ( dir_name , "%s\\*%s" , path , ext );

        list = FindFirstFile ( dir_name , &file_data );
        if ( list == INVALID_HANDLE_VALUE )
                return MIG_ERROR_IO;

        finished = FALSE;
        while ( !finished )
        {
                /* skip directories */
                if ( !( file_data.dwFileAttributes &
                        FILE_ATTRIBUTE_DIRECTORY  ) )
                {
                        if ( num_read >= MAX_DIR_CONTENTS )
                        {
                                FindClose( list );
                                return MIG_ERROR_IO;
                        }

                        len = strlen ( file_data.cFileName ) + 1;
                        full_name = ( char* )
                                malloc ( sizeof(char) * len );

                        if ( full_name == NULL )
                        {
                                FindClose( list );
                                return MIG_ERROR_MEMORY;
                        }

                        strcpy ( full_name , file_data.cFileName );
                        *dir_contents = full_name;
                        dir_contents ++;
                        num_read ++;
                }

                if ( !FindNextFile( list , &file_data ) )
                        if ( GetLastError() == ERROR_NO_MORE_FILES )
                                finished = TRUE;
        }

        FindClose( list );
        return MIG_OK;
}

/************************************/
#else   /* LINUX */
/************************************/

int
mig_ut_fs_rdir ( char *path , char *ext ,
                 char **dir_contents )
{
        DIR *dp;
        int num_read = 0;
        char *idx;
        int len;
        char *full_name , *wd , *ptr;

        struct dirent *dir_entry;
        struct stat stat_info;

        if ( ( path == NULL ) ||
             ( ext == NULL ) ||
             ( dir_contents == NULL ) )
                return MIG_ERROR_PARAM;

        /* zero dir_contents array */
        bzero ( dir_contents ,
                sizeof( char* ) * MAX_DIR_CONTENTS );

        /* save current working directory */
        len = pathconf( "." , _PC_PATH_MAX );
        wd = ( char*) malloc ( len );
        if ( wd == NULL )
                return MIG_ERROR_MEMORY;

        ptr = getcwd( wd , len );
        if ( ptr == NULL )
                return MIG_ERROR_IO;

        if ( chdir( path ) != 0 )
                return MIG_ERROR_IO;

        if ( ( dp = opendir( "." ) ) == NULL )
        {
                chdir( wd );
                return MIG_ERROR_IO;
        }

        while ( ( dir_entry = readdir( dp ) ) != NULL )
        {
                if ( num_read >= MAX_DIR_CONTENTS )
                {
                        closedir( dp );
                        chdir( wd );
                        return MIG_ERROR_IO;
                }

                lstat( dir_entry->d_name , &stat_info );

                if ( S_ISDIR( stat_info.st_mode ) == 1 )
                        continue;

                idx = strstr( dir_entry->d_name , ext );
                if ( idx == NULL )
                        continue;

                len = strlen( dir_entry->d_name ) + 1;
                full_name = (char*) malloc( len * sizeof( char ) );
                if ( full_name == NULL )
                {
                        closedir( dp );
                        chdir( wd );
                        return MIG_ERROR_MEMORY;
                }

                strcpy ( full_name , dir_entry->d_name );
                *dir_contents = full_name;
                dir_contents ++;
                num_read ++;
    }

        closedir( dp );
        chdir( wd );
        free ( wd );

        return MIG_OK;
}

/************************************/
#endif /* LINUX */
/************************************/

/************************************/
#if defined(WIN32)	/* WINDOWS */
/************************************/
int
mig_uf_fs_isreadable ( char *path , int *isreadable )
{
	DWORD FileAttributes;
	*isreadable = 0;

	if ( path == NULL )
		return MIG_ERROR_PARAM;

	FileAttributes = GetFileAttributes( path );

	if ( FileAttributes == INVALID_FILE_ATTRIBUTES )
		return MIG_ERROR_IO;

	*isreadable = 1;
	return MIG_OK;
}

/**********************************************************************/
int
mig_uf_fs_iswritable ( char *path , int *iswritable )
{
	DWORD FileAttributes;
	*iswritable = 0;

	if ( path == NULL )
		return MIG_ERROR_PARAM;

	FileAttributes = GetFileAttributes( path );

	if ( FileAttributes == INVALID_FILE_ATTRIBUTES )
		return MIG_ERROR_IO;

	if ( FileAttributes &  FILE_ATTRIBUTE_READONLY )
		return MIG_OK;

	*iswritable = 1;
	return MIG_OK;
}

/************************************/
#else /* LINUX */
/************************************/
int
mig_uf_fs_isreadable ( char *path , int *isreadable )
{
	*isreadable = 0;

	if ( path == NULL )
		return MIG_ERROR_PARAM;

	if ( access ( path , R_OK ) == -1 )
	{
		switch ( errno )
		{
			case EACCES :
				return MIG_OK;
				break;
			default :
				return MIG_ERROR_IO;
				break;
		}
	}

	*isreadable = 1;
	return MIG_OK;
}

/**********************************************************************/
int
mig_uf_fs_iswritable ( char *path , int *iswritable )
{
	*iswritable = 0;

	if ( path == NULL )
		return MIG_ERROR_PARAM;

	if ( access ( path , W_OK ) == -1 )
	{
		switch ( errno )
		{
			case EACCES :
				return MIG_OK;
				break;
			default :
				return MIG_ERROR_IO;
				break;
		}
	}

	*iswritable = 1;
	return MIG_OK;
}

/************************************/
#endif /* LINUX */
/************************************/


