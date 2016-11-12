#ifndef __LIBMIGDB_H__
#define __LIBMIGDB_H__

#include "mig_config.h"
#include "mig_defs.h"
#include "mig_data_types.h"
#include "mig_data_dicom.h"
#include "mig_data_image.h"
#include "mig_error_codes.h"
#include "sqlite3.h"

/* Status codes
   for different processing
   steps inside database.
*/

typedef enum 
{
        MIG_PROC_STATUS_WAITING = 0,
        MIG_PROC_STATUS_READY   = 1,
        MIG_PROC_STATUS_RUNNING = 2,
        MIG_PROC_STATUS_DONE    = 3,
        MIG_PROC_STATUS_ERROR   = 4

} MIG_DBSTATUS;

typedef enum
{
        MIG_RECEIVE  = 0,
        MIG_PROCESS  = 1,
        MIG_SEND     = 2

} MIG_DBFIELD;

MIG_C_LINKAGE_START

typedef struct _mig_db_t 
{
        char *dbname;
        sqlite3 *db;
        const char *err;

} mig_db_t;

extern int
mig_db_init ( mig_db_t *db , 
              char *dbname );

extern int
mig_db_close ( mig_db_t *db );

extern int
mig_db_put_series ( mig_db_t *db ,
                    mig_dcm_data_t *dicom_data ,
                    mig_size_t *size_data );

extern int
mig_db_query_series ( mig_db_t *db ,
                      mig_dcm_data_t *dicom_data ,
                      int *result );

extern int
mig_db_delete_series ( mig_db_t *db , 
                       char *storage_path );


extern int
mig_db_set_status ( mig_db_t *db ,
                    char *storage_path ,
                    MIG_DBFIELD field ,
                    MIG_DBSTATUS status );

extern int
mig_db_set_date ( mig_db_t *db ,
                  char *storage_path ,
                  MIG_DBFIELD field ,
                  char *date ,
                  char *time );

extern int
mig_db_unprocessed ( mig_db_t *db , 
                     char **storage_path );

extern int
mig_db_set_dest ( mig_db_t *db ,
                  const char *storage_path  , 
                  const char *results_path );

MIG_C_LINKAGE_END


#endif /* __LIBMIGDB_H__ */
