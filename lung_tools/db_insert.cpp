#include "mig_config.h"
#include "mig_defs.h"
#include "mig_error_codes.h"

#undef MATLAB

#include "libmigio.h"
#include "libmigut.h"
#include "libmigdb.h"

/***************************************************/
#define APP_NAME        "db_insert"
#define APP_DESC        "Database Insert"

/***************************************************/
/* PRIVATE */
/***************************************************/

MIG_C_LINKAGE_START

static void
_usage ();

static int
_insert ( const char *entry , 
          mig_db_t *db );

MIG_C_LINKAGE_END

/***************************************************/
/* MAIN */
/***************************************************/

int
main ( int argc , char **argv )
{
        int i , rc , num;
        int flag;
        mig_db_t db;
        char* lst[MAX_DIR_CONTENTS];
        
        if ( argc != 3 )
        {
                _usage ();
                exit ( EXIT_FAILURE );
        }

        /* check wether we can open database file */
        rc = mig_db_init ( &db , argv[2] );
        if ( rc != MIG_OK )
        {
                fprintf ( stderr , "\nERROR. Opening %s..." , argv[2] );
                exit ( EXIT_FAILURE );
        }

        /* check wether second entry is file list or directory */
        rc = mig_ut_fs_isfile ( argv[1] , &flag );
        if ( rc != MIG_OK )
        {
                fprintf ( stderr , "\nERROR. Reading %s..." , argv[1] );
                exit ( EXIT_FAILURE );
        }
        
        /* given entry is file -> list of directories*/
        if ( flag == 1 )
        {
                printf ( "\n%s is a file list..." , argv[1] );
                rc = mig_ut_fs_rlist ( argv[1] , lst , &num );
                if ( rc != MIG_OK )
                {
                        fprintf ( stderr , "\nERROR. Reading list %s..." , argv[1] );
                        exit ( EXIT_FAILURE );
                }
        
                for ( i = 0 ; i < num ; ++i )
                {
                        rc = _insert ( lst[i] , &db );
                        if ( rc != MIG_OK )
                        {
                                fprintf ( stderr , "\nERROR. Inserting %s..." , lst[i] );
                                exit ( EXIT_FAILURE );
                        }
                }

                goto out;
        }

        printf ( "\n%s is a directory..." , argv[1] );
        rc = _insert ( argv[1] , &db );
        if ( rc != MIG_OK )
        {
                fprintf ( stderr , "\nERROR. Inserting %s..." , argv[1] );          
        }
        
out :
        mig_db_close ( &db );
        exit ( EXIT_SUCCESS );
}


/***************************************************/
/* PRIVATE */
/***************************************************/

static void
_usage ()
{
        printf ( "\n%s" , APP_DESC );
        printf ( "\nInsert dicom data taken from a single");
        printf ( "\ndirectory or read from list into database." );
        printf ( "\nUsage : %s data dbfile..." , APP_NAME );
        printf ( "\n" );
        printf ( "\nATTENTION : all paths should be absolute (full)..." );
}

/***************************************************/

static int
_insert ( const char *entry , 
          mig_db_t *db )
{
        int rc , flag;
        mig_dcm_data_t dicom_data;
        mig_size_t size_data;

        char curdate[DATE_LEN];
        char curtime[TIME_LEN];

        printf ( "\nInserting %s..." , entry );

        /* check if entry exists and if it is a
           directory */
        rc = mig_ut_fs_isdir ( (char*)entry , &flag );
        if ( rc != MIG_OK )
        {
                fprintf ( stderr , "\nERROR. Reading %s..." , entry );
                return -1;
        }

        if ( flag == 0 )
        {
                fprintf ( stderr , "\nERROR. %s is not a directory..." , entry );
                return -1;
        }

        snprintf ( dicom_data.storage , MAX_PATH , "%s%c" , 
                   entry , MIG_PATH_SEPARATOR );

        rc = mig_dcm_get_info ( &dicom_data , &size_data );
        if ( rc != MIG_OK )
        {
                fprintf ( stderr , "\nERROR. Reading %s..." , entry );
                return -1;
        }
        
        /* get rid of terminating slash if present */
        mig_ut_strip_sep ( (char*) &( dicom_data.storage ) );
        
        rc = mig_db_put_series ( db , &dicom_data , &size_data );
        if ( rc != MIG_OK )
        {
                fprintf ( stderr , "\nERROR. Put series %d..." , rc );
                fprintf ( stderr , "\nERROR. Db Error message : %s..." , db->err );
                return -1;
        }

        /* get current date and time */        
        rc = mig_ut_date_time ( (char*)&curdate , (char*) &curtime );
        if ( rc != MIG_OK )
        {
                fprintf ( stderr , "\nERROR. Get current date/time %d..." , rc );
                return -1;
        }

        /* set receive status */
        rc = mig_db_set_status ( db , 
                (char*) &( dicom_data.storage ) ,
                MIG_RECEIVE , MIG_PROC_STATUS_DONE );
        if ( rc != MIG_OK )
        {
                fprintf ( stderr , "\nERROR. Set receive status %d..." , rc );
                fprintf ( stderr , "\nERROR. Db Error message : %s..." , db->err );
        }
        
        /* set receive date */
        rc = mig_db_set_date ( db , 
                (char*) &( dicom_data.storage ) ,
                MIG_RECEIVE , 
                (char*)&curdate ,
                (char*)&curtime );
        if ( rc != MIG_OK )
        {
                fprintf ( stderr , "\nERROR. Set receive date-time %d..." , rc );
                fprintf ( stderr , "\nERROR. Db Error message : %s..." , db->err );
        }

        /* set process status */
        rc = mig_db_set_status ( db , 
                (char*) &( dicom_data.storage ) ,
                MIG_PROCESS , 
                MIG_PROC_STATUS_READY );
        if ( rc != MIG_OK )
        {
                fprintf ( stderr , "\nERROR. Set process status %d..." , rc );
                fprintf ( stderr , "\nERROR. Db Error message : %s..." , db->err );
        }
        
        memset ( &dicom_data , 0x00 , sizeof( mig_dcm_data_t ) );
        memset ( &size_data , 0x00 , sizeof( mig_size_t ) );

        return rc;
}
