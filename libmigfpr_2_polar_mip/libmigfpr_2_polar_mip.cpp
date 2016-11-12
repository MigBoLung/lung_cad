#include "libmigfpr_2_polar_mip.h"

#define UI8UI16RATIO  0.0038910506f  /* ratio between maxuint8(255) and maxuint16(65535) */

/*
******************************************************************************
*                               PRIVATE DATA
******************************************************************************
*/

typedef struct _fpr2_thread_data
{
    /* thread id */
    int id;

    /* Input data -> results of fpr1 */
    mig_lst_t *Input;

} fpr2_thread_data;

typedef struct _fpr2_params_t
{    
    /* svm model */
    mig_svm_t model;

    /* svm feature normalization parameters */
    mig_svm_scale_t scales;

    /* crop side length after resizing */
    int resized_len;

	/* how much of voi is used to get mip image (float [0,1]) */
	float mip_ratio;

	/**********************************************************/
	/* polar transform params
	/**********************************************************/
	
	/* starting radius */
	float polar_min_radius;
	/* ending radius */
	float polar_max_radius;
	/* number of radii */
	int polar_nr;
	/* number of angles */
	int polar_nphy;
	
	/*********************************************************/

	/* minimum positive labels */
	int min_pos_labels;

	/* results directory where to dump intermediate results */
    char *dir_results;
	
    /* threading */
    pthread_t           thread[2];      /* frp1 threads for left and right lung */ 
    fpr2_thread_data    thread_data[2]; /* fpr1 thread parameters for left and right lungs */

} fpr2_params_t;

typedef struct _float_point3d_t
{
	float x;
	float y;
	float z;
} float_point3d_t;

typedef struct int_point3d_t
{
	int x;
	int y;
	int z;
} int_point3d_t;

/*
******************************************************************************
*                               PRIVATE DATA
******************************************************************************
*/

/* logger for the whole cad */
static Logger _log =
	Logger::getInstance ( CAD_LOGGER_NAME );

/* pointer to global data structure */
static mig_cad_data_t *_CadData = NULL;

/* fpr1 parameters */
static fpr2_params_t _Fpr2Params;

/*
******************************************************************************
*                               PRIVATE PROTOTYPE DECLARATIONS
******************************************************************************
*/

static void*
_fpr2_thread_routine ( void *arg );

static int 
_classify_obj_3d ( mig_im_region_t *obj3d , int *label );


/*
******************************************************************************
*                               GLOBAL FUNCTIONS
******************************************************************************
*/

/* DLL entry for Windows */
#if defined(WIN32)

BOOL APIENTRY
DllMain ( HANDLE hModule , DWORD dwReason , LPVOID lpReserved )
{
	return TRUE;
}

#endif /* Win32 DLL */

/*
******************************************************************************
*                               DLL INITIALIZATION
******************************************************************************
*/

int
mig_init ( mig_dic_t *d ,
           mig_cad_data_t *data )
{
    char *model_file_name = NULL;
    char *scale_file_name = NULL;
    int rc;

    /* setup logging system */
    char *_logger_ini_f_name =
        mig_ut_ini_getstring ( d , PARAM_LOG_INI , DEFAULT_PARAM_LOG_INI );
    if ( _logger_ini_f_name != NULL )
    {
        try
        {
            PropertyConfigurator::doConfigure ( _logger_ini_f_name );
        }
        catch ( ... )
        {
            fprintf ( stderr , "\nError loading log parameters from : %s..." , _logger_ini_f_name );
        }
    }

    /* here we've got a logging system so log what we are doing */
    LOG4CPLUS_DEBUG ( _log , " libmigfpr_2 -> mig_init starting..." );

    /* global cad data */
    _CadData = data;

	/* load parameters from ini file */
    
	/* TODO: add polarmip parameters */
	
    /* get model file name from ini file */
    model_file_name = mig_ut_ini_getstring ( d , PARAM_FPR2_SVM_MODEL_FILE , NULL );
    if ( model_file_name == NULL )
    {
        LOG4CPLUS_FATAL ( _log , " Ini file does not contain a model file..." );
        return MIG_ERROR_PARAM;
    }

	/* get scale file name from ini file */
    scale_file_name = mig_ut_ini_getstring ( d , PARAM_FPR2_SVM_NORM_FILE , NULL );
    if ( scale_file_name == NULL )
    {
        LOG4CPLUS_FATAL ( _log , " Ini file does not contain a scale file..." );
        return MIG_ERROR_PARAM;
    }

    /* load scale file into memory */
    rc = mig_svm_scale_params_load ( scale_file_name , &( _Fpr2Params.scales ) );
    if ( rc != MIG_OK )
    {
        mig_svm_scale_params_free ( &( _Fpr2Params.scales ) );
        LOG4CPLUS_FATAL ( _log , " Could not load scales file : " << scale_file_name );
        return rc;
    }

    /* load model file into memory */
    rc = mig_svm_model_load ( model_file_name , &( _Fpr2Params.model ) );
    if ( rc != MIG_OK )
    {
        mig_svm_model_free ( &( _Fpr2Params.model ) );
        LOG4CPLUS_FATAL ( _log , " Could not load model file : " << model_file_name );
        return rc;
    }

	/* results */
	_Fpr2Params.dir_results = 
		mig_ut_ini_getstring ( d , PARAM_CAD_DIR_OUT , DEFAULT_PARAM_CAD_DIR_OUT );

    /* log parameters */
    if ( _log.getLogLevel() <= INFO_LOG_LEVEL )
    {
        std::stringstream os;
		os << "\nFPR2 parameters : ";
        os << "\nModel file  : " << model_file_name;
        os << "\nScales file : " << scale_file_name;
        LOG4CPLUS_INFO ( _log , os.str() );
    }

    LOG4CPLUS_DEBUG ( _log , " libmigfpr_2 -> mig_init done..." );
    return MIG_OK;
}

/*
******************************************************************************
*                               DLL MAIN FUNCTION
******************************************************************************
*/

int
mig_run ()
{
    int rc, rc1, rc2;

	char path[MAX_PATH];

    LOG4CPLUS_DEBUG ( _log , " libmigfpr_2 -> mig_run starting..." );

	/******************************************************************************/
    /* try loading fpr2 point from disc */
  
    /* right lung */
    snprintf ( path , MAX_PATH , "%s%c%s_%s_%s_fpr2_r.txt" ,
                        _Fpr2Params.dir_results , MIG_PATH_SEPARATOR ,
                        _CadData->dicom_data.patient_id ,
                        _CadData->dicom_data.study_uid ,
                        _CadData->dicom_data.series_uid );

    LOG4CPLUS_INFO( _log , " Trying to load fpr2 results for right lung from : " << path );
    rc1 = mig_tag_read ( path , &( _CadData->det_r ) );
    if ( rc1 == MIG_OK )
    {
        _CadData->det_r._free = &free;
        LOG4CPLUS_INFO ( _log , " Loaded right lung fpr2 data from disk...");
    }
    
    /* left lung */
    snprintf ( path , MAX_PATH , "%s%c%s_%s_%s_fpr2_l.txt" ,
               _Fpr2Params.dir_results , MIG_PATH_SEPARATOR ,
               _CadData->dicom_data.patient_id ,
               _CadData->dicom_data.study_uid ,
               _CadData->dicom_data.series_uid );

    LOG4CPLUS_INFO( _log , " Trying to load fpr2 results for left lung from : " << path );
    rc2 = mig_tag_read ( path , &( _CadData->det_l ) );
    if ( rc2 == MIG_OK )
    {
        _CadData->det_l._free = &free;
        LOG4CPLUS_INFO ( _log , " Loaded left lung fpr2 data from disk...");
    }
    
    if ( ( rc1 == MIG_OK ) && ( rc2 == MIG_OK ) )
    {
		_CadData ->det_cleanup  = &free;
		_CadData ->fpr1_cleanup  = &free;
        LOG4CPLUS_INFO ( _log , " Using fpr2 data from disk...");
        return MIG_OK;
    }

    LOG4CPLUS_INFO( _log , " Could not load fpr2 data from disk. Performing full fpr2..." );

    /* setup input to detection threads */
    _Fpr2Params.thread_data[0].id      = 0;
    _Fpr2Params.thread_data[0].Input   = &( _CadData->det_r );

	_Fpr2Params.thread_data[1].id      = 1;
    _Fpr2Params.thread_data[1].Input   = &( _CadData->det_l );

    /* spawn detection thread 0 */
    rc = pthread_create ( &( _Fpr2Params.thread[0] ) , NULL ,
            &( _fpr2_thread_routine ),  &( _Fpr2Params.thread_data[0] ) );

	if ( rc != 0 )
    {
        LOG4CPLUS_FATAL ( _log , " libmigfpr_2 -> mig_run pthread_create returned : " << rc );
        return MIG_ERROR_THREAD;
    }
   
    /* spawn detection thread 1 */
    rc = pthread_create ( &( _Fpr2Params.thread[1] ) , NULL ,
            &( _fpr2_thread_routine ),  &( _Fpr2Params.thread_data[1] ) );
    if ( rc != 0 )
    {
        LOG4CPLUS_FATAL ( _log , " libmigfpr_2 -> mig_run pthread_create returned : " << rc );
        return MIG_ERROR_THREAD;
    }

    /* wait for left thread to finish */
    pthread_join ( _Fpr2Params.thread[0] , (void**) &rc );
   
    /* wait for right thread to finish */
    pthread_join ( _Fpr2Params.thread[1] , (void**) &rc ); 
   
    LOG4CPLUS_DEBUG ( _log , " libmigfpr_2 -> mig_run end..." );

	/******************************************************************************/
    /* save fpr2 lists to disk */
   
    /* right lung */
    snprintf ( path , MAX_PATH , "%s%c%s_%s_%s_fpr2_r.txt" ,
                        _Fpr2Params.dir_results , MIG_PATH_SEPARATOR ,
                        _CadData->dicom_data.patient_id ,
                        _CadData->dicom_data.study_uid ,
                        _CadData->dicom_data.series_uid );

    LOG4CPLUS_INFO( _log , " Trying to save fpr2 results for right lung to : " << path );
    rc = mig_tag_write ( path , &( _CadData->det_r ) );
    if ( rc != MIG_OK )
    {
        LOG4CPLUS_WARN( _log , " Could not save fpr2 results for right lung to disk..." );
    }
    else
    {
        LOG4CPLUS_INFO( _log , " Saved fpr2 results for right lung to disk..." );
    }

    /* left lung */
    snprintf ( path , MAX_PATH , "%s%c%s_%s_%s_fpr2_l.txt" ,
                        _Fpr2Params.dir_results , MIG_PATH_SEPARATOR ,
                        _CadData->dicom_data.patient_id ,
                        _CadData->dicom_data.study_uid ,
                        _CadData->dicom_data.series_uid );

    LOG4CPLUS_INFO( _log , " Trying to save fpr2 results for left lung to : " << path );
    rc = mig_tag_write ( path , &( _CadData->det_l ) );
    if ( rc != MIG_OK )
    {
        LOG4CPLUS_WARN( _log , " Could not save fpr2 results for left lung to disk..." );
    }
    else
    {
        LOG4CPLUS_INFO( _log , " Saved fpr2 results for left lung to disk..." );
    }

    return MIG_OK;
}

/*
******************************************************************************
*                               DLL CLEANUP
******************************************************************************
*/

void
mig_cleanup ( void* data )
{
    if ( data )
        free ( data );
}

/*
******************************************************************************
*                               DLL INFORMATION
******************************************************************************
*/

void
mig_info ( mig_dll_info_t* info )
{

}

/*
******************************************************************************
*                               PRIVATE PROTOTYPE DEFINITIONS
******************************************************************************
*/

static void*
_fpr2_thread_routine ( void *arg )
{
    mig_lst_t survived_objs = { NULL , NULL , 0 , free };      /* list of survived 3d objects */
    mig_im_region_t *curr;                                      /* current 3d region */
    fpr2_thread_data *data = (fpr2_thread_data*) arg;           /* input parameters */
    int label = 0;
    int rc;
    
    LOG4CPLUS_DEBUG ( _log , " _fpr2_thread_routine : " << data->id );

    if ( data->Input == NULL )
    {
        LOG4CPLUS_DEBUG ( _log , " _fpr2_thread_routine : " << data->id << " , Input data was empty..." );
        return ( (void*)MIG_OK );
    }

    /* classify each 3d object */
    while ( curr = (mig_im_region_t*) mig_lst_get_head ( data->Input ) )
    {
		/* classify 3d object */
        rc = _classify_obj_3d ( curr , &label );
        if ( rc != MIG_OK )
        {
            LOG4CPLUS_FATAL ( _log , "Aborting fpr2 thread. Error : " <<  rc << " in : " << data->id );
            pthread_exit ( (void*)rc );    
        }

        /* if label is 1 add to survived objects list */
        if ( label == 1 )
            mig_lst_put_tail ( &survived_objs , curr );
        /* if label is 0 free all 2d objects belonging to current 3d object */
        else
        {
			mig_lst_free_custom_static_data_and_node ( &( curr->objs ) , _CadData->det_cleanup , _CadData->fpr1_cleanup);
			_CadData->det_cleanup ( curr );
        }
            
    } /* while */
    
    /* copy survived list into input list */
    mig_lst_cat ( &survived_objs , data->Input );

    return ( (void*)MIG_OK );
}

/*******************************************************************************/

static int 
_classify_obj_3d ( mig_im_region_t *obj3d , int *label )
{
    mig_lst_iter it;

    int rc;
    int pos = 0 , neg = 0;
    int curr_label;
	int idir = 0;
	mig_svm_example_t x;

	int diam_full , r_full, diam_full_z, r_full_z;
    int diam_valid , r_valid, diam_valid_z, r_valid_z;
    float *crop = NULL , *rot = NULL , *cut = NULL , *resized = NULL, *whitened = NULL;
    unsigned short *slice;
    int pos = 0 , neg = 0 , tot;
	
	rc = MIG_OK;


	/* centroid & radius */
	/* crop 3d */
	 /* crop cutting radii */
    r_full  = (int) ( 2.0f * MIG_SQRT2 *( obj2d->radius * 2.0f + 0.5f ) );
    r_valid = (int) ( 2.0f *( obj2d->radius * 2.0f + 0.5f ) );

    /* crop cutting diameters */
    diam_full = 2 * r_full + 1;
    diam_valid = 2 * r_valid + 1;

    /* memory for crop */
    crop = (float*) malloc ( MIG_POW2( diam_full ) * sizeof(float) );
    if ( crop == NULL )
        return MIG_ERROR_MEMORY;

	
	
	for (idir = 0; idir != 3; idir++){
		/* mip */
		mig_im_proj_mip_axes_vol (  float *src , float *dst , int w , int h, int d, int dir_idx, float ratio)
		/* resize */
		mig_im_geom_resize ( cut , diam_valid , diam_valid , resized , _Fpr2Params.resized_len , _Fpr2Params.resized_len , BILINEAR );
		/* polar */
		mig_im_polar (  float *src , float *dst , int w , int h, int nr , int nphy , float min_r, float max_r)
		/* moments and histo*/
		
		/* whitening */
		mig_im_scale_whitening ( resized, whitened , NSIGMA_WHITE,
            _Fpr2Params.scales.len , _Fpr2Params.scales.mean ,  _Fpr2Params.scales.std );
		

		/* load data into svm suitable format */
        /* x.len  = MIG_POW2( diam_valid ); */
		x.len  = MIG_POW2( _Fpr2Params.resized_len );
	    x.feat = whitened;
		
		/* label prediction */
		if ( mig_svm_predict ( &( _Fpr2Params.model ) , &x ) != MIG_OK )
            goto error;
		/*increment pos or neg */
		

        if ( x.label == 1 )
            pos ++ ;
        else
            neg ++;

		/* zero buffers */
        memset ( rot     , 0x00 ,  MIG_POW2( diam_full  ) * sizeof(float) );
        memset ( cut     , 0x00 ,  MIG_POW2( diam_valid ) * sizeof(float) );
		memset ( resized , 0x00 ,  MIG_POW2( _Fpr2Params.resized_len ) * sizeof(float) );
		memset ( whitened , 0x00 ,  MIG_POW2( _Fpr2Params.resized_len ) * sizeof(float) );
	}
	if (pos >= _Fpr2Params.min_pos_labels)
		*label = 1;
	else
		*label = 0;   




    free ( crop );
    free ( rot );
    free ( cut );
    free ( resized );
	free ( whitened );

    return MIG_OK;

error :

	if ( crop )
        free ( crop );

    if ( rot )
        free ( rot );

    if ( cut )
        free ( cut );

    if ( resized )
        free ( resized );

	if ( whitened )
		free ( whitened );
    
    return MIG_ERROR_MEMORY;     
}

/*******************************************************************************/

static float
_obj3d_max_radius ( mig_im_region_t *reg )
{
	mig_lst_t *objs = &( reg->objs );   /* list of 2d objects belonging to current 3d object */
    mig_im_region_t *curr;              /* current 2d view */
    mig_lst_iter it; 
	float radius = 0;
	float curr_radius_mm = 0;
	/* build up corrdinate arrays */
    mig_lst_iter_get ( &it , objs );
    while (  curr = (mig_im_region_t*) mig_lst_iter_next ( &it ) )
    {
		curr_radius_mm = curr->radius * _CadData->stack_s.h_res;
        if (curr_radius_mm  > radius) radius = curr_radius_mm;
    }

	return radius;
}

static int
_obj3d_centroid_int ( mig_im_region_t *reg, int *centroid )
{
	mig_lst_t *objs = &( reg->objs );   /* list of 2d objects belonging to current 3d object */
    mig_im_region_t *curr;              /* current 2d view */
    mig_lst_iter it;
	int count = 0;
    mig_lst_iter_get ( &it , objs );
	float centroid_f[3];
	
	centroid_f[0] = 0;
	centroid_f[1] = 0;
	centroid_f[2] = 0;

    while (  curr = (mig_im_region_t*) mig_lst_iter_next ( &it ) )
    {
		centroid_f[0] += curr->centroid[0];
		centroid_f[1] += curr->centroid[1];
		centroid_f[2] += curr->centroid[2];
		count++;
    }
	centroid[0] = (int) (centroid_f[0] / count + 0.5);
	centroid[1] = (int) (centroid_f[1] / count + 0.5);
	centroid[2] = (int) (centroid_f[2] / count + 0.5);

	return MIG_OK;
}

//PORTARE IN mig_im_bb?
static int
_dims_from_diam_mm ( float diam, float* res, int* dims)
{
	dims[0] = (int)(diam / res[0] + 0.5); 
	dims[1] = (int)(diam / res[1] + 0.5); 
	dims[2] = (int)(diam / res[2] + 0.5); 

	return MIG_OK;
}
