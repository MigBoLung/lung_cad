#include "libmigdb.h"
#include "libmigut.h"

/************************************/
/* SQL STATEMENTS */
/************************************/

/*********************************************************/
/* insert a series entry into database */
static const char *_insert_series_sql =
"INSERT INTO data (\
	id, \
        patient_name,\
        patient_id,\
        study_uid,\
        study_date,\
        study_time,\
        series_uid,\
        storage,\
	slices,\
	size_stack,\
	plane_res,\
	z_res,\
        thickness )"
"VALUES( NULL , ? , ? , \
         ? , ? , ? , ? , ? , \
         ? , ? , ? , ? , ? )";

/*********************************************************/
/* query wether series is already present in database */
static const char*
_query_series_sql =
"SELECT id FROM data WHERE \
patient_id=? AND study_uid=? AND series_uid=? LIMIT 1";

/*********************************************************/
/* delete a series entry from the database */
static const char*
_delete_series_sql =
"DELETE * FROM data WHERE storage=?";

/*********************************************************/
/* get id of next unprocessed series*/
static const char*
_get_unprocessed_sql = 
"SELECT storage FROM data \
WHERE process_status=? LIMIT 1";

/*********************************************************/
static const char*
_set_receive_status_sql =
"UPDATE data SET \
receive_status=? \
WHERE storage=?";

/*********************************************************/
static const char*
_set_receive_date_sql =
"UPDATE data SET \
receive_date=?, \
receive_time=? \
WHERE storage=?";

/*********************************************************/
static const char*
_set_process_status_sql =
"UPDATE data SET \
process_status=? \
WHERE storage=?";

/*********************************************************/
static const char*
_set_process_date_sql =
"UPDATE data SET \
process_date=?, \
process_time=? \
WHERE storage=?";

/*********************************************************/
static const char*
_set_send_status_sql =
"UPDATE data SET \
send_status=? \
WHERE storage=?";

/*********************************************************/
static const char*
_set_send_date_sql =
"UPDATE data SET \
send_date=?, \
send_time=? \
WHERE storage=?";

/*********************************************************/
static const char*
_set_results_path =
"UPDATE data SET \
results=? \
WHERE storage=?";

/********************************/
/* EXPORTED FUNCTIONS */
/********************************/
int
mig_db_init ( mig_db_t *db , char *dbname )
{
        int rc , isfile;

        db->dbname = strdup( dbname );
        if ( db->dbname == NULL )
        {
                return MIG_ERROR_MEMORY;
        }

        db->err = NULL;

        rc = mig_ut_fs_isfile ( db->dbname , &isfile );
        if ( ( rc != 0 ) || ( isfile == 0 ) )
        {
                free ( db->dbname );
                db->dbname = NULL;
                return MIG_ERROR_IO;
        }

        /* try opening the database file */
        if (  sqlite3_open ( db->dbname , &(db->db) ) != 0 )
        {
                /* could not open */
                db->err = sqlite3_errmsg ( db->db );
                rc = sqlite3_close ( db->db );
                free ( db->dbname );
                return MIG_ERROR_IO;
        }
        
        return MIG_OK;
}

/********************************/
int
mig_db_close ( mig_db_t *db )
{
    int rc;

    if ( db->db )
    {
        rc = sqlite3_close ( db->db );
    }
    
    return MIG_OK;
}

/********************************/
int
mig_db_put_series ( mig_db_t *db ,
                    mig_dcm_data_t *dicom_data ,
                    mig_size_t *size_data )
{
        sqlite3_stmt *stmt = NULL;
        const char* tail;
        int rc , flag = 0;
        
        rc = mig_db_query_series ( db , dicom_data , &flag );
        if ( rc != MIG_OK )
        {
                goto error;
        }

        if ( flag == 1 )
        {
                return MIG_OK;
        }

        /* prepare sql statement */
        rc = sqlite3_prepare ( db->db , 
                               _insert_series_sql , 
                               (int)strlen(_insert_series_sql), 
                               &stmt, 
                               &tail );
        if ( rc != SQLITE_OK )
        {
                goto error;
        }

        /* patient name */
        rc = sqlite3_bind_text (  stmt , 1 , 
                                  dicom_data->patient_name , 
                                  strlen(dicom_data->patient_name) , 
                                  SQLITE_STATIC );
        if ( rc != SQLITE_OK )
        {
                goto error;
        }
                
        /* patient id */
        rc = sqlite3_bind_text (  stmt , 2 , 
                                  dicom_data->patient_id , 
                                  strlen(dicom_data->patient_id) , 
                                  SQLITE_STATIC );
        if ( rc != SQLITE_OK )
        {
                goto error;
        }
        
        /* study uid */
        rc = sqlite3_bind_text (  stmt , 3 , 
                                  dicom_data->study_uid , 
                                  strlen(dicom_data->study_uid) , 
                                  SQLITE_STATIC );
        if ( rc != SQLITE_OK )
        {
                goto error;
        }
        
        /* study date */
        rc = sqlite3_bind_text (  stmt , 4 , 
                                  dicom_data->study_date , 
                                  strlen(dicom_data->study_date) , 
                                  SQLITE_STATIC );
        if ( rc != SQLITE_OK )
        {
                goto error;
        }
        
        /* study time */
        rc = sqlite3_bind_text (  stmt , 5 , 
                                  dicom_data->study_time , 
                                  strlen(dicom_data->study_time) , 
                                  SQLITE_STATIC );
        if ( rc != SQLITE_OK )
        {
                goto error;
        }
        
        /* series uid */
        rc = sqlite3_bind_text (  stmt , 6 , 
                                  dicom_data->series_uid , 
                                  strlen(dicom_data->series_uid) , 
                                  SQLITE_STATIC );
        if ( rc != SQLITE_OK )
        {
                goto error;
        }
                
        /* storage location */
        rc = sqlite3_bind_text (  stmt , 7 , 
                                  dicom_data->storage , 
                                  strlen(dicom_data->storage) , 
                                  SQLITE_STATIC );
        if ( rc != SQLITE_OK )
        {
                goto error;
        }
        
        /* number of slices */
        rc = sqlite3_bind_int (  stmt , 8 , 
                                 size_data->slices );
        if ( rc != SQLITE_OK )
        {
                goto error;
        }
        
        /* size in bytes of stack */
        rc = sqlite3_bind_int (  stmt , 9 , 
                                 size_data->size_stack );
        if ( rc != SQLITE_OK )
        {
                goto error;
        }
        
	    /* in plane resolution */
        rc = sqlite3_bind_double (  stmt , 10 , 
                                    size_data->h_res );
        if ( rc != SQLITE_OK )
        {
                goto error;
        }
        
        /* z resolution */
        rc = sqlite3_bind_double (  stmt , 11 , 
                                    size_data->z_res );
        if ( rc != SQLITE_OK )
        {
                goto error;
        }
        
        /* slice thickness */
        rc = sqlite3_bind_double (  stmt , 12 , 
                                    size_data->thickness );
        if ( rc != SQLITE_OK )
        {
                goto error;
        }
        
        /* execute sql statement */
        do 
        {
                rc = sqlite3_step ( stmt );
        }
        while (  rc == SQLITE_BUSY );

        if ( rc != SQLITE_DONE )
        {
                goto error;
        }

        rc = sqlite3_finalize ( stmt );
        if ( rc != SQLITE_OK )
        {
                goto error;
        }
        
        return MIG_OK;

error :

        rc = sqlite3_finalize ( stmt );
        db->err = sqlite3_errmsg ( db->db );
        return MIG_ERROR_DB;
}

/********************************/
int
mig_db_query_series ( mig_db_t *db ,
                      mig_dcm_data_t *dicom_data , 
                      int *result )
{
        sqlite3_stmt *stmt;
        const char* tail;
        int rc;
        
        *result = 0;

        /* prepare sql statement */
        rc = sqlite3_prepare ( db->db , 
                               _query_series_sql , 
                               (int)strlen(_query_series_sql), 
                               &stmt, 
                               &tail );
        if ( rc != SQLITE_OK )
                goto error;
        
        /* patient id */
        rc = sqlite3_bind_text (  stmt , 1 ,
                                  dicom_data->patient_id , 
                                  strlen(dicom_data->patient_id) , 
                                  SQLITE_STATIC );
        if ( rc != SQLITE_OK )
                goto error;
        
        /* study uid */
        rc = sqlite3_bind_text (  stmt , 2 , 
                                  dicom_data->study_uid , 
                                  strlen(dicom_data->study_uid) , 
                                  SQLITE_STATIC );
        if ( rc != SQLITE_OK )
                goto error;
        
        /* series uid */
        rc = sqlite3_bind_text (  stmt , 3 , 
                                  dicom_data->series_uid , 
                                  strlen(dicom_data->series_uid) , 
                                  SQLITE_STATIC );
        if ( rc != SQLITE_OK )
                goto error;
        
        /* execute sql statement */
        do 
        {
                rc = sqlite3_step ( stmt );
        }
        while (  rc == SQLITE_BUSY );

        if ( rc == SQLITE_ROW )
                *result = 1;        
        
        rc = sqlite3_finalize ( stmt );
        if ( rc != SQLITE_OK )
                goto error;

        return MIG_OK;

error :

        rc = sqlite3_finalize ( stmt );
        db->err = sqlite3_errmsg ( db->db );
        return MIG_ERROR_DB;
}

/*******************************************************************/
int
mig_db_delete_series ( mig_db_t *db , char *path )
{
        sqlite3_stmt *stmt;
        const char* tail;
        int rc;
                
        /* prepare sql statement */
        rc = sqlite3_prepare ( db->db , 
                               _delete_series_sql , 
                               (int)strlen(_delete_series_sql), 
                               &stmt, 
                               &tail );
        if ( rc != SQLITE_OK )
                goto error;
        
        rc = sqlite3_bind_text ( stmt , 1 ,
                                 path , strlen(path) , 
                                 SQLITE_STATIC );
        if ( rc != SQLITE_OK )
                goto error;
        
        /* execute sql statement */
        do 
        {
                rc = sqlite3_step ( stmt );
        }
        while (  rc == SQLITE_BUSY );
        
        if ( rc != SQLITE_DONE )
                goto error;
        
        rc = sqlite3_finalize ( stmt );
        if ( rc != SQLITE_OK )
                goto error;

        return MIG_OK;

error :
        
        rc = sqlite3_finalize ( stmt );
        db->err = sqlite3_errmsg ( db->db );
        return MIG_ERROR_DB;
}

/*******************************************************************/
int
mig_db_set_status ( mig_db_t *db ,
                    char *storage_path ,
                    MIG_DBFIELD field ,
                    MIG_DBSTATUS status )
{
        sqlite3_stmt *stmt = NULL;
        const char* tail;
        int rc;
                
        switch ( field )
        {
                case MIG_RECEIVE :

                        rc = sqlite3_prepare ( 
                                db->db , 
                                _set_receive_status_sql , 
                                (int) strlen( _set_receive_status_sql ) ,
                                &stmt , &tail );
                        if ( rc != SQLITE_OK )
                                goto error;
                        break;

                case MIG_PROCESS :

                        rc = sqlite3_prepare ( 
                                db->db , 
                                _set_process_status_sql , 
                                (int) strlen( _set_process_status_sql ) ,
                                &stmt , &tail );
                        if ( rc != SQLITE_OK )
                                goto error;
                        break;

                case MIG_SEND :

                        rc = sqlite3_prepare ( 
                                db->db , 
                                _set_send_status_sql , 
                                (int) strlen( _set_send_status_sql ) ,
                                &stmt , &tail );
                        if ( rc != SQLITE_OK )
                                goto error;
                        break;
                
                default :

                        rc = MIG_ERROR_INTERNAL;
                        goto error;
        }
        
        /* status */
        rc = sqlite3_bind_int (  stmt , 1 , (int)status );
        if ( rc != SQLITE_OK )
                goto error;
        
        /* storage */
        rc = sqlite3_bind_text ( stmt , 2 ,
                                 storage_path , 
                                 strlen( storage_path ) , 
                                 SQLITE_STATIC );
        if ( rc != SQLITE_OK )
                goto error;
        
        do
        {
                rc = sqlite3_step ( stmt );
        } while ( rc == SQLITE_BUSY );

        rc = sqlite3_finalize ( stmt );
        if ( rc != SQLITE_OK )
                goto error;

        return MIG_OK;

error :

        rc = sqlite3_finalize ( stmt );
        db->err = sqlite3_errmsg ( db->db );
        return MIG_ERROR_DB;
}

/*******************************************************************/
int
mig_db_set_date ( mig_db_t *db ,
                  char *storage_path ,
                  MIG_DBFIELD field ,
                  char *date ,
                  char *tme )
{
        sqlite3_stmt *stmt = NULL;
        const char* tail;
        int rc;
        
        switch ( field )
        {
                case MIG_RECEIVE :

                        rc = sqlite3_prepare ( 
                                db->db , 
                                _set_receive_date_sql , 
                                (int) strlen( _set_receive_date_sql ) ,
                                &stmt , &tail );
                        if ( rc != SQLITE_OK )
                                goto error;
                        break;

                case MIG_PROCESS :

                        rc = sqlite3_prepare ( 
                                db->db , 
                                _set_process_date_sql , 
                                (int) strlen( _set_process_date_sql ) ,
                                &stmt , &tail );
                        if ( rc != SQLITE_OK )
                                goto error;
                        break;

                case MIG_SEND :

                        rc = sqlite3_prepare ( 
                                db->db , 
                                _set_send_date_sql , 
                                (int) strlen( _set_send_date_sql ) ,
                                &stmt , &tail );
                        if ( rc != SQLITE_OK )
                                goto error;
                        break;
                
                default :

                        rc = MIG_ERROR_INTERNAL;
                        goto error;
        }
        
        /* date */
        rc = sqlite3_bind_text ( stmt , 1 ,
                                 date , DATE_LEN , 
                                 SQLITE_STATIC );
        if ( rc != SQLITE_OK )
                goto error;

        /* time */
        rc = sqlite3_bind_text ( stmt , 2 ,
                                 tme , TIME_LEN , 
                                 SQLITE_STATIC );
        if ( rc != SQLITE_OK )
                goto error;
        
        /* storage */
        rc = sqlite3_bind_text ( stmt , 3 ,
                                 storage_path , 
                                 strlen( storage_path ) , 
                                 SQLITE_STATIC );
        if ( rc != SQLITE_OK )
                goto error;
        
        do
        {
                rc = sqlite3_step ( stmt );
        } while ( rc == SQLITE_BUSY );

        rc = sqlite3_finalize ( stmt );
        if ( rc != SQLITE_OK )
                goto error;

        return MIG_OK;

error :

        rc = sqlite3_finalize ( stmt );
        db->err = sqlite3_errmsg ( db->db );
        return MIG_ERROR_DB;
}

/*******************************************************************/
int
mig_db_unprocessed ( mig_db_t *db ,
                     char **path )
{
        int rc;
        sqlite3_stmt *stmt;
        const char* tail;
        char *tmp = NULL;
        

        *path = NULL;

        /* prepare sql statement */
        rc = sqlite3_prepare ( 
                db->db , _get_unprocessed_sql , 
                (int) strlen( _get_unprocessed_sql ) , 
                &stmt , &tail );
        if ( rc != SQLITE_OK )
                goto error;
        
        /* status = MIG_PROC_STATUS_READY */
        rc = sqlite3_bind_int ( stmt , 1 , 
                                MIG_PROC_STATUS_READY );
        if ( rc != SQLITE_OK )
                goto error;
                
        do
        {
                rc = sqlite3_step ( stmt );
        } while ( rc == SQLITE_BUSY );

        /* no new entries */
        if ( rc != SQLITE_ROW )
        {
                rc = sqlite3_finalize ( stmt );
                return MIG_OK;
        }
        
        /* new entry found -> copy storage location */
        tmp = (char*)
                strdup ( (const char*) 
                sqlite3_column_text( stmt , 0 ) );
        if ( tmp == NULL )
                goto error;
        
        rc = sqlite3_finalize ( stmt );
        if ( rc != SQLITE_OK )
                goto error;

        /* mark as being processed */
        rc = mig_db_set_status ( 
                db , tmp ,
                MIG_PROCESS ,
                MIG_PROC_STATUS_RUNNING );
        if ( rc != MIG_OK )
                goto error;

        *path = tmp;
        return MIG_OK;

error :

        rc = sqlite3_finalize ( stmt );
        db->err = sqlite3_errmsg ( db->db );
        
        if ( tmp )
                free ( tmp );

        return MIG_ERROR_DB;
}

/*******************************************************************/
int
mig_db_set_dest ( mig_db_t *db ,
                  const char *storage_path  , 
                  const char *results_path )
{
        sqlite3_stmt *stmt;
        const char* tail;
        int rc;
        
        /* prepare sql statement */
        rc = sqlite3_prepare ( db->db , 
                               _set_results_path ,
                               (int)strlen(_set_results_path), 
                               &stmt, 
                               &tail );
        if ( rc != SQLITE_OK )
                goto error;
        
        rc = sqlite3_bind_text ( stmt , 1 ,
                                 results_path , 
                                 strlen(results_path) , 
                                 SQLITE_STATIC );
        if ( rc != SQLITE_OK )
        {
                goto error;
        }

        rc = sqlite3_bind_text ( stmt , 2 ,
                                 storage_path , 
                                 strlen(storage_path) , 
                                 SQLITE_STATIC );
        if ( rc != SQLITE_OK )
        {
                goto error;
        }

        /* execute sql statement */
        do 
        {
                rc = sqlite3_step ( stmt );
        }
        while (  rc == SQLITE_BUSY );
        
        if ( rc != SQLITE_DONE )
        {
                goto error;
        }
        
        rc = sqlite3_finalize ( stmt );
        if ( rc != SQLITE_OK )
        {
                goto error;
        }

        return MIG_OK;

error :
        
        rc = sqlite3_finalize ( stmt );
        if ( db != NULL )
        {
            db->err = sqlite3_errmsg ( db->db );
        }

        return MIG_ERROR_DB;        
}
