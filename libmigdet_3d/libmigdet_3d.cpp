#include <pthread.h>
#include "libmigdet_3d.h"

/*FAST RADIAL 3D requires a lot of memory, this switch make detection execution serial,
 * so that it can work without problems in a 32 bit environment
 * here we assume linux builds are 64bits
 */
#if (defined(_WIN64) || !defined(_WIN32))
	#define FR_SERIAL 0
#else
	#define FR_SERIAL 1
#endif

/*
******************************************************************************
*                               PRIVATE DATA
******************************************************************************
*/

typedef struct _det_thread_data
{
	/* thread id */
	int id;

	/* Input data -> segmented */
	Mig16u      *Src;
	mig_size_t  *SrcSize;
	mig_roi_t   *SrcBoundingBox;

	/* Input data -> Original */
	Mig16u      *Original;
	mig_size_t  *OriginalSize;

	/* Output ( suspect ROIs ) */
	mig_lst_t *Results;

} det_thread_data;

typedef struct _det_params_t
{
	/* Fast Radial */
	float           *fr_radii;      /* fast radial radii */
	int             fr_num_radii;   /* fast radial number of radii */
	ThresholdType   fr_thr_type;    /* fast radial thresholding type */
	float           fr_thr;         /* fast radial threshold */
	float			fr_beta_thr;

	/* Scale Space */
	SigmaSpacing ss_spacing;    /* scale space structure */
	float ss_sigma_start;       /* scale space start sigma in mm */
	float ss_sigma_end;         /* scale space final sigma in mm */
	float ss_sigma_inc;         /* scale space sigma increment */
	float ss_thr;               /* scale space responses threshold */

	/* debugging */
	int dump;                   /* shall we dump fast radial images to disk */
	char *dir_dump;             /* where to dump fast radial images */

	/* results directory where to dump intermediate results */
	char *dir_results;

	/* threading */
	pthread_t thread[2];            /* detection threads for left and right lung */ 
	det_thread_data thread_data[2]; /* detection thread parameters for left and right lungs */

} det_params_t;
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

/* detection parameters */
static det_params_t _DetectionParams;

/*
******************************************************************************
*                               PRIVATE PROTOTYPE DECLARATIONS
******************************************************************************
*/

static void*
_det_thread_routine ( void *arg );

/*
******************************************************************************
*                               GLOBAL FUNCTIONS
******************************************************************************
*/

/* DLL entry for Windows */
#if defined(WIN32)

BOOL APIENTRY
DllMain ( HANDLE hModule ,
		 DWORD dwReason ,
		 LPVOID lpReserved )
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
			fprintf ( stderr ,
				"\nError loading log parameters from : %s..." ,
				_logger_ini_f_name );
		}
	}

	/* here we've got a logging system so log what we are doing */
	LOG4CPLUS_DEBUG ( _log , " libmigdet_3d -> mig_init starting..." );

	/* global cad data */
	_CadData = data;

	/* get detection parameters from ini file */

	/* fast radial parameters */
	_DetectionParams.fr_radii =
		mig_ut_ini_getfloatarray ( d , PARAM_DET_FR_RADII , &( _DetectionParams.fr_num_radii ) );
	if ( _DetectionParams.fr_radii == NULL )
		return MIG_ERROR_IO;

	_DetectionParams.fr_thr_type = (ThresholdType) mig_ut_ini_getint ( d , PARAM_DET_FR_THR_TYPE , DEFAULT_PARAM_DET_FR_THR_TYPE );

	_DetectionParams.fr_thr = mig_ut_ini_getfloat ( d , PARAM_DET_FR_THR , DEFAULT_PARAM_DET_FR_THR );

	_DetectionParams.fr_beta_thr = mig_ut_ini_getfloat ( d , PARAM_DET_FR_BETA_THR , DEFAULT_PARAM_DET_FR_BETA_THR );


	/* scale space parameters */
	_DetectionParams.ss_spacing = (SigmaSpacing) 
		mig_ut_ini_getint ( d , PARAM_DET_SSPACE_SPACING , DEFAULT_PARAM_DET_SSPACE_SPACING );

	_DetectionParams.ss_sigma_inc = 
		mig_ut_ini_getfloat ( d , PARAM_DET_SSPACE_INCREMENT , DEFAULT_PARAM_DET_SSPACE_INCREMENT );

	_DetectionParams.ss_sigma_start = 0.5f *
		mig_ut_ini_getfloat ( d , PARAM_DET_SSPACE_MIN_DIAM , DEFAULT_PARAM_DET_SSPACE_MIN );

	_DetectionParams.ss_sigma_end = 0.5f * 
		mig_ut_ini_getfloat ( d , PARAM_DET_SSPACE_MAX_DIAM , DEFAULT_PARAM_DET_SSPACE_MAX );

	_DetectionParams.ss_thr = 
		mig_ut_ini_getfloat ( d , PARAM_DET_SSPACE_THR , DEFAULT_PARAM_DET_SSPACE_THR );        

	/* debug */
	_DetectionParams.dump = 
		mig_ut_ini_getint ( d , PARAM_DET_DUMP , DEFAULT_PARAM_DET_DUMP );

	_DetectionParams.dir_dump = 
		mig_ut_ini_getstring ( d , PARAM_DET_DIR_DUMP , DEFAULT_PARAM_DET_DIR_DUMP );


	/* results */
	_DetectionParams.dir_results = 
		mig_ut_ini_getstring ( d , PARAM_CAD_DIR_OUT , DEFAULT_PARAM_CAD_DIR_OUT );

	if ( _log.getLogLevel() <= INFO_LOG_LEVEL )
	{
		int i;

		std::stringstream os;
		os << "Processing options : ";
		os << "\n\t DUMP     : " << _DetectionParams.dump;
		os << "\n\t DUMP DIR : " << _DetectionParams.dir_dump;
		os << "\nProcessing parameters Fast Radial : ";
		os << "\n\t FR thr is %  : " << (int)_DetectionParams.fr_thr_type;
		os << "\n\t FR thr       : " << _DetectionParams.fr_thr;
		os << "\n\t FR num radii : " << _DetectionParams.fr_num_radii;
		os << "\n\t FR radii     : ";
		for ( i = 0 ; i < _DetectionParams.fr_num_radii ; ++i )
			os << " " << _DetectionParams.fr_radii[i];        
		os << "\nProcessing parameters Scale Space : ";
		os << "\n\t SS sigmas spacing  : " << (int)_DetectionParams.ss_spacing;
		os << "\n\t SS start sigma     : " << _DetectionParams.ss_sigma_start;
		os << "\n\t SS end sigma       : " << _DetectionParams.ss_sigma_end;
		os << "\n\t SS sigma increment : " << _DetectionParams.ss_sigma_inc;
		os << "\n\t SS thr             : " << _DetectionParams.ss_thr;
		LOG4CPLUS_INFO ( _log , os.str() );
	}

	LOG4CPLUS_DEBUG ( _log , " libmigdet_3d -> mig_init done..." );
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

	time_t t0 , t1;
	char path[MAX_PATH];

	t0 = getticks_sys();



	LOG4CPLUS_DEBUG ( _log , " libmigdet_3d -> mig_run starting..." );

	/******************************************************************************/
	/* try loading detection point from disc */

	/* right lung */
	snprintf ( path , MAX_PATH , "%s%c%s_%s_%s_det_r.txt" ,
		_DetectionParams.dir_results , MIG_PATH_SEPARATOR ,
		_CadData->dicom_data.patient_id ,
		_CadData->dicom_data.study_uid ,
		_CadData->dicom_data.series_uid );

	LOG4CPLUS_INFO( _log , " Trying to load detection results for right lung from : " << path );
	rc1 = mig_tag_read ( path , &( _CadData->det_r ) );
	if ( rc1 == MIG_OK )
	{
		_CadData->det_r._free = &free;
		LOG4CPLUS_INFO ( _log , " Loaded right lung detection data from disk...");
	}

	/* left lung */
	snprintf ( path , MAX_PATH , "%s%c%s_%s_%s_det_l.txt" ,
		_DetectionParams.dir_results , MIG_PATH_SEPARATOR ,
		_CadData->dicom_data.patient_id ,
		_CadData->dicom_data.study_uid ,
		_CadData->dicom_data.series_uid );

	LOG4CPLUS_INFO( _log , " Trying to load detection results for left lung from : " << path );
	rc2 = mig_tag_read ( path , &( _CadData->det_l ) );
	if ( rc2 == MIG_OK )
	{
		_CadData->det_l._free = &free;
		LOG4CPLUS_INFO ( _log , " Loaded left lung detection data from disk...");
	}

	if ( ( rc1 == MIG_OK ) && ( rc2 == MIG_OK ) )
	{
		LOG4CPLUS_INFO ( _log , " Using detection data from disk...");
		return MIG_OK;
	}

	LOG4CPLUS_INFO( _log , " Could not load detection data from disk. Performing full detection..." );
	if ( rc1 == MIG_OK )
		mig_lst_empty ( &( _CadData->det_r ) ); 

	if ( rc2 == MIG_OK )
		mig_lst_empty ( &( _CadData->det_l ) ); 

	/******************************************************************************/
	/* setup input to detection threads */

	/* right lung detection setup */
	_DetectionParams.thread_data[0].id             = 0;
	_DetectionParams.thread_data[0].Src            = _CadData->stack_r;
	_DetectionParams.thread_data[0].SrcSize        = &( _CadData->stack_r_s );
	_DetectionParams.thread_data[0].SrcBoundingBox = &( _CadData->bb[0] );
	_DetectionParams.thread_data[0].Original       = _CadData->stack;
	_DetectionParams.thread_data[0].OriginalSize   = &( _CadData->stack_s );
	_DetectionParams.thread_data[0].Results        = &( _CadData->det_r );

	_CadData->det_r._free = &free;

	/* left lung detection setup */
	_DetectionParams.thread_data[1].id             = 1;
	_DetectionParams.thread_data[1].Src            = _CadData->stack_l;
	_DetectionParams.thread_data[1].SrcSize        = &( _CadData->stack_l_s );
	_DetectionParams.thread_data[1].SrcBoundingBox = &( _CadData->bb[1] );
	_DetectionParams.thread_data[1].Original       = _CadData->stack;
	_DetectionParams.thread_data[1].OriginalSize   = &( _CadData->stack_s );
	_DetectionParams.thread_data[1].Results        = &( _CadData->det_l );

	_CadData->det_l._free = &free;

	/* spawn detection thread 0 */
	rc = pthread_create ( &( _DetectionParams.thread[0] ) , NULL ,
		&( _det_thread_routine ),  &( _DetectionParams.thread_data[0] ) );
	if ( rc != 0 )
	{
		LOG4CPLUS_FATAL ( _log , " libmigdet_3d -> mig_run pthread_create returned : " << rc );
		return MIG_ERROR_THREAD;
	}


	/* in order to preserve memory we apply fr3d in serial mode */
	if (FR_SERIAL)
	{
		/* wait for left thread to finish */
		pthread_join ( _DetectionParams.thread[0] , (void**) &rc );
		/* _CadData->stack_r = NULL; */
		memset ( &( _DetectionParams.thread_data[0] ) , 0x00 , sizeof( det_thread_data ) );

	}



	/* spawn detection thread 1 */
	rc = pthread_create ( &( _DetectionParams.thread[1] ) , NULL ,
		&( _det_thread_routine ),  &( _DetectionParams.thread_data[1] ) );
	if ( rc != 0 )
	{
		LOG4CPLUS_FATAL ( _log , " libmigdet_3d -> mig_run pthread_create returned : " << rc );
		return MIG_ERROR_THREAD;
	}

	if (!FR_SERIAL)
	{
		/* wait for left thread to finish */
		pthread_join ( _DetectionParams.thread[0] , (void**) &rc );
		/*_CadData->stack_r = NULL;*/
		memset ( &( _DetectionParams.thread_data[0] ) , 0x00 , sizeof( det_thread_data ) );
	}	

	/* wait for right thread to finish */
	pthread_join ( _DetectionParams.thread[1] , (void**) &rc ); 
	/*_CadData->stack_l = NULL;*/
	memset ( &( _DetectionParams.thread_data[1] ) , 0x00 , sizeof( det_thread_data ) );

	LOG4CPLUS_DEBUG ( _log , " libmigdet_3d -> mig_run end..." );

	t1 = getticks_sys();
	LOG4CPLUS_INFO ( _log , "Detection total timing : " << elapsed_sys( t1 , t0 ) << " secs." );


	/******************************************************************************/
	/* save detection lists to disk */

	/* right lung */
	snprintf ( path , MAX_PATH , "%s%c%s_%s_%s_det_r.txt" ,
		_DetectionParams.dir_results , MIG_PATH_SEPARATOR ,
		_CadData->dicom_data.patient_id ,
		_CadData->dicom_data.study_uid ,
		_CadData->dicom_data.series_uid );

	LOG4CPLUS_INFO( _log , " Trying to save detection results for right lung to : " << path );
	rc = mig_tag_write ( path , &( _CadData->det_r ) );
	if ( rc != MIG_OK )
	{
		LOG4CPLUS_WARN( _log , " Could not save detection results for right lung to disk..." );
	}
	else
	{
		LOG4CPLUS_INFO( _log , " Saved detection results for right lung to disk..." );
	}

	/* left lung */
	snprintf ( path , MAX_PATH , "%s%c%s_%s_%s_det_l.txt" ,
		_DetectionParams.dir_results , MIG_PATH_SEPARATOR ,
		_CadData->dicom_data.patient_id ,
		_CadData->dicom_data.study_uid ,
		_CadData->dicom_data.series_uid );

	LOG4CPLUS_INFO( _log , " Trying to save detection results for left lung to : " << path );
	rc = mig_tag_write ( path , &( _CadData->det_l ) );
	if ( rc != MIG_OK )
	{
		LOG4CPLUS_WARN( _log , " Could not save detection results for left lung to disk..." );
	}
	else
	{
		LOG4CPLUS_INFO( _log , " Saved detection results for left lung to disk..." );
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
_det_thread_routine ( void *arg )
{    
	int  rc;
	time_t tot0 , tot1;             /* total timing */
	time_t t0 , t1;                 /* partial timing */

	int j;

	det_thread_data *data = 
		(det_thread_data*) arg; /* input parameters */

	mig_fradial_t   *FRadial;     /* fast radial structure */
	mig_lst_t       FRadialRes;   /* fast radial results list */
	mig_im_region_t *FRadialReg;  /* single fast radial result */

	mig_sspace_t    *SSpace;       /* scale space structure */
	float           *SSpaceInput;  /* scale space input cube */
	mig_im_region_t *SSReg;        /* scale space single result */

	/* global timer */
	tot0 = getticks_sys();

	LOG4CPLUS_DEBUG ( _log , " _det_thread_routine : " << data->id );

	/* prepare fast radial structure */
	FRadial = mig_im_fradial_get ( _DetectionParams.fr_radii , 
		_DetectionParams.fr_num_radii , 
		_DetectionParams.fr_thr ,
		_DetectionParams.fr_thr_type ,
		_DetectionParams.fr_beta_thr );
	if ( FRadial == NULL )
	{
		LOG4CPLUS_FATAL ( _log , "Aborting detection thread. Memory error in : " << data->id );
		pthread_exit ( (void*)MIG_ERROR_MEMORY );
	}

	/* setup fast radial dumping */
	FRadial->dump = _DetectionParams.dump;

	/* path prefix for dumping fast radial results */
	if ( FRadial->dump == 1 )
	{
		snprintf ( FRadial->prefix , MAX_PATH , "%s%c%s_%s_%s_%d" ,
			_DetectionParams.dir_dump , MIG_PATH_SEPARATOR ,
			_CadData->dicom_data.patient_id ,
			_CadData->dicom_data.study_uid ,
			_CadData->dicom_data.series_uid , 
			data->id );
	}

	/* perform fast radial calcualtions */
	mig_lst_zero ( &FRadialRes );
	rc = mig_im_fradial_3d ( data->Src , data->SrcSize->w , data->SrcSize->h , 
		data->SrcSize->slices , FRadial , &FRadialRes );


	/* free segmented lung */
	/* GF20101001: comment following line if seg data is needed in further analysis */
	/*_CadData->seg_cleanup ( data->Src );*/

	mig_im_fradial_del ( FRadial );
	if ( rc != 0 )
	{
		pthread_exit( (void*)MIG_ERROR_MEMORY );
	}

	LOG4CPLUS_DEBUG ( _log , " number of fr regions :  " << mig_lst_len( &FRadialRes )  );

	if ( mig_lst_len( &FRadialRes) == 0 )
	{    
		pthread_exit( (void*)MIG_OK );
	}


	/* END FRADIAL PROCESSING: NOW DO WHAT IS NEEDED! */


	/******************************************************/
	/* SCALE SPACE 3D (BOILED DOWN)						  */
	/******************************************************/

    /* prepare scale space structure */
    SSpace = mig_im_sspace_get ( 0 , /* scale space type -> £D */
                                 _DetectionParams.ss_spacing ,
                                 _DetectionParams.ss_sigma_start / ( data->SrcSize->h_res ) ,
                                 _DetectionParams.ss_sigma_end   / ( data->SrcSize->h_res ) ,
                                 _DetectionParams.ss_sigma_inc ,
                                 _DetectionParams.ss_thr );
    if ( SSpace == NULL )
    {
        mig_lst_empty ( &FRadialRes );
        LOG4CPLUS_FATAL ( _log , "Aborting detection thread. Memory error in : " << data->id );
        pthread_exit ( (void*)MIG_ERROR_MEMORY );
    }

    if ( _log.getLogLevel() <= DEBUG_LOG_LEVEL )
    {
        std::stringstream os;
        os << "Scale space parameters : ";
        os << "\n\t WINDOW RADIUS     : " << SSpace->window_radius;
        os << "\n\t WINDOW LENGTH     : " << SSpace->window_len;
        os << "\n\t WINDOW PIXELS     : " << SSpace->window_voxels;
        os << "\n\t START SIGMA       : " << SSpace->sigma_start;
        os << "\n\t END SIGMA         : " << SSpace->sigma_end;
        os << "\n\t NUM SIGMAS        : " << SSpace->num_sigmas;
        os << "\n\t SIGMAS            : ";
        for ( j = 0 ; j < SSpace->num_sigmas ; ++j )
            os << "(" << SSpace->kernels[j]->sigma << "," << SSpace->kernels[j]->scale << ")";
    
        LOG4CPLUS_DEBUG ( _log , os.str() );
	}

    /* prepare scale space input buffer */
    SSpaceInput = (float*) calloc ( SSpace->window_voxels , sizeof(float) );
    if ( SSpaceInput == NULL )
    {
        mig_lst_empty ( &FRadialRes );
        mig_im_sspace_del ( SSpace );
            
        LOG4CPLUS_FATAL ( _log , "Aborting detection thread. Memory error in : " << data->id );
        pthread_exit ( (void*)MIG_ERROR_MEMORY );
    }
              
    LOG4CPLUS_DEBUG ( _log , " number of fr regions :  " << mig_lst_len( &FRadialRes )  );

	///************************************/
	//   /* PERFORM SCALE SPACE CALCULATIONS */
	//   /************************************/
	//
	///* scale space timing */
	   t0 = getticks_sys();



	//
	//
	/* process regions left after fast radial */
	while ( ( FRadialReg = (mig_im_region_t*) mig_lst_get_head ( &FRadialRes ) ) != NULL )
	{
		/* adjust fast radial region coordinates to global coordinates system */
		FRadialReg->centroid[0] += data->SrcBoundingBox->x0;
		FRadialReg->centroid[1] += data->SrcBoundingBox->y0;
		FRadialReg->centroid[2] += data->SrcBoundingBox->z0;

		LOG4CPLUS_DEBUG ( _log , " thread : " << data->id << " fr region :  "
			<< FRadialReg->centroid[0] << "," << FRadialReg->centroid[1] << ","
			<< FRadialReg->centroid[2] );



		/*****************************************************************/
		/* SIMPLIFIED SCALE SPACE ANALYSIS								 */
		/*(just compute responses of 3d mexican hat in the center)		 */
		/*****************************************************************/


		
		/* copy data from original stack into scale space input buffer */
	    mig_im_bb_cut_3d ( data->Original , data->OriginalSize->w , 
	                       data->OriginalSize->h , data->OriginalSize->slices ,
	                       (int)( FRadialReg->centroid[0] + 0.5f ) ,
	                       (int)( FRadialReg->centroid[1] + 0.5f ) ,
	                       (int)( FRadialReg->centroid[2] + 0.5f ) ,
	                       SSpaceInput , SSpace->window_radius );


		/* scale to [0,1] interval */
	    mig_im_util_mat2gray_32f ( SSpaceInput , SSpace->window_voxels , 0.0f , 65535.0f );
		
		/* perform scale space processing */
		SSReg = mig_im_sspace_radius ( SSpaceInput , SSpace ); 
		

	    if ( SSReg != NULL )
	    {
	        /* adjust coordinates to global coordinate system */
	        SSReg->centroid[0] += FRadialReg->centroid[0];
	        SSReg->centroid[1] += FRadialReg->centroid[1];
	        SSReg->centroid[2] += FRadialReg->centroid[2];
	                
	        /* add resulting region to tail of output list */
	        mig_lst_put_tail( data->Results , SSReg );
	        LOG4CPLUS_DEBUG ( _log , " thread : " << data->id << " ss region :  " \
		    << SSReg->centroid[0] << "," << SSReg->centroid[1] << "," << SSReg->centroid[2] );
	    }

		free ( FRadialReg );
		

	}
	free ( SSpaceInput );

	t1 = getticks_sys();
	LOG4CPLUS_INFO ( _log , "thread : " << data->id << " SS timing : " << ( elapsed_sys( t1 , t0 ) / 60.0f ) << " min." );


	mig_lst_empty  ( &FRadialRes );
	mig_im_sspace_del ( SSpace );

	

	//free ( FRadialReg );

	//mig_lst_empty  ( &FRadialRes );

	
	/* global timer */
	tot1 = getticks_sys();
	LOG4CPLUS_INFO ( _log , "Thread " << data->id << " total timing : " << ( elapsed_sys( tot1 , tot0 ) / 60.0f ) << " min." );

	LOG4CPLUS_DEBUG ( _log , "thread : " << data->id << " number of ss regions :  " << data->Results->num );

	//TODO: NUMBER OF REGIONS BECOMES ZERO!


	return ( (void*)MIG_OK );
}
