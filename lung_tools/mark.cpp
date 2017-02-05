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
#define APP_NAME        "mark"
#define APP_DESC        "Mark DICOM stack with tags"

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

static RGB_COLOR TagColor = { 0xFF , 0x00 , 0x00 };

int
main ( int argc , char **argv )
{
   int i , rc , flag;
        
   Mig16u *DicomData = NULL;       /* input stack */
   Mig16u *Slice;                  /* current slice to mark */
   
   mig_dcm_data_t DicomInfo;       /* input stack dicom data */
   mig_size_t DicomDataSize;       /* input stack size */
        
   Mig8u  **RGBCanvases = NULL;    /* drawing area */
        
   mig_lst_t *Tags = NULL;         /* cad results */
   mig_im_region_t *obj = NULL;    /*  single nodule from tag file */
   
   char OutFileName[MAX_PATH];
   
   if ( argc != 4 )
   {
      _usage ();
      exit ( EXIT_FAILURE );
   }
   
   /* check wether second entry is a directory */
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
   
   /* try making new directory for output */
   rc = mig_ut_fs_mkdir ( argv[3] );
   if ( rc != MIG_OK )
   {
      fprintf ( stderr , "\nERROR. Creating %s..." , argv[3] );
      exit ( EXIT_FAILURE );
   }
   
   /* prepare for loading */
   snprintf ( DicomInfo.storage , MAX_PATH , "%s%c" , argv[1] , MIG_PATH_SEPARATOR );
   
   /* read dicom information */
   rc = mig_dcm_get_info ( &DicomInfo , &DicomDataSize );
   if ( rc != MIG_OK )
   {
      fprintf ( stderr , "\nERROR. Loading dicom info from : %s..." , argv[1] );
      exit( EXIT_FAILURE );
   }
  
   /* TODO: remove magic numbers */ 
   /* read dicom data */        
   rc = mig_dcm_rdir_16u ( &DicomData , &DicomInfo , &DicomDataSize , -600 , 1600, 1000, 1 );
   if ( rc != MIG_OK )
   {
      fprintf ( stderr , "\nERROR. Loading dicom data from : %s..." , argv[1] );
      exit ( EXIT_FAILURE );
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

   /* prepare RGB canvases for writing */
   RGBCanvases = (unsigned char**) malloc ( DicomDataSize.slices * sizeof(unsigned char*) );
   if ( RGBCanvases == NULL )
   {
      fprintf ( stderr , "\nERROR. Memory..." );
      exit ( EXIT_FAILURE );
   }

   *RGBCanvases = (unsigned char*) malloc ( 3 * DicomDataSize.dim_stack * sizeof(unsigned char) );
   if ( *RGBCanvases == NULL )
   {
      fprintf ( stderr , "\nERROR. Memory..." );
      exit ( EXIT_FAILURE );
   }

   /* setup pointers and copy original image */
   for ( i = 0 ; i < DicomDataSize.slices ; ++i )
   {
      RGBCanvases[i] = RGBCanvases[0] + i * DicomDataSize.dim * 3;
      Slice = DicomData + DicomDataSize.dim * i;
      mig_im_util_scale_16u_rgb ( Slice , RGBCanvases[i] , DicomDataSize.w , DicomDataSize.h );
   }

   /* while there are still any tags inside tag list */
   while ( obj = (mig_im_region_t*) mig_lst_get_head ( Tags ) )
   {
      /* draw circle inside canvas */
      mig_im_draw_circle_rgb ( RGBCanvases[(int) obj->centroid[2]] , DicomDataSize.w , DicomDataSize.h ,
                               (int) ( obj->centroid[0] ) , (int) ( obj->centroid[1] ) , 
                               (int) ( obj->radius ) , TagColor );

      free ( obj );
   }
   
   /* write taged stack to disk */
   for ( i = 0 ; i < DicomDataSize.slices ; ++i , ++RGBCanvases )
   {
      /* prepare output file name */
      snprintf ( (char*) OutFileName , MAX_PATH , "%s%c%04d.tif" , argv[3] , MIG_PATH_SEPARATOR ,i );
      
      /* write canvas to tiff file */
      rc = mig_io_tif_w_rgb ( OutFileName , *RGBCanvases , DicomDataSize.w , DicomDataSize.h );
      if ( rc != MIG_OK )
      {
         fprintf ( stderr , "\nERROR. Writing : %s..." , OutFileName );
         exit ( EXIT_FAILURE );
      }
      
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
   printf ( "\nMark dicom images using information from");
   printf ( "\nText tag files. Save marked images as a sequence" );
   printf ( "\nof tiff files." );
   printf ( "\nUsage : %s [input dicom directory][corresponding tag file][output directory]..." , APP_NAME );
   printf ( "\n" );
}

