#include "mig_config.h"
#include "mig_defs.h"
#include "mig_error_codes.h"

/* make sure libmigio has been compiled with MATLAB support */
# if !(defined(MATLAB))
#error "Please recompile source with MATLAB macro on"
#endif

#include "libmigio.h"
#include "libmigut.h"

/***************************************************/
#define APP_NAME        "convert"
#define APP_DESC        "Convert dicom images from a directory to matlab .mat file"

/***************************************************/
/* PRIVATE */
/***************************************************/

#define MAX_SLICES  700

MIG_C_LINKAGE_START

static void
_usage ();

MIG_C_LINKAGE_END

/***************************************************/
/* MAIN */
/***************************************************/

int
main ( int argc , char **argv )
{
    int rc;
    mig_dcm_data_t DicomData;
    mig_size_t     SizeData;
    mig_roi_t      BoundingBoxData;
    Mig16u         *Data = NULL;
    int            WindowCenter;
    int            WindowWidth;
    int            ForceFlg;

    if ( argc != 6 )
    {
        _usage ();
        exit ( EXIT_FAILURE );
    }

    ForceFlg     = (int) atoi( argv[3] );
    WindowCenter = (int) atoi( argv[4] );
    WindowWidth  = (int) atoi( argv[5] );

    printf ( "\nInput directory is    : %s " , argv[1] );
    printf ( "\nOutput MATLAB file is : %s " , argv[2] );
    printf ( "\nMax slices is         : %d " , MAX_SLICES );
    printf ( "\nForce flag is         : %d " , ForceFlg );
    printf ( "\nWindowCenter is       : %d " , WindowCenter );
    printf ( "\nWindowWidth  is       : %d " , WindowWidth );

    /* write location of dicom file to DicomData var */
    snprintf ( (char*) &(DicomData.storage) , MAX_PATH , "%s" , argv[1] ); 

    printf ( "\nTrying to read dicom meta data..." );
    rc = mig_dcm_get_info ( &DicomData , &SizeData );
    if ( rc != MIG_OK )
    {
        fprintf ( stderr , "\nCould not load dicom information for files. Error code was %d" , rc );
        exit( EXIT_FAILURE );
    }

    std::stringstream os;
    printf( "\nORIGINAL" );
    mig_dcm_dump_info ( &DicomData , &SizeData , os );
    printf( "\n" );
    cout << os.str();

    printf ( "\nTrying to load dicom images..." );
    rc =  mig_dcm_rdir_16u ( &Data , 
                             &DicomData , 
                             &SizeData ,
                             ForceFlg , 
                             WindowCenter , 
                             WindowWidth ,
                             MAX_SLICES );
    if ( rc != MIG_OK )
    {
        fprintf ( stderr , "\nCould not load dicom images. Error code was %d" , rc );
        exit( EXIT_FAILURE );
    }

    printf( "\nAFTER LOADING" );
    mig_dcm_dump_info ( &DicomData , &SizeData , os );

    /* zero out bounding box */
    memset ( &BoundingBoxData , 0x00 , sizeof(mig_roi_t) );

    printf ( "\nTrying to write mat file..." );
    rc = mig_io_mat_w_ct ( argv[2] , 
                           Data , 
                           &DicomData , 
                           &SizeData , 
                           &SizeData , 
                           &BoundingBoxData );
    if ( rc != MIG_OK )
    {
        fprintf ( stderr , "\nCould not write data to mat file. Error code was %d" , rc );
        exit( EXIT_FAILURE );
    }
        
    exit ( EXIT_SUCCESS );
}

/***************************************************/
/* PRIVATE */
/***************************************************/

static void
_usage ()
{
    printf ( "\n%s" , APP_DESC );
    printf ( "\nConvert dicom images read from a single");
    printf ( "\ndirectory into MATLAB .mat file." );
    printf ( "\nUsage : %s dicom_dir file.mat ForceFlg WindowCenter WindowWidth..." , APP_NAME );
    printf ( "\n" );
}
