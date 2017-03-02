#include "pthread.h"
#include "mig_cad.h"

#include "libmigut.h"
#include "libmigst.h"
#include "libmigim.h"
#include "libmigio.h"
#include "libmigdb.h"
#include "libmigtag.h"

MIG_C_LINKAGE_START

/***********************************************************/
/* private definitions */
/***********************************************************/

/* single entry in the "to be processed" queue */
typedef struct 
__queue_entry_t
{
    /* path to directory containing DICOM images */
    char *InputPath;
        
    /* processing final retunr code != MIG_OK if error */
    int ErrorCode;

    /* output directory */
    char *ResultsFileName;

} _queue_entry_t;

/***********************************************************/
/* PRIVATE VARS */
/***********************************************************/

/* variables whose values come from the ini file */
static char *_DatabaseFile              = DEFAULT_PARAM_CAD_DB_FILE;

static int   _MaxQueueLen               = DEFAULT_PARAM_CAD_QUEUE_LEN;
static int   _RetryReadInterval         = DEFAULT_PARAM_CAD_RETRY_READ;
static int   _RetryWriteInterval        = DEFAULT_PARAM_CAD_RETRY_WRITE;

static char *_DicomLoadDLL              = NULL;

static int   _FlagPerformSegmentation   = DEFAULT_PARAM_CAD_SEGMENT;
static char *_SegmentationDLL           = NULL;

static int   _FlagPerformDetection      = DEFAULT_PARAM_CAD_DETECT;
static char *_DetectionDLL              = NULL;

static int   _FlagPerformFpr1           = DEFAULT_PARAM_FPR1;
static char *_Fpr1DLL                   = NULL;

static int   _FlagPerformFpr2           = DEFAULT_PARAM_FPR2;
static char *_Fpr2DLL                   = NULL;

/* logger for the whole cad */
static Logger _CadLogger = Logger::getInstance ( CAD_LOGGER_NAME );

/* holds all important information and data for the whole cad. */
static mig_cad_data_t _CadData;

/* input queue - not yet processed data */
static mig_queue_t _InputQueue;

/* output queue - already processed data */
static mig_queue_t _OutputQueue;

/* input queue reader thread */
static pthread_t _InputReader;

/* output queue writer thread */
static pthread_t _OutputWriter;

/* processing functions */

/* dicom loading */
static mig_init_f       _InitDicomLoading     = NULL;
static mig_run_f        _RunDicomLoading      = NULL;
static mig_cleanup_f    _CleanupDicomLoading  = NULL;
//static mig_info_f       _InfoDicomLoading     = NULL;

/* segmentation */
static mig_init_f       _InitSegmentation       = NULL;
static mig_run_f        _RunSegmentation        = NULL;
static mig_cleanup_f    _CleanupSegmentation    = NULL;
//static mig_info_f       _InfoSegmentation       = NULL;

/* detection */
static mig_init_f       _InitDetection          = NULL;
static mig_run_f        _RunDetection           = NULL;
static mig_cleanup_f    _CleanupDetection       = NULL;
//static mig_info_f       _InfoDetection          = NULL;

/* fpr1 */
static mig_init_f       _InitFpr1               = NULL;
static mig_run_f        _RunFpr1                = NULL;
static mig_cleanup_f    _CleanupFpr1            = NULL;
//static mig_info_f       _InfoFpr1               = NULL;

/* fpr2 */
static mig_init_f       _InitFpr2               = NULL;
static mig_run_f        _RunFpr2                = NULL;
static mig_cleanup_f    _CleanupFpr2            = NULL;
//static mig_info_f       _InfoFpr2               = NULL;

/* function to cleaup global structure */
static void
_cleanup_global_data ();

/* output path for cad results */
static char *_OutputPath = NULL;

/* full name for current detection results : tag file name */
static char _OutputName[MAX_PATH];

/***********************************************************/
/* PRIVATE FUNCTIONS */
/***********************************************************/

/* read "to process" data from database */
static void*
_db_reader ( void* arg );

/* write "processed" data to database */
static void*
_db_writer ( void* arg );

static int 
_init_dicom_loader ();

static int 
_init_segementation ();

static int 
_init_detection ();

static int 
_init_fpr1 ();

static int 
_init_fpr2 ();

//static void
//_freeResults  ( mig_lst_t *list , mig_lst_free_f free_data , mig_lst_free_f free_node );

MIG_C_LINKAGE_END

/***********************************************************/
/* EXPORTS */
/***********************************************************/

int
mig_cad_init ( mig_dic_t *params )
{
    int rc;
        
    LOG4CPLUS_DEBUG ( _CadLogger , "mig_cad_init" );

    /*********************************************/
    /* parameters from ini file */
    /*********************************************/
	
    /* sqlite database file */
    _DatabaseFile = mig_ut_ini_getstring ( params , PARAM_CAD_DB_FILE , DEFAULT_PARAM_CAD_DB_FILE );

    /* results directory */
    _OutputPath = mig_ut_ini_getstring ( params , PARAM_CAD_DIR_OUT , DEFAULT_PARAM_CAD_DIR_OUT );

    /* internal queue lenght */
    _MaxQueueLen = mig_ut_ini_getint ( params , PARAM_CAD_QUEUE_LEN , DEFAULT_PARAM_CAD_QUEUE_LEN );

    /* how often to check database file for new entries */
    _RetryReadInterval = mig_ut_ini_getint ( params , PARAM_CAD_RETRY_READ , DEFAULT_PARAM_CAD_RETRY_READ );

    /* if database file is locked retry to write cad results using this interval in seconds */
    _RetryWriteInterval = mig_ut_ini_getint ( params , PARAM_CAD_RETRY_WRITE , DEFAULT_PARAM_CAD_RETRY_WRITE );
        
    /* dicom loading dll */
    _DicomLoadDLL = mig_ut_ini_getstring ( params , PARAM_CAD_LOAD_DLL , NULL );

    /* segmentation dll */
    _FlagPerformSegmentation = mig_ut_ini_getint ( params , PARAM_CAD_SEGMENT , DEFAULT_PARAM_CAD_SEGMENT );

    _SegmentationDLL = mig_ut_ini_getstring ( params , PARAM_CAD_SEGMENT_DLL , NULL );

    /* detection dll */
    _FlagPerformDetection = mig_ut_ini_getint ( params , PARAM_CAD_DETECT , DEFAULT_PARAM_CAD_DETECT );

    _DetectionDLL = mig_ut_ini_getstring ( params , PARAM_CAD_DETECT_DLL , NULL );
    
    /* fpr1 dll */
    _FlagPerformFpr1 = mig_ut_ini_getint ( params , PARAM_FPR1 , DEFAULT_PARAM_FPR1 );

    _Fpr1DLL = mig_ut_ini_getstring ( params , PARAM_FPR1_DLL , NULL );
    
	/* fpr2 dll */
    _FlagPerformFpr2 = mig_ut_ini_getint ( params , PARAM_FPR2 , DEFAULT_PARAM_FPR2 );

    _Fpr2DLL = mig_ut_ini_getstring ( params , PARAM_FPR2_DLL , NULL );

    /***********************************************/
    /* next enrey to process / save results queues */
    /***********************************************/
    rc = mig_queue_init ( &_InputQueue , _MaxQueueLen );
    if ( rc != MIG_OK )
        return rc;

    rc = mig_queue_init ( &_OutputQueue , _MaxQueueLen );
    if ( rc != MIG_OK )
        return rc;

    /* initialize dicom loading */
    rc = _init_dicom_loader ();
    if ( rc != MIG_OK )
        return rc;

    /* initialize segmentation */
    rc = _init_segementation ();
    if ( rc != MIG_OK )
        return rc;

    /* initialize detection */
    rc = _init_detection ();
    if ( rc != MIG_OK )
        return rc;

    /* initialize false positive reduction 1 */
    rc = _init_fpr1 ();
    if ( rc != MIG_OK )
        return rc;
    
    /* initialize false positive reduction 2 */
    rc = _init_fpr2 ();
    if ( rc != MIG_OK )
        return rc;
    
    /*********************************************/
    /* make directory for results */
    /*********************************************/
        
    /* if output directory does not exist create it */
    mig_ut_fs_mkdir ( _OutputPath );

    /*********************************************/
    /* initialize dicom loading */
    /*********************************************/
    rc = _InitDicomLoading ( params , &_CadData );
    if ( rc != MIG_OK )
        return rc;

    /*********************************************/
    /* initialize segmentation */
    /*********************************************/
    if ( _FlagPerformSegmentation == 1 )
    {
        rc = _InitSegmentation ( params , &_CadData );
        if ( rc != MIG_OK )
            return rc;
    }

    /*********************************************/
    /* initialize detection */
    /*********************************************/
    if ( _FlagPerformDetection == 1 )
    {
        rc = _InitDetection ( params , &_CadData );
        if ( rc != MIG_OK )
            return rc;
    }

    /*********************************************/
    /* initialize fpr1 */
    /*********************************************/
    if ( _FlagPerformFpr1 == 1 )
    {
        rc = _InitFpr1 ( params , &_CadData );
        if ( rc != MIG_OK )
            return rc;
    }

    /*********************************************/
    /* initialize fpr2 */
    /*********************************************/
    if ( _FlagPerformFpr2 == 1 )
    {
        rc = _InitFpr2 ( params , &_CadData );
        if ( rc != MIG_OK )
            return rc;
    }

    return MIG_OK;
}

/***********************************************************/
int
mig_cad_run ()
{
    time_t t0 , t1;
    time_t tot0 , tot1;

    int rc = MIG_OK;
    mig_db_t db_data;               /* database connection */
    _queue_entry_t *CurrEntry;      /* current entry to process */
        
    LOG4CPLUS_DEBUG ( _CadLogger , "mig_cad_run" );
   
    /* initialize database connection */
    rc = mig_db_init ( &db_data , _DatabaseFile );
    if ( rc != MIG_OK )
    {
        LOG4CPLUS_FATAL ( _CadLogger , "mig_cad_run database error. Aborting..." );
        return rc;
    }
   
    /* spawn input reader thread */
    pthread_create ( &_InputReader , NULL , _db_reader , NULL );
   
    /* spawn output writer thread */
    pthread_create ( &_OutputWriter , NULL , _db_writer , NULL );
   
    /* MAIN LOOP -> forever */
    while ( 1 )
    {
        /* get next entry to process from input list */
        CurrEntry = (_queue_entry_t *) mig_queue_get ( &_InputQueue );
        if ( CurrEntry == NULL )
        {
            LOG4CPLUS_FATAL ( _CadLogger , "mig_cad_run -> mig_queue_get retreaved a NULL. Aborting..." );
            return MIG_ERROR_INTERNAL;
        }      

        CurrEntry->ErrorCode = MIG_OK;
        LOG4CPLUS_INFO ( _CadLogger , " Next entry to process : " << CurrEntry->InputPath );

        /* start global timer */
        tot0 = getticks_sys();

        /*********************************************/
        /* zero out cad data structure */
        /*********************************************/
        _cleanup_global_data ();
        
        /*************************************************/
        /* DICOM LOADING */
        /*************************************************/
      
        /* prepare for loading dicom directory from disk */
        snprintf ( _CadData.dicom_data.storage , MAX_PATH , "%s" , CurrEntry->InputPath );

        t0 = getticks_sys();
        rc = _RunDicomLoading ();
        if ( rc != MIG_OK )
        {
            LOG4CPLUS_FATAL ( _CadLogger , " Dicom loading returned : " <<  rc );
            LOG4CPLUS_FATAL ( _CadLogger , " Trying to continue... " );
            goto cleanup;
        }
        t1 = getticks_sys();
        LOG4CPLUS_INFO ( _CadLogger , "DICOM LOADER TIMING : " << elapsed_sys( t1 , t0 ) << " secs." );
      
        /*************************************************/
        /* SEGMENTATION */
        /*************************************************/
        if ( _FlagPerformSegmentation == 1 )
        {
            t0 = getticks_sys();
            rc = _RunSegmentation ();
            if ( rc != MIG_OK )
            {
                LOG4CPLUS_FATAL ( _CadLogger , " Segmenation returned : " <<  rc );
                LOG4CPLUS_FATAL ( _CadLogger , " Trying to continue... " );
                goto cleanup;
            }
            t1 = getticks_sys();
            LOG4CPLUS_INFO ( _CadLogger , "SEGMENTATION TIMING : " << elapsed_sys( t1 , t0 ) << " secs." );
        }
      
        /*************************************************/
        /* DETECTION */
        /*************************************************/
        if ( _FlagPerformDetection == 1 )
        {
            t0 = getticks_sys();
            rc = _RunDetection ();
            if ( rc != MIG_OK )
            {
                LOG4CPLUS_FATAL ( _CadLogger , " Detection returned : " <<  rc );
                LOG4CPLUS_FATAL ( _CadLogger , " Trying to continue... " );
                goto cleanup;
            }
            t1 = getticks_sys();
            LOG4CPLUS_INFO ( _CadLogger , "DETECTION TIMING : " << elapsed_sys( t1 , t0 ) << " secs." );
        }

        /*************************************************/
        /* FPR1 */
        /*************************************************/
        if ( _FlagPerformFpr1 == 1 )
        {
            t0 = getticks_sys();
            rc = _RunFpr1 ();
            if ( rc != MIG_OK )
            {
                LOG4CPLUS_FATAL ( _CadLogger , " FPR1 returned : " <<  rc );
                LOG4CPLUS_FATAL ( _CadLogger , " Trying to continue... " );
                goto cleanup;
            }
            t1 = getticks_sys();
            LOG4CPLUS_INFO ( _CadLogger , "FPR1 TIMING : " << elapsed_sys( t1 , t0 ) << " secs." );
        }
      
        /*************************************************/
        /* FPR2 */
        /*************************************************/
        if ( _FlagPerformFpr2 == 1 )
        {
            t0 = getticks_sys();
            rc = _RunFpr2 ();
            if ( rc != MIG_OK )
            {
                LOG4CPLUS_FATAL ( _CadLogger , " FPR2 returned : " <<  rc );
                LOG4CPLUS_FATAL ( _CadLogger , " Trying to continue... " );
                goto cleanup;
            }
            t1 = getticks_sys();
            LOG4CPLUS_INFO ( _CadLogger , "FPR2 TIMING : " << elapsed_sys( t1 , t0 ) << " secs." );
        }
      
        /*************************************************/
        /* OUTPUT DETECTION RESULTS */
        /*************************************************/
      
        /* merge left and right lung lists into a single list */
        mig_lst_cat ( &( _CadData.det_r ) , &( _CadData.results ) );
        mig_lst_cat ( &( _CadData.det_l ) , &( _CadData.results ) );
        
        /* no results -> do nothing */
		/* GF 20100930 no results IS A result, also for checking sake and visualizer,
			it's better to write a tag file with 0 elements. */
        /*
			if ( mig_lst_len ( &( _CadData.results ) ) == 0 )
            goto cleanup;
		*/
      
        /* build output tag file name */
		/* we want results based both on original data and resized */

		if ( _CadData.resampled )
		{
			/* resized: no additional process is needed */
			//snprintf ( _OutputName , MAX_PATH , "%s%s_res.tag" , _OutputPath , _CadData.dicom_data.patient_id );
			snprintf ( _OutputName , MAX_PATH , "%s%s_%s_%s_res.tag" , _OutputPath ,
				_CadData.dicom_data.patient_id ,
				_CadData.dicom_data.study_uid ,
				_CadData.dicom_data.series_uid );
		
			/* write results to tag file */
			rc = mig_tag_write ( _OutputName , &( _CadData.results ) );
			if ( rc != MIG_OK )
			{
				LOG4CPLUS_FATAL ( _CadLogger , " Writing detection results : " <<  rc );
				LOG4CPLUS_FATAL ( _CadLogger , " Trying to continue... " );
			}
		
			/* compute "unresampled" right centroid[2] values */
		
			mig_tag_resize ( &_CadData.results, _CadData.stack_s.z_res , _CadData.raw_s.z_res );
		}

		/* write final tags*/
		/* old version, in order to find unique series we add study and series uids
		snprintf ( _OutputName , MAX_PATH , "%s%s.tag" , _OutputPath ,
		_CadData.dicom_data.patient_id );*/
		snprintf ( _OutputName , MAX_PATH , "%s%s_%s_%s.tag" , _OutputPath ,
		_CadData.dicom_data.patient_id ,
		_CadData.dicom_data.study_uid ,
		_CadData.dicom_data.series_uid );

        /* write results to tag file */
        rc = mig_tag_write ( _OutputName , &( _CadData.results ) );
        if ( rc != MIG_OK )
        {
            LOG4CPLUS_FATAL ( _CadLogger , " Writing detection results : " <<  rc );
            LOG4CPLUS_FATAL ( _CadLogger , " Trying to continue... " );
        }

		
/*************************************************/

cleanup :

/*************************************************/
         
        /*************************************************/
        /* DONE PROCESSING */
        /*************************************************/
         
        /* set final processing status */
        CurrEntry->ErrorCode = rc;
      
        /* set results filename  */
        /* GF we write results even with 0 findings!
		   in these cases a tag file with 0 on the first row is written.

		if ( mig_lst_len ( &( _CadData.results ) ) == 0 )
            CurrEntry->ResultsFileName = strdup( " " );
        else 
		*/
        CurrEntry->ResultsFileName = strdup( _OutputName );
      
        /* when done processing put processed entry on DONE list */
        rc = mig_queue_add ( &_OutputQueue , CurrEntry );
        if ( rc != MIG_OK )
        {
            LOG4CPLUS_ERROR ( _CadLogger , " Could not add entry to done queue : " <<  rc );
        }
		

        LOG4CPLUS_INFO ( _CadLogger , " Done processing  : " << CurrEntry->InputPath );
        LOG4CPLUS_INFO ( _CadLogger , " Results are in  : " << CurrEntry->ResultsFileName );
      
        /* dicom loader cleanup */
        if ( _CadData.load_cleanup && _CadData.stack)
            _CadData.load_cleanup ( _CadData.stack );
      
        /* segmentation cleanup */
        if ( _CadData.seg_cleanup )
        {
            if ( _CadData.stack_l ) _CadData.seg_cleanup ( _CadData.stack_l );
            if ( _CadData.stack_r ) _CadData.seg_cleanup ( _CadData.stack_r );
        }
      
        /* detection cleanup */
		if ( _CadData.det_cleanup )
		{
			if ( & ( _CadData.det_r ) ) mig_lst_free_custom_static ( &( _CadData.det_r ) , _CadData.det_cleanup );
			if ( & ( _CadData.det_l ) ) mig_lst_free_custom_static ( &( _CadData.det_l ) , _CadData.det_cleanup );
		}

		/*AAAAAAA next line works only with regs with only one element*/
		/*in order to use old data with objs use _freeResults (but now is buggy)*/
		mig_lst_free_custom_static ( &( _CadData.results ) , _CadData.det_cleanup );		
		//_freeResults ( &( _CadData.results ) , _CadData.det_cleanup , _CadData.fpr1_cleanup );

        /* global timing */
        tot1 = getticks_sys();
        LOG4CPLUS_INFO ( _CadLogger , "CAD total timing : " << elapsed_sys( tot1 , tot0 ) << " secs." );

		#if defined(_DEBUG) && defined(_MIG_TRACK_LEAKS)
			_CrtDumpMemoryLeaks();
		#endif
	} /* FOREVER */
   
    return MIG_OK;
}

/***********************************************************/
/* PRIVATE FUNCTION */
/***********************************************************/

static int 
_init_dicom_loader ()
{
    mig_handle dll_handle = NULL;
    char *ErrorMsg = NULL;
    
    if ( _DicomLoadDLL == NULL )
    {
        LOG4CPLUS_FATAL ( _CadLogger , " Ini file does not contain a dicom loading dll..." );
        return MIG_ERROR_PARAM;
    }

    dll_handle = mig_dlopen ( _DicomLoadDLL );
    if ( dll_handle == NULL )
    {
        /* get last error */
        ErrorMsg = mig_dlerror ();
        if ( ErrorMsg != NULL )
        {
            LOG4CPLUS_FATAL ( _CadLogger , " Loading dicom loading dll : " << ErrorMsg );
        }
        else
        {
            LOG4CPLUS_FATAL ( _CadLogger , " Loading dicom loading dll... " );
        }

        return MIG_ERROR_IO;
    }

    /* we have got the handle to the dll copy function addresses by name */
    _InitDicomLoading    = (mig_init_f)    mig_dlsym ( dll_handle , MIG_INIT_F_NAME ); 
    _RunDicomLoading     = (mig_run_f)     mig_dlsym ( dll_handle , MIG_RUN_F_NAME ); 
    _CleanupDicomLoading = (mig_cleanup_f) mig_dlsym ( dll_handle , MIG_CLEANUP_F_NAME ); 
                
    if ( ( _InitDicomLoading    == NULL ) || ( _RunDicomLoading     == NULL ) || ( _CleanupDicomLoading == NULL ) )
    {
        ErrorMsg = mig_dlerror ();
        if ( ErrorMsg != NULL )
        {
            LOG4CPLUS_FATAL ( _CadLogger , " Accessing dll functions from dicom dll : " << ErrorMsg );
        }
        else
        {
            LOG4CPLUS_FATAL ( _CadLogger , " Accessing dll functions from dicom dll... " );
        }

        return MIG_ERROR_IO;
    }

    /* save dicom loading cleanup function address in global cad structure */
    _CadData.load_cleanup = _CleanupDicomLoading;

    return MIG_OK;
}

/***********************************************************/

static int 
_init_segementation ()
{
    mig_handle dll_handle = NULL;    
    char *ErrorMsg = NULL;

    /* segmentation dll */
    if ( _FlagPerformSegmentation == 1 )
    {
        if ( _SegmentationDLL == NULL )
        {
            LOG4CPLUS_FATAL ( _CadLogger , " Ini file does not contain a segmentation dll..." );
            return MIG_ERROR_PARAM;
        }

        /* get handle to DLL */
        dll_handle = mig_dlopen ( _SegmentationDLL );
        if ( dll_handle == NULL )
        {
            /* get last error */
            ErrorMsg = mig_dlerror ();			
            if ( ErrorMsg != NULL )
            {
                LOG4CPLUS_FATAL ( _CadLogger , " Loading segmentation dll : " << ErrorMsg );
            }
            else
            {
                LOG4CPLUS_FATAL ( _CadLogger , " Loading segmentation dll... " );
            }

            return MIG_ERROR_IO;
        }

        /* we have got the handle to the dll copy function addresses by name */
        _InitSegmentation = (mig_init_f) mig_dlsym ( dll_handle , MIG_INIT_F_NAME ); 
        _RunSegmentation = (mig_run_f) mig_dlsym ( dll_handle , MIG_RUN_F_NAME ); 
        _CleanupSegmentation = (mig_cleanup_f) mig_dlsym ( dll_handle , MIG_CLEANUP_F_NAME ); 
                
        if ( ( _InitSegmentation == NULL ) || ( _RunSegmentation == NULL ) || ( _CleanupSegmentation == NULL ) )
        {
            ErrorMsg = mig_dlerror ();
            if ( ErrorMsg != NULL )
            {
                LOG4CPLUS_FATAL ( _CadLogger , " Accessing dll functions from segmentation dll : " << ErrorMsg );
            }
            else
            {
                LOG4CPLUS_FATAL ( _CadLogger , " Accessing dll functions from segmentation dll... " );
            }

            return MIG_ERROR_IO;
        }
                
        /* save segmentation cleanup function address in global cad structure */
        _CadData.seg_cleanup = _CleanupSegmentation;
    }

    return MIG_OK;
}

/***********************************************************/

static int 
_init_detection ()
{
    mig_handle dll_handle = NULL;       
    char *ErrorMsg = NULL;

    /* detection */
    if ( _FlagPerformDetection == 1 )
    {
        if ( _DetectionDLL == NULL )
        {
            LOG4CPLUS_FATAL ( _CadLogger , " Ini file does not contain a detection dll..." );
            return MIG_ERROR_PARAM;
        }     
    
        /* get handle to DLL */
        dll_handle = mig_dlopen ( _DetectionDLL );
        if ( dll_handle == NULL )
        {
            /* get last error */
            ErrorMsg = mig_dlerror ();            
            if ( ErrorMsg != NULL )
            {
                LOG4CPLUS_FATAL ( _CadLogger , " Loading detection dll : " << ErrorMsg );
            }
            else
            {
                LOG4CPLUS_FATAL ( _CadLogger , " Loading detection dll... " );
            }
        
            return MIG_ERROR_IO;
        }

        /* we have got the handle to the dll copy function addresses by name */
        _InitDetection = (mig_init_f) mig_dlsym ( dll_handle , MIG_INIT_F_NAME ); 
        _RunDetection = (mig_run_f) mig_dlsym ( dll_handle , MIG_RUN_F_NAME ); 
        _CleanupDetection = (mig_cleanup_f) mig_dlsym ( dll_handle , MIG_CLEANUP_F_NAME ); 
                
        if ( ( _InitDetection == NULL ) || ( _RunDetection == NULL ) || ( _CleanupDetection == NULL ) )
        {
            ErrorMsg = mig_dlerror ();
            if ( ErrorMsg != NULL )
            {
                LOG4CPLUS_FATAL ( _CadLogger , "Accessing dll functions from detection dll : " << ErrorMsg );
            }
            else
            {
                LOG4CPLUS_FATAL ( _CadLogger , "Accessing dll functions from detection dll... " );
            }

            return MIG_ERROR_IO;
        }

        /* save detection cleanup function address in global cad structure */
        _CadData.det_cleanup = _CleanupDetection;
    }

    return MIG_OK;
}

/***********************************************************/

static int 
_init_fpr1 ()
{
    mig_handle dll_handle = NULL;
    char *ErrorMsg = NULL;

    /* detection */
    if ( _FlagPerformFpr1 == 1 )
    {
        if ( _Fpr1DLL == NULL )
        {
            LOG4CPLUS_FATAL ( _CadLogger , " Ini file does not contain a fpr1 dll..." );
            return MIG_ERROR_PARAM;
        }     
    
        /* get handle to DLL */
        dll_handle =  mig_dlopen ( _Fpr1DLL );
        if ( dll_handle == NULL )
        {
            /* get last error */
            ErrorMsg = mig_dlerror ();            
            if ( ErrorMsg != NULL )
            {
                LOG4CPLUS_FATAL ( _CadLogger , " Loading fpr1 dll : " << ErrorMsg );
            }
            else
            {
                LOG4CPLUS_FATAL ( _CadLogger , " Loading fpr1 dll... " );
            }
        
            return MIG_ERROR_IO;
        }

        /* we have got the handle to the dll copy function addresses by name */
        _InitFpr1 = (mig_init_f) mig_dlsym ( dll_handle , MIG_INIT_F_NAME ); 
        _RunFpr1 = (mig_run_f) mig_dlsym ( dll_handle , MIG_RUN_F_NAME ); 
        _CleanupFpr1 = (mig_cleanup_f) mig_dlsym ( dll_handle , MIG_CLEANUP_F_NAME ); 
                
        if ( ( _InitFpr1 == NULL ) || ( _RunFpr1 == NULL ) || ( _CleanupFpr1 == NULL ) )
        {
            ErrorMsg = mig_dlerror ();
            if ( ErrorMsg != NULL )
            {
                LOG4CPLUS_FATAL ( _CadLogger , "Accessing dll functions from fpr1 dll : " << ErrorMsg );
            }
            else
            {
                LOG4CPLUS_FATAL ( _CadLogger , "Accessing dll functions from fpr1 dll... " );
            }

            return MIG_ERROR_IO;
        }

		/* save fp1 cleanup function address in global cad structure */
        _CadData.fpr1_cleanup = _CleanupFpr1;
    }

    return MIG_OK;
}

/***********************************************************/

static int 
_init_fpr2 ()
{
    mig_handle dll_handle = NULL;
    char *ErrorMsg = NULL;

    /* detection */
    if ( _FlagPerformFpr2 == 1 )
    {
        if ( _Fpr2DLL == NULL )
        {
            LOG4CPLUS_FATAL ( _CadLogger , " Ini file does not contain a fpr2 dll..." );
            return MIG_ERROR_PARAM;
        }     
    
        /* get handle to DLL */
        dll_handle =  mig_dlopen ( _Fpr2DLL );
        if ( dll_handle == NULL )
        {
            /* get last error */
            ErrorMsg = mig_dlerror ();            
            if ( ErrorMsg != NULL )
            {
                LOG4CPLUS_FATAL ( _CadLogger , " Loading fpr2 dll : " << ErrorMsg );
            }
            else
            {
                LOG4CPLUS_FATAL ( _CadLogger , " Loading fpr2 dll... " );
            }
        
            return MIG_ERROR_IO;
        }

        /* we have got the handle to the dll copy function addresses by name */
        _InitFpr2 = (mig_init_f) mig_dlsym ( dll_handle , MIG_INIT_F_NAME ); 
        _RunFpr2 = (mig_run_f) mig_dlsym ( dll_handle , MIG_RUN_F_NAME ); 
        _CleanupFpr2 = (mig_cleanup_f) mig_dlsym ( dll_handle , MIG_CLEANUP_F_NAME ); 
                
        if ( ( _InitFpr2 == NULL ) || ( _RunFpr2 == NULL ) || ( _CleanupFpr2 == NULL ) )
        {
            ErrorMsg = mig_dlerror ();
            if ( ErrorMsg != NULL )
            {
                LOG4CPLUS_FATAL ( _CadLogger , "Accessing dll functions from fpr2 dll : " << ErrorMsg );
            }
            else
            {
                LOG4CPLUS_FATAL ( _CadLogger , "Accessing dll functions from fpr2 dll... " );
            }

            return MIG_ERROR_IO;
        }
    }

    return MIG_OK;
}

/***********************************************************/

static void
_cleanup_global_data ()
{
    _CadData.stack = NULL;
    memset ( &( _CadData.stack_s ) , 0x00 , sizeof( mig_size_t ) );
    
    _CadData.resampled = 0;
    memset ( &( _CadData.raw_s ) , 0x00 , sizeof( mig_size_t ) );
    
    memset ( &( _CadData.dicom_data ) , 0x00 , sizeof( mig_dcm_data_t ) );
    
    _CadData.stack_l = NULL;
    memset ( &( _CadData.stack_l_s ) , 0x00 , sizeof( mig_size_t ) );
    
    _CadData.stack_r = NULL;
    memset ( &( _CadData.stack_r_s ) , 0x00 , sizeof( mig_size_t ) );
        
    memset ( &( _CadData.bb ) , 0x00 , 2 * sizeof( mig_roi_t ) );

    mig_lst_empty ( &( _CadData.results ) );
}

/***********************************************************/

static void*
_db_reader ( void* arg )
{
    mig_db_t db_data;               /* database connection */
    _queue_entry_t *Entry;          /* new queue entry */
    char *NewEntryPath;
    int rc;
    int CurrQueueLen;
    int AvailableEntries;

    /* open database connection  */
    rc = mig_db_init ( &db_data , _DatabaseFile );
    if ( rc != MIG_OK )
    {
        LOG4CPLUS_ERROR ( _CadLogger , " _db_reader_f " << db_data.err );
        return NULL;
    }
        
    /* forever */
    while ( 1 )
    {
        /* current lenght of queue */
        CurrQueueLen = mig_queue_get_len ( &_InputQueue );
        
        /* calculate how many entries we can add */
        AvailableEntries = _MaxQueueLen - CurrQueueLen;
        if ( AvailableEntries == 0 )
        {
            sleep ( (unsigned int)_RetryReadInterval );
            continue;
        }
           
        /* while there is still space inside processing queue keep adding entries */
        while ( AvailableEntries > 0 )
        {
            rc = mig_db_unprocessed ( &db_data , &NewEntryPath );
            if ( rc != MIG_OK )
            {
                LOG4CPLUS_ERROR ( _CadLogger , " _db_reader_f " << db_data.err );
                break;
            }
              
            /* no more unprocessed entries in database */
            if ( NewEntryPath == NULL )
            {
                sleep ( (unsigned int)_RetryReadInterval );
                break;
            }
              
            Entry = (_queue_entry_t*) malloc ( sizeof( _queue_entry_t ) );
            if ( Entry == NULL )
                return NULL;
              
            Entry->InputPath = NewEntryPath;
            mig_queue_add ( &_InputQueue , Entry );
              
            LOG4CPLUS_DEBUG ( _CadLogger , " List Reader Retreived  : " << Entry->InputPath );
              

            NewEntryPath = NULL;
            AvailableEntries --;
        }
    }
        
    mig_db_close ( &db_data );
    return NULL;
}

/***********************************************************/
static void*
_db_writer ( void* arg )
{
    mig_db_t db_data;
    _queue_entry_t *Entry;
	int rc;
   
    char CurrentDate[DATE_LEN];
    char CurrentTime[TIME_LEN];
   
    /* connect to database */
    rc = mig_db_init ( &db_data , _DatabaseFile );
    if ( rc != MIG_OK )
    {
        LOG4CPLUS_ERROR( _CadLogger , " _db_writer_f " << db_data.err );
        return NULL;
    }
   
    /* forever */
    while ( 1 )
    {
        Entry = (_queue_entry_t*) mig_queue_get ( &_OutputQueue );
        if ( Entry == NULL )
        {
            LOG4CPLUS_ERROR ( _CadLogger , "_db_writer -> mig_queue_get retreaved a NULL. Trying to continue..." );
            continue;
        }

        LOG4CPLUS_DEBUG ( _CadLogger , " List Writer Retreived  : " << Entry->InputPath );
      
        /* processing was successful */
        if ( Entry->ErrorCode == MIG_OK )
        {
            /* set process status */
            rc = mig_db_set_status ( &db_data , Entry->InputPath , MIG_PROCESS , MIG_PROC_STATUS_DONE );
            if ( rc != MIG_OK )
            {
                LOG4CPLUS_ERROR ( _CadLogger , " List Writer Error  : " << rc << "Db message : " << db_data.err );                
            }
        }
        /* there was an error during processing */
        else
        {
            /* set process status */
            rc = mig_db_set_status ( &db_data , Entry->InputPath , MIG_PROCESS , MIG_PROC_STATUS_ERROR );
            if ( rc != MIG_OK )
            {
                LOG4CPLUS_ERROR ( _CadLogger , " List Writer Error  : " << rc << "Db message : " << db_data.err );                
            }        
        }
      
        /* get current date and time */        
        rc = mig_ut_date_time ( (char*)&CurrentDate , (char*) &CurrentTime );        
      
        /* set process date */
        rc = mig_db_set_date ( &db_data ,  Entry->InputPath , MIG_PROCESS , 
                (char*)&CurrentDate , (char*)&CurrentTime );
        if ( rc != MIG_OK )
        {
            LOG4CPLUS_ERROR ( _CadLogger , " List Writer Error  : " << rc << "Db message : " << db_data.err );                
        }
      
        /* set destination directory */
        rc = mig_db_set_dest ( &db_data , Entry->InputPath , Entry->ResultsFileName );
        if ( rc != MIG_OK )
        {
            LOG4CPLUS_ERROR ( _CadLogger , " List Writer Error  : " << rc << "Db message : " << db_data.err );                
        }
      
        /* cleanup */
        free ( Entry->InputPath );
        free ( Entry->ResultsFileName );
        free ( Entry );
    }
   
    mig_db_close ( &db_data );
    return NULL;
}

/***********************************************************/
static void
_freeResults  ( mig_lst_t *list , mig_lst_free_f free_data , mig_lst_free_f free_node )
{
	mig_im_region_t *curr = NULL;
	assert ( free_data );
	//AAAA for now, we assume free_node is NULL if fpr1 is not performed
	if ( free_node == NULL ) {
		//in this case results does not contains 3d structures
		mig_lst_free_custom_static ( list , free_data) ;
	}
	else {
		
		//we need to clear all objs
		while ( curr = ( mig_im_region_t * ) mig_lst_get_head ( list ) ) {
			if (curr->objs.num > 0){
				mig_lst_free_custom_static_data_and_node ( &( curr->objs ) , free_data , free_node );
			}
			free_data ( curr );
		}
	}
}

