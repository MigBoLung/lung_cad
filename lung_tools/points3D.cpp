#include "mig_config.h"
#include "mig_defs.h"
#include "mig_data_types.h"
#include "mig_data_image.h"
#include "mig_error_codes.h"

#include "libmigim.h"
#include "libmigio.h"
#include "libmigut.h"
#include "libmigtag.h"

#include "mig_data_cad.h"

/***************************************************/
#define APP_NAME        "points3D"
#define APP_DESC        "Convert 3D point set to 3D binary volume"

/***************************************************/
/* PRIVATE */
/***************************************************/

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
        int rc , flag;

        mig_dcm_data_t  DicomInfo;
        mig_size_t      DicomTmpSize;

        Mig8u           *Data = NULL;
        Mig8u           *Slice;
        mig_size_t      DataSize;

        mig_lst_t *Tags = NULL;
        mig_im_region_t *obj = NULL;

        if ( argc != 4 )
        {
                _usage ();
                exit ( EXIT_FAILURE );
        }

        /* check wether second cmd line argument is a directory */
        rc = mig_ut_fs_isdir ( argv[1] , &flag );
        if ( rc != MIG_OK )
        {
                fprintf ( stderr , "\nERROR. Reading %s..." , argv[1] );
                exit ( EXIT_FAILURE );
        }

        if ( flag == 0 )
        {
                fprintf ( stderr , "\nERROR. %s is not a directory..." , argv[1] );
                exit ( EXIT_FAILURE );
        }

        /* prepare for loading */
        snprintf ( DicomInfo.storage , MAX_PATH , "%s%c" ,
                   argv[1] , MIG_PATH_SEPARATOR );

        /* read dicom information */
        rc = mig_dcm_get_info ( &DicomInfo , &DicomTmpSize );
        if ( rc != MIG_OK )
        {
                fprintf ( stderr , "\nERROR. Loading dicom info from : %s..." , argv[1] );
                exit( EXIT_FAILURE );
        }

        /* prepare Tags list */
        Tags = mig_lst_alloc ();
        if ( Tags == NULL )
        {
                fprintf ( stderr , "\nERROR. Memory..." );
                exit ( EXIT_FAILURE );
        }

        /* read tag file */
        rc = mig_tag_read ( argv[2] , Tags );
        if ( rc != MIG_OK )
        {
                fprintf ( stderr , "\nERROR. Loading tag info from : %s..." , argv[2] );
                exit ( EXIT_FAILURE );
        }

        /* build up 3D binary volume */
        memcpy ( &DataSize , &DicomTmpSize , sizeof(mig_size_t) );

        /* modify size bytes */
        DataSize.size >>= 1;
        DataSize.size_stack >>= 1;

        Data = (unsigned char*) calloc ( DataSize.dim_stack , sizeof(Mig8u) );
        if ( Data == NULL )
        {
                fprintf ( stderr , "\nERROR. Memory..." );
                exit ( EXIT_FAILURE );
        }

        /* while there are still any tags inside tag list */
        while ( obj = (mig_im_region_t*) mig_lst_get_head ( Tags ) )
        {
                Slice = Data + ( (int)( obj->centroid[2])  ) * DataSize.dim;
                Slice[((int)(obj->centroid[0])) + ((int)(obj->centroid[1])) * DataSize.w ] = 0x01;

                free ( obj );
        }

        rc = mig_io_mat_w_masks ( argv[3] , Data , &DicomInfo , &DataSize , NULL );
        if ( rc != MIG_OK )
        {
                fprintf ( stderr , 
                        "\nERROR. Writing : %s. Code was : %d..." , argv[3] , rc );
                exit ( EXIT_FAILURE );
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
        printf ( "\nEncode a list of 3D points as a 3D binary volume.");
        printf ( "\nSave resulting volume to MATLAB .mat file." );
        printf ( "\nUsage : %s [input dicom directory] [corresponding tag file] [output mat file]..." , APP_NAME );
        printf ( "\n" );
}
