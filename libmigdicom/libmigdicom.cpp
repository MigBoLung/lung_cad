#include "libmigdicom.h"

/*******************************************************************/
/* PRIVATE DATA */
/*******************************************************************/

/* logger for the whole cad */
static Logger _log = 
    Logger::getInstance ( CAD_LOGGER_NAME );

/* global cad data */
static
mig_cad_data_t*
_CadData = NULL;

/* Maximum slice number to load */
static int
_MaxSliceNumber;

/* Force DICOM Window Center and Window Width Externaly 
   using values from ini file. If set to 0 values inside
   the DICOM files are used */
static int
_VoiLutType;

/* Forced DICOM Window Center */
static int
_WindowCenter;

/* Forced DICOM Window Width */
static int
_WindowWidth;

/* Ignore percent for histogram based VOI LUT */
static float
_IgnorePerc;

/* resample */
static int
_ResampleFlg;

/* target z resolution */
static float
_TargetZRes;

/* Should we write original data
   out as MAT file */
static int
_DumpOriginalFlg;

/* output path from ini params file */
static char*
_OutputPath = NULL;

/*******************************************************************/
/* EXPORTS */
/*******************************************************************/

/* DLL entry for Windows */
#if defined(WIN32)

BOOL APIENTRY
DllMain ( HANDLE hModule , DWORD dwReason , LPVOID lpReserved )
{
	return TRUE;
}

#endif /* Win32 DLL */

/*******************************************************************/
/* Initialization function.
   1. Initialize logging system.
   2. Get parameters from ini structure and
      copy them to local variables.
   3. Zero out data in global structure.
*/
/*******************************************************************/
int
mig_init ( mig_dic_t *d , mig_cad_data_t *data )
{
    /* setup logging system */
    char *_logger_ini_f_name = mig_ut_ini_getstring ( d , PARAM_LOG_INI , DEFAULT_PARAM_LOG_INI );
    if ( _logger_ini_f_name != NULL )
    {
        try
        {
            PropertyConfigurator::doConfigure ( _logger_ini_f_name );
        }
        catch ( ... )
        {
            return MIG_ERROR_IO;
        }
    }

    /* here we've got a logging system so log what we are doing */
    LOG4CPLUS_DEBUG ( _log , " libmigdicom -> mig_init starting..." );

    /* copy pointer to global data */
    _CadData = data;

    /* VOI LUT Type to use */
    _MaxSliceNumber = mig_ut_ini_getint ( d , PARAM_DCM_SLICE_LIMIT ,
                                          DEFAULT_PARAM_DCM_SLICE_LIMIT );

    /* VOI LUT Type to use */
    _VoiLutType = mig_ut_ini_getint ( d , PARAM_DCM_VOI_LUT_TYPE , 
                                          DEFAULT_PARAM_DCM_VOI_LUT_TYPE );

    /* Forcec VOI LUT Window Center */
    _WindowCenter = mig_ut_ini_getint ( d , PARAM_DCM_VOI_LUT_WC , 
                                            DEFAULT_PARAM_DCM_VOI_LUT_WC );

	/* Forcec VOI LUT Window Width */
    _WindowWidth = mig_ut_ini_getint ( d , PARAM_DCM_VOI_LUT_WW , 
                                           DEFAULT_PARAM_DCM_VOI_LUT_WW );

    /* Histogram VOI LUT Ignore Percentage */
    _IgnorePerc = mig_ut_ini_getfloat ( d , PARAM_DCM_VOI_LUT_IGNORE_PERC ,
                                            DEFAULT_PARAM_DCM_VOI_LUT_IGNORE_PERC );

    /* Resample flag */
    _ResampleFlg = mig_ut_ini_getint ( d , PARAM_DCM_RESAMPLE , DEFAULT_PARAM_DCM_RESAMPLE );

    /* Resampling target z resolution */
    _TargetZRes = mig_ut_ini_getfloat ( d , PARAM_DCM_ZRES , DEFAULT_PARAM_DCM_ZRES );

    /* Dump original to .MAT file ? */
    _DumpOriginalFlg = mig_ut_ini_getint ( d , PARAM_DCM_DUMP , DEFAULT_PARAM_DCM_DUMP );

    /* output path for writing .MAT files */
    _OutputPath = mig_ut_ini_getstring ( d , PARAM_CAD_DIR_OUT , DEFAULT_PARAM_CAD_DIR_OUT );

    /* Log parameters */
    if ( _log.getLogLevel() <= INFO_LOG_LEVEL )
    {
        std::stringstream os;
        os << "Input options                     : ";
        os << "\n\t DUMP                         : " << _DumpOriginalFlg;
        os << "\n\t DUMP DIR                     : " << _OutputPath;
        os << "\n\t Resample                     : " << _ResampleFlg;
        os << "\n\t Wanted Z res                 : " << _TargetZRes;
        os << "\n\t Max slice number             : " << _MaxSliceNumber;
        os << "\n\t Requested VOI LUT            : " << _VoiLutType;
        switch ( _VoiLutType )
        {
            case MIG_VOI_LUT_STORED :
                os << "\t VOI LUT STORED";
                break;
            
            case MIG_VOI_LUT_FORCE :
                os << "\t VOI LUT FORCED";
                break;
            
            case MIG_VOI_LUT_WIN_MIN_MAX :
                os << "\t VOI LUT MIN MAX";
                break;

            case MIG_VOI_LUT_WIN_MIN_MAX_NO_EXTREMES :
                os << "\t VOI LUT MIN MAX NO EXTREMES";
                break;

            case MIG_VOI_LUT_HIST :
                os << "\t VOI LUT HISTOGRAM";
                break;
            
            default :
                os << "\t UNKNOWN";
        }
        os << "\n\t Forced VOI LUT Window Center : " << _WindowCenter;
		os << "\n\t Forced VOI LUT Window Width  : " << _WindowWidth;
        os << "\n\t Histogram VOI LUT ignore %   : " << _IgnorePerc;
        
        LOG4CPLUS_INFO ( _log , os.str() );
    }

    /* zero out global data structures we are going to use */
    _CadData->stack = NULL;
    memset ( &( _CadData->stack_s ) , 0 , sizeof( mig_size_t ) );
    memset ( &( _CadData->dicom_data ) , 0 , sizeof( mig_dcm_data_t ) );

    LOG4CPLUS_DEBUG ( _log ,  " libmigdicom -> mig_init done..." );
    return MIG_OK;
}

/*******************************************************************/
/* Function executed for each stack of data :
   1. Load dicom information for files in a given directory.
   2. Load image data.
   3. Resampled image data if asked to.
*/
/*******************************************************************/
int
mig_run ()
{
    int rc;         /* return code */

    LOG4CPLUS_DEBUG ( _log ,  " libmigdicom -> mig_run starting..." );

    /* first load dicom directory information */
    LOG4CPLUS_DEBUG ( _log , " Loading dicom info..." );
    
    rc = mig_dcm_get_info (  &( _CadData->dicom_data ) , &( _CadData->stack_s ) );
    if ( rc != MIG_OK )
    {
        LOG4CPLUS_FATAL ( _log , " Dicom info loader returned : " <<  rc );
        goto error;
    }

    /* dump dicom information to log  */
    if ( _log.getLogLevel() <= INFO_LOG_LEVEL )
    {
        std::stringstream os;
        mig_dcm_dump_info ( &( _CadData->dicom_data ) , &( _CadData->stack_s ) , os );
        LOG4CPLUS_INFO ( _log , os.str() );   
    }

    /* load dicom image data */
    LOG4CPLUS_DEBUG ( _log , " Loading dicom image data..." );
		
    rc = mig_dcm_rdir_16u (  &( _CadData->stack ) , &( _CadData->dicom_data ) ,
                             &( _CadData->stack_s ) , 
                              _VoiLutType , 
                              _WindowCenter , 
                              _WindowWidth , 
                              _MaxSliceNumber );
    if ( rc != MIG_OK )
    {
        LOG4CPLUS_FATAL ( _log , "Dicom image loader returned : " <<  rc );
        goto error;
    }

    /* Resample data if asked to */
    if ( _ResampleFlg == 1 )
    {
        Mig16u *TmpBuffer = NULL;
        mig_size_t TmpSize;

        /* set TmpSize z_res to desired z resolution */
        TmpSize.z_res = _TargetZRes;

        /* resample */
        rc = mig_im_geom_resize_z ( _CadData->stack , &_CadData->stack_s , &TmpBuffer , &TmpSize );
        if ( rc == MIG_OK )
        {
            /* switch stacks */
            mig_free ( _CadData->stack );
            _CadData->stack = TmpBuffer;

            /* copy size information */
            memcpy ( &_CadData->raw_s ,  &_CadData->stack_s , sizeof( mig_size_t ) );
            memcpy ( &_CadData->stack_s , &TmpSize , sizeof( mig_size_t ) );

            /* signal resampled */
            _CadData->resampled = 1;
        }
        else
        {
            /* error resampling */
            LOG4CPLUS_WARN ( _log , "Dicom resampler returned : " <<  rc );
                        
            /* zero out raw_s as it is the same as stack_s */
            memset ( &_CadData->raw_s , 0 , sizeof(mig_size_t) );
            _CadData->resampled = 0;
        }
    }

    /* Write original images to .MAT file */
    if ( _DumpOriginalFlg == 1 )
    {
        /* dump original stack to disk in MATLAB .mat  format. For this to work libmigio library 
           should have been compiled with MATLAB support enabled. See mig_config.h and mig_io_mat.h
           for flags enabeling MATLAB support */
                
        char DumpName[MAX_PATH];

        snprintf ( DumpName , MAX_PATH , "%s%c%s_%s_%s_or.mat" , _OutputPath , 
            MIG_PATH_SEPARATOR ,
            _CadData->dicom_data.patient_id ,
            _CadData->dicom_data.study_uid ,
            _CadData->dicom_data.series_uid );

        rc = mig_io_mat_w_ct (  DumpName , _CadData->stack ,
                &( _CadData->dicom_data ) , &( _CadData->stack_s ) , 
                &( _CadData->stack_s ) , NULL );

	if ( rc == -1 )
	{
	  LOG4CPLUS_ERROR ( _log , "mig_io_mat_w_ct : error writing original stack to mat file..." );
	}
            
    } /* dumping */

    /* clean original image file names */
    if ( _CadData->dicom_data.file_names )
    {
        int i = 0;
        while ( _CadData->dicom_data.file_names[i] != NULL )
        {
            free ( _CadData->dicom_data.file_names[i] );
            ++ i;
        }

        free ( _CadData->dicom_data.file_names );
        _CadData->dicom_data.file_names = NULL;
    }

    LOG4CPLUS_DEBUG ( _log , " libmigdicom -> mig_run done..." );

    return MIG_OK;

error :
        
    /* free data if neccessary */
    if ( _CadData->stack != NULL )
    {
        mig_free ( _CadData->stack );
        _CadData->stack = NULL;
    }

    mig_memz ( &( _CadData->stack_s ) , sizeof( mig_size_t ) );

    /* original image file names */
    if ( _CadData->dicom_data.file_names )
    {
        int i = 0;
        while ( _CadData->dicom_data.file_names[i] != NULL )
        {
            free ( _CadData->dicom_data.file_names[i] );
            ++ i;
        }
        free ( _CadData->dicom_data.file_names );
    }

    mig_memz ( &( _CadData->dicom_data ) , sizeof( mig_dcm_data_t ) );
    return rc;
}

/*******************************************************************/
/* Function executed for each stack of data at the end of processing :
   1. Free all data that has been allocated by the mig_run function
      of this dll.
*/
/*******************************************************************/
void
mig_cleanup ( void* data )
{
    if ( data )
        free ( data );
}

/*******************************************************************/
void
mig_info ( mig_dll_info_t* info )
{
    LOG4CPLUS_DEBUG ( _log , " libmigdicom -> mig_info starting..." );
    LOG4CPLUS_DEBUG ( _log , " libmigdicom -> mig_info done..." );
}
