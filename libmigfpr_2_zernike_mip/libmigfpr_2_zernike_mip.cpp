#include "libmigfpr_2_zernike_mip.h"
#include "libmig_feat_zernike_mip.h"
#include "libmigwhitening.h"

#define NSIGMA_WHITE 1
#define NDIR 3
/*
******************************************************************************
*                               PRIVATE DATA
******************************************************************************
*/

typedef struct _fpr2_thread_data
{
	/* thread id */
	int id;

	/* Input data -> segmented */
	Mig16u      *Src;
	mig_size_t  *SrcSize;
	mig_roi_t   *SrcBoundingBox;

	/* Input data -> results of fpr1 */
	mig_lst_t *Input;

	/* Buffers */
	float *whitened;
	feat_t *featstruct;


} fpr2_thread_data;




typedef struct _fpr2_params_t
{    
	/* svm model */
	mig_svm_t model;

	/* svm feature normalization parameters */
	/*mig_svm_scale_t scales;*/
	EigenWhitener whitener;

	/*********************************************************/

	/* feature extraction data */

	fpr2_params_feat_t featparams;

	/* minimum positive labels */
	int min_pos_labels;

	/* results directory where to dump intermediate results */
	char *dir_results;

	/* threading */
	pthread_t           thread[2];      /* frp1 threads for left and right lung */ 
	fpr2_thread_data    thread_data[2]; /* fpr1 thread parameters for left and right lungs */

} fpr2_params_t;


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

/* fpr2 parameters */
static fpr2_params_t _Fpr2Params;

/*
******************************************************************************
*                               PRIVATE PROTOTYPE DECLARATIONS
******************************************************************************
*/

static void*
_fpr2_thread_routine ( void *arg );

static int 
_classify_obj_3d ( mig_im_region_t *obj3d , int *label  , fpr2_thread_data *data);

static int
_thread_data_alloc( fpr2_thread_data *thread_data );

static void
_thread_data_free( fpr2_thread_data *thread_data );
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

	/* TODO: update INI with new parameters */
	_Fpr2Params.min_pos_labels = 
		mig_ut_ini_getint ( d, PARAM_FPR2_MIN_POS_LABELS, DEFAULT_PARAM_FPR2_MIN_POS_LABELS );

	_Fpr2Params.featparams.mip_ratio = 
		mig_ut_ini_getdouble ( d, PARAM_FPR2_MIP_RATIO, DEFAULT_PARAM_FPR2_MIP_RATIO );

	_Fpr2Params.featparams.resized_len =
		mig_ut_ini_getint ( d, PARAM_FPR2_RESIZED_LEN , DEFAULT_PARAM_FPR2_RESIZED_LEN ) ;

	_Fpr2Params.featparams.crop_sizes =
		mig_ut_ini_getintarray ( d , PARAM_FPR2_CROP_SIZES , &( _Fpr2Params.featparams.crop_sizes_len ) );
	if ( _Fpr2Params.featparams.crop_sizes == NULL )
		return MIG_ERROR_IO;

	_Fpr2Params.featparams.mom_orders = 
		mig_ut_ini_getintarray ( d , PARAM_FPR2_MOM_ORDERS , &( _Fpr2Params.featparams.mom_orders_len ) );
	if ( _Fpr2Params.featparams.mom_orders == NULL )
		return MIG_ERROR_IO;

	/* Moments data structure  creation */
	_Fpr2Params.featparams.mom_masks = mig_im_mom_get ( _Fpr2Params.featparams.resized_len ,
		_Fpr2Params.featparams.mom_orders , _Fpr2Params.featparams.mom_orders_len );

	if ( _Fpr2Params.featparams.mom_masks == NULL )
	{
		LOG4CPLUS_FATAL ( _log , " Error filling moment masks..." );
		return MIG_ERROR_INTERNAL;
	}


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
	/* DEACTIVATED, WE ARE NOW USING REAL WHITENING
	rc = mig_svm_scale_params_load ( scale_file_name , &( _Fpr2Params.scales ) );
	if ( rc != MIG_OK )
	{
		mig_svm_scale_params_free ( &( _Fpr2Params.scales ) );
		LOG4CPLUS_FATAL ( _log , " Could not load scales file : " << scale_file_name );
		return rc;
	}
	*/
	
	rc = mig_whitening_load ( &(_Fpr2Params.whitener), scale_file_name);
	if ( rc != MIG_OK )
	{
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
	_Fpr2Params.thread_data[0].Src            = _CadData->stack_r;
	_Fpr2Params.thread_data[0].SrcSize        = &( _CadData->stack_r_s );
	_Fpr2Params.thread_data[0].SrcBoundingBox = &( _CadData->bb[0] );
	rc = _thread_data_alloc ( &_Fpr2Params.thread_data[0] );

	if ( rc != 0 )
	{
		LOG4CPLUS_FATAL ( _log , " libmigfpr_2 -> mig_run thread_data_alloc : " << rc );
		return MIG_ERROR_MEMORY;
	}

	_Fpr2Params.thread_data[1].id      = 1;
	_Fpr2Params.thread_data[1].Input   = &( _CadData->det_l );
	_Fpr2Params.thread_data[1].Src            = _CadData->stack_l;
	_Fpr2Params.thread_data[1].SrcSize        = &( _CadData->stack_l_s );
	_Fpr2Params.thread_data[1].SrcBoundingBox = &( _CadData->bb[1] );
	rc = _thread_data_alloc ( &_Fpr2Params.thread_data[1] );

	if ( rc != 0 )
	{
		LOG4CPLUS_FATAL ( _log , " libmigfpr_2 -> mig_run thread_data_alloc : " << rc );
		return MIG_ERROR_MEMORY;
	}

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
	

	/* cleanup buffers */
	_thread_data_free ( &(_Fpr2Params.thread_data[0]) );
	_thread_data_free ( &(_Fpr2Params.thread_data[1]) );

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
	int oldlabel = 0;
	int rc;
	
	/*******************************/
	/* MULTIRES PARAMS             */
	/*******************************/
	int multires_pos = 0;
	float multires_radius = 0.0f;

	int radii[] = { 3, 6, 12, 24};
	int radii_len = 4;
	int r_idx;

	/*******************************/

	LOG4CPLUS_DEBUG ( _log , " _fpr2_thread_routine : " << data->id );

	if ( data->Input == NULL )
	{
		LOG4CPLUS_DEBUG ( _log , " _fpr2_thread_routine : " << data->id << " , Input data was empty..." );
		return ( (void*)MIG_OK );
	}

	/* classify each 3d object */
	while ( curr = (mig_im_region_t*) mig_lst_get_head ( data->Input ) )
	{
		/* if radius is not 0 use estimated radius, else do multires */

		if (curr->radius > 0.0001)
		{
			/* classify 3d object */
				rc = _classify_obj_3d ( curr , &label, data);
				if ( rc != MIG_OK )
				{
					LOG4CPLUS_FATAL ( _log , "Aborting fpr2 thread. Error : " <<  rc << " in : " << data->id );
					pthread_exit ( (void*)rc );    
				}
		}
		else /* multires */
		{

			multires_pos = 0;
			multires_radius = 0.0f;
			oldlabel = 0;
			for ( r_idx = 0 ; r_idx < radii_len ; ++r_idx )
			{
				label = 0;
				curr->radius = radii[r_idx];
				/* classify 3d object */
				rc = _classify_obj_3d ( curr , &label, data);
				if ( rc != MIG_OK )
				{
					LOG4CPLUS_FATAL ( _log , "Aborting fpr2 thread. Error : " <<  rc << " in : " << data->id );
					pthread_exit ( (void*)rc );    
				}

				if ( label == 1 )
				{
					
					++multires_pos;
					if ( multires_radius  < curr->radius )
						multires_radius = curr->radius;
				}
				/* here we require at least two nearby positive labels */
				/*if ( oldlabel && label )
				{
					multires_pos = 1;
					multires_radius = curr->radius;
					break;
				}
				*/

				oldlabel = label;
			}

			/* assign final label and radius, here we can do better things */
			if (multires_pos >= 1 )
			{
				label = 1;
				curr->radius = multires_radius;
			}
			else
			{
				label = 0;
			}
		
		}

		/* if label is 1 add to survived objects list */
		if ( label == 1 )
		{
			LOG4CPLUS_DEBUG ( _log , " _fpr2_thread_routine : reg classified as positive" );

			mig_lst_put_tail ( &survived_objs , curr );
		}
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
_classify_obj_3d ( mig_im_region_t *obj3d , int *label , fpr2_thread_data *data)
{
	
	mig_svm_example_t x;

	int idir = 0;

	float *whitened = data->whitened;
	int pos = 0 , neg = 0;

	feat_t *featstruct = data->featstruct;
	
	mig_im_region_t _temp_obj3d;
	/* get features */
	/* from original */
	/*
	if ( extract_features (obj3d, _CadData->stack,
		_CadData->stack_s.w , _CadData->stack_s.h , _CadData->stack_s.slices ,
		&(_Fpr2Params.featparams), featstruct ))
		return MIG_ERROR_INTERNAL;
	*/
	/* from segmented :TODO: NOT OK: we must change extract_feature in order to handle bb*/
	
	_temp_obj3d.centroid[0] = obj3d->centroid[0] - data->SrcBoundingBox->x0;
	_temp_obj3d.centroid[1] = obj3d->centroid[1] - data->SrcBoundingBox->y0;
	_temp_obj3d.centroid[2] = obj3d->centroid[2] - data->SrcBoundingBox->z0;
	_temp_obj3d.radius = obj3d->radius;
	_temp_obj3d.size = obj3d->size;

	if ( extract_features (&_temp_obj3d, data->Src,
		data->SrcSize->w , data->SrcSize->h , data->SrcSize->slices ,
		&(_Fpr2Params.featparams), featstruct ))
		return MIG_ERROR_INTERNAL;

	for (idir = 0; idir != NDIR ; idir++){

		/* TODO: here create REAL whitening  (it gives way better results) */
		/*
		mig_im_scale_whitening ( featstruct->feats[idir], whitened , NSIGMA_WHITE,
			_Fpr2Params.scales.len , _Fpr2Params.scales.mean ,  _Fpr2Params.scales.std );
		*/
		mig_whitening_apply ( &(_Fpr2Params.whitener), featstruct->feats[idir] , whitened );

		/* load data into svm suitable format */
		/* x.len  = MIG_POW2( diam_valid ); */
		x.len  = _Fpr2Params.featparams.mom_orders_len;
		x.feat = whitened;

		/* label prediction */
		if ( mig_svm_predict ( &( _Fpr2Params.model ) , &x ) != MIG_OK )
			return MIG_ERROR_INTERNAL;  

		/*increment pos or neg */
		if ( x.label == 1 )
			pos ++ ;
		else
			neg ++;

	}
	if (pos >= _Fpr2Params.min_pos_labels)
		*label = 1;
	else
		*label = 0;   

	return MIG_OK;   
}

/*******************************************************************************/


static int
_thread_data_alloc( fpr2_thread_data *thread_data )
{

	/* memory for whitened */
	thread_data->whitened = (float*) calloc (
		MIG_POW2( _Fpr2Params.featparams.mom_orders_len ), sizeof(float) );
	if ( thread_data->whitened == NULL )
		return MIG_ERROR_MEMORY;


	thread_data->featstruct = feat_t_alloc ( NDIR, _Fpr2Params.featparams.mom_orders_len );
	if ( thread_data->featstruct == NULL )
	{
		free ( thread_data->whitened );
		return MIG_ERROR_MEMORY;
	}
	return MIG_OK;
}

static void 
_thread_data_free ( fpr2_thread_data *thread_data )
{
	if ( thread_data->whitened )
		free ( thread_data->whitened );
	if ( thread_data->featstruct )
		feat_t_free ( thread_data->featstruct );	
}
