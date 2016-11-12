#include "mig_config.h"
#include "mig_defs.h"
#include "mig_error_codes.h"

#include "libmigio.h"
#include "libmigut.h"
#include "libmigim.h"

/*******************************************************************/
#define APP_NAME        "resize"
#define APP_DESC        "Resize stack in z"

/*******************************************************************/
/* PRIVATE */
/*******************************************************************/

MIG_C_LINKAGE_START

static void
_usage ();

static int
_resize ( const char *entry );

MIG_C_LINKAGE_END

static float _NewZres;
static char *_Input  = NULL;
static char *_Output = NULL;
static int _WC = -600;
static int _WW = 1500;

/*******************************************************************/
/* MAIN */
/*******************************************************************/

int
main ( int argc , char **argv )
{
   int i , rc , num, flag;
   char* lst[MAX_DIR_CONTENTS];
   
   if ( argc != 4 )
   {
      _usage ();
      exit ( EXIT_FAILURE );
   }

   /* process input arguments */
        
   /* desired z resolution */
   errno = 0;
   _NewZres = (float) atof (  argv[1] );
   if ( _NewZres == 0 && errno != 0 )
   {
      fprintf ( stderr , "\nERROR. %s is not a valid float..." , argv[1] );
      exit ( EXIT_FAILURE );
   }
   
   /* input */
   _Input  = argv[2];
        
   /* output */
   _Output = argv[3];
        
   /* check wether second entry is file list or directory */
   rc = mig_ut_fs_isfile ( _Input , &flag );
   if ( rc != MIG_OK )
   {
      fprintf ( stderr , "\nERROR. mig_ut_fs_isfile ..." );
      fprintf ( stderr , "\nERROR. Reading %s..." , _Input );
      exit ( EXIT_FAILURE );
   }
        
   /* given entry is file -> list of directories*/
   if ( flag == 1 )
   {
      printf ( "\n%s is a file list..." , _Input );
      rc = mig_ut_fs_rlist ( _Input , lst , &num );
      if ( rc != MIG_OK )
      {
         fprintf ( stderr , "\nERROR. mig_ut_fs_rlist ..." );
         fprintf ( stderr , "\nERROR. Reading list %s..." , _Input );
         exit ( EXIT_FAILURE );
      }
        
      for ( i = 0 ; i < num ; ++i )
      {
         rc = _resize ( lst[i] );
         if ( rc != MIG_OK )
         {
            fprintf ( stderr , "\nERROR. Processing %s..." , lst[i] );
            exit ( EXIT_FAILURE );
         }
      }
      
      exit( EXIT_SUCCESS );
   }

   printf ( "\n%s is a directory..." , _Input );
   rc = _resize ( _Input );
   if ( rc != MIG_OK )
   {
      fprintf ( stderr , "\nERROR. Processing %s..." , _Input );
      exit ( EXIT_FAILURE );
   }        
}

/*******************************************************************/
/* PRIVATE */
/*******************************************************************/

static void
_usage ()
{
   printf ( "\nResize dicom stack in z dimension and save as mat.");
   printf ( "\nDicom stack is read from directory or from list." );
   printf ( "\n\nUsage : %s newzres [dirname|listname] dirout" , APP_NAME );
   printf ( "\n\t newzres   - target z resolution" );
   printf ( "\n\t [dirname  - name of input dicom directory" );
   printf ( "\n\t [listname - name of input dicom directories list" );
   printf ( "\n\t dirout    - name of output mat directory" );
   printf ( "\n" );
   printf ( "\nATTENTION : all paths should be absolute (full)...\n" );
}

/*******************************************************************/
static int
_resize ( const char *entry )
{
   int rc , flag , i = 0;
   mig_dcm_data_t dataDicom;
   mig_size_t sizeOriginal;
   mig_size_t sizeNew;
   Mig16u *dataOriginal = NULL;
   Mig16u *dataNew = NULL;
   char DumpName[MAX_PATH];
        
   printf ( "\nProcessing %s..." , entry );
   
   /* check if entry exists and if it is a directory */
   rc = mig_ut_fs_isdir ( (char*)entry , &flag );
   if ( rc != MIG_OK )
   {
      fprintf ( stderr , "\nERROR. mig_ut_fs_isdir ..." );
      fprintf ( stderr , "\nERROR. Reading %s..." , entry );
      return -1;
   }

   if ( flag == 0 )
   {
      fprintf ( stderr , "\nERROR. %s is not a directory..." , entry );
      return -1;
   }
   
   /* load dicom information */
   snprintf ( dataDicom.storage , MAX_PATH , "%s%c" , entry , MIG_PATH_SEPARATOR );
   
   rc = mig_dcm_get_info ( &dataDicom , &sizeOriginal );
   if ( rc != MIG_OK )
   {
      fprintf ( stderr , "\nERROR. mig_dcm_get_info ..." );
      fprintf ( stderr , "\nERROR. Reading information from %s..." , entry );
      return -1;
   }
        
   /* load dicom image data */
   rc = mig_dcm_rdir_16u (  &dataOriginal , &dataDicom , &sizeOriginal , _WC , _WW , MIG_VOI_LUT_STORED, -1 );
   if ( rc != MIG_OK )
   {
      fprintf ( stderr , "\nERROR. mig_dcm_rdir_16u ..." );
      fprintf ( stderr , "\nERROR. Reading data from %s..." , entry );
      return -1;
   }

   /* if input has already the desired resolution just save output to MATLAB mat file */
   if ( sizeOriginal.z_res == _NewZres )
   {
      dataNew = dataOriginal;
      dataOriginal = NULL;
      memcpy ( &sizeNew , &sizeOriginal , sizeof( mig_size_t ) );
   }
   else
   {
      /* set TmpSize z_res to desired z resolution */
      sizeNew.z_res = _NewZres;
      
      /* resample */
      rc = mig_im_geom_resize_z ( dataOriginal , &sizeOriginal , &dataNew , &sizeNew );
      if ( rc != MIG_OK )
      {
         /* error resampling */
         fprintf ( stderr , "\nERROR. Resampling %s..." , entry );
         return -1;        
      }
   }
   
   /* dump original stack to disk in MATLAB .mat 
      format. For this to work libmigio library 
      should have been compiled with MATLAB support
      enabled. See mig_config.h and mig_io_mat.h
      for flags enabeling MATLAB support */        
   
   snprintf ( DumpName , MAX_PATH , "%s%c%s.mat" ,
              _Output , MIG_PATH_SEPARATOR , dataDicom.patient_id );

   rc = mig_io_mat_w_ct ( DumpName , dataNew , &dataDicom , &sizeNew , NULL, NULL);
   if ( rc != MIG_OK )
   {
      fprintf ( stderr , "\nERROR. Writing mat file %s..." , DumpName );
      return -1;
   } 

   /* clean up */
   if ( dataOriginal != NULL )
      mig_free ( dataOriginal );
   
   if ( dataNew != NULL )
      mig_free ( dataNew );
        
   /* original image file names */
   while ( dataDicom.file_names[i] != NULL )
   {
      free ( dataDicom.file_names[i] );
      ++ i;
   }

   free ( dataDicom.file_names );
   return 0;
}
