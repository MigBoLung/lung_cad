/*
 ***************************************************************************
 *     TRAINING FEATURES EXTRACTION
 *	train classifier using gt tags and feature extraction from cad
 * 	system
 *
 *  AUTHOR:		Gianluca Ferri
 *  CREATED:	June 2010
 *
 *
 *  UPDATE:	in order to easily make changes only to svm parameters we just create a
 *			datafile suitable for libsvm. Then we create model files from the
 *			command line 
 *
 * IMPORTANT: in order to work with detected regions and segmented data, dumped results from
 *		      CAD MUST be available. (_seg, _det_l.txt, and _det_r.txt files)
 *			  Pay also attention that resize is done with the same parameters
 ***************************************************************************
 */



/**********************************************************************
*			WHAT WE NEED
*********************************************************************/
//for each patient in list (fname with list, passed as arg)

// load _CadData for current patient

// resize to cubic pixel (based on dicom infos)


// load GT

//POSITIVES ADDITION
// for each element in GT
// construct obj3d
// extract features
// append to feature list
// append label 1

//NEGATIVES ADDITION
// 1.generate random centroid and radius
// check for BB overlap with TP
// if near TP skip goto 1  (....)
// construct obj3d
// extract features
// append to feature list
// append label -1



//compute and save whitening parameters

//whiten

// write svmlib data file

// NOT ANYMORE call svm_train for feature list

// NOT ANYMORE write model file

/************************************************************************************/

#include "mig_config.h"
#include "mig_defs.h"
#include "mig_data_dicom.h"
#include "mig_data_cad.h"
#include "mig_error_codes.h"

#include "libmigio.h"
#include "libmigut.h"
#include "libmigim.h"
#include "libmigst.h"
#include "libmigtag.h"
#include "libmigsvm.h"
#include "libmigwhitening.h"

#include "libmig_feat_zernike_mip.h"


/*******************************************************************
* C++ STUFF				
*******************************************************************/
/*#include <vector>*/

/*******************************************************************/
#define APP_NAME        "training"
#define APP_DESC        "Train svm using gt and random cut examples"

/* DUMP IMAGES ? */
//#define _DEBUG_WIN_FEAT_DUMP
//#define _DEBUG_WIN_TRAIN_DUMP

/*******************************************************************/
#define POS_LABEL 1
#define NEG_LABEL 0

#define NSIGMA_WHITE 1

#define NFP_PER_PAT 1000 //not used anymore 
#define NEGPOSRATIO 10
#define MIN_NEG 10 //minimum negative to add per patient (for patients without positives)
#define MIN_DIAM 5
#define MAX_DIAM 56
#define DIAM_INC_FACT 2

/* parameters for the elimination of nodules cut by segmentation
   a little cube is cut around the center and we keep the region
   if the number of on voxels is >= BLANK_RATIO */

#define BLANK_RATIO 0.3f //ratio of on voxels respect to all voxels
#define BLANK_RADIUS 3  //radius of check cube
/********************************************************************
*     FEATURE EXTRACTION PARAMETERS
********************************************************************/
#define MOM_ORDERS {1,2,3,4,5,8,9,14}
#define MOM_ORDERS_LEN 8
#define NDIR 3
#define MIP_RATIO 0.7f
#define RESIZED_LEN 64

#define FEAT_LEN MOM_ORDERS_LEN

#define FEAT_FNAME "features.dat"
#define SCALE_FNAME "scales.dat"

/*******************************************************************/
/* PRIVATE */
/*******************************************************************/

MIG_C_LINKAGE_START

static void
_usage ();

static int
_load_and_resize ( const char *entry );

static int
_stack2seg ( );

static int
_load_gt ( mig_lst_t *nod_lst);

static int
_create_det_negs ( mig_lst_t *nod_lst, mig_lst_t *pos_lst,
				  int min_diam_px, int max_diam_px, float diam_inc);

static int
_create_ran_list ( mig_lst_t *nod_lst, mig_lst_t *remove_lst,
				  int nelem, int min_diam_px, int max_diam_px);

/*add features in lst, if label == 1 removes elements cut by segmentation */
/*returns: number of added elements */
static int
_add_features ( mig_lst_t *pos_lst , int label, mig_lst_t *feat_lst, int nelem);


static int
_write_feat_file ( char *fname, mig_lst_t *feat_lst );

static int
_train_scaling ( mig_lst_t *feat_lst, float nsigma );

static int
_train_scaling_whitening ( mig_lst_t *feat_lst );

static void _cleanup ();

/*************************************************************
 *  AUXILIARY FUNCTIONS
 *************************************************************/

static int
_random_reg ( int w, int h, int t,
			 int min_diam_px, int max_diam_px,
			 mig_im_region_t *reg );

/* return 0 if region center is not inside a nodule region */
static int
_possible_nodule ( mig_im_region_t *reg, mig_lst_t *nod_lst );


/* compute scaling params */
static int
_compute_scales ( mig_lst_t *feat_lst, mig_svm_scale_t *scales );

/* apply scaling to all features */
static int
_apply_scale (mig_lst_t *feat_lst, mig_svm_scale_t *scales, float nsigma );

/* read raw segmented data from disk */
static int
_raw_r ( char *name , mig_cad_data_t *cad_data ) ;



/* use data in stack_r and stack_s to replace original
	cad data with whole segmented stack */
static int
_replace_or_w_seg ( mig_cad_data_t *cad_data );

/* check if region is cut by segmentation */
static int
_is_blank_reg ( mig_im_region_t *reg, int radius, float blank_ratio);

#ifdef _DEBUG_WIN_TRAIN_DUMP
static int
_pgm_write ( float *img, int w, int h,  char* fname );
static int
_pgm_write_mig16u ( Mig16u *img, int w, int h,  char* fname );
char maskfname[MAX_PATH];
char maskdir[] = "D:\\MIG\\trabajo\\LUNG_training\\mip_images\\";
char segfname[MAX_PATH];
char segdir[] = "D:\\MIG\\trabajo\\LUNG_training\\segmented\\";
#endif

MIG_C_LINKAGE_END



typedef enum _neg_type_t {
	RANDOM,
	DET_REGS
} neg_type_t;

static float _TargetZRes;
static char *_Input  = NULL; 
static char *_Output = NULL; 
static int _WindowCenter = -600;
static int _WindowWidth = 1500;

static char *_GT_Dir = NULL;
static char *_DetRegs_Dir = NULL;

static mig_cad_data_t _CadData;
static int _VoiLutType = 1;
static int _MaxSliceNumber = 700;

static int _ResampleFlg = 1;


static fpr2_params_feat_t _featparams;


static int mom_orders[] = MOM_ORDERS;

static char *_Output_Prefix = NULL;

static neg_type_t neg_type = DET_REGS;


static EigenWhitener _whitener;
/*******************************************************************/
/* MAIN */
/*******************************************************************/


/**********************************************************************
  WARNING: for now we don't care about final freeing, considering it's
  a standalone program
 **********************************************************************/
int
main ( int argc , char **argv )
{
	int i , rc , num, flag;
	char* lst[MAX_DIR_CONTENTS];
	
	char* feat_fname = NULL;
	char* scale_fname = NULL;

	mig_lst_t *pos_lst = NULL;
	mig_lst_t *neg_lst = NULL;
	mig_lst_t *feat_lst = NULL;

	if ( argc != 6 )
	{
		_usage ();
		exit ( EXIT_FAILURE );
	}

	/* init random seed */
	srand (time(NULL)) ;

	/* process input arguments */
		
	/* desired z resolution */
	errno = 0;
	_TargetZRes = (float) atof (  argv[1] );
	if ( _TargetZRes == 0 && errno != 0 )
	{
		fprintf ( stderr , "\nERROR. %s is not a valid float..." , argv[1] );
		exit ( EXIT_FAILURE );
	}

	/* input */
	_Input  = argv[2];


	/* Ground Truth directory */
	_GT_Dir = argv[3];


	/* Detection regions directory */
	_DetRegs_Dir = argv[4];

	/* output prefix */
	_Output_Prefix = argv[5];

	/* check wether second entry is file list or directory */
	rc = mig_ut_fs_isfile ( _Input , &flag );
	if ( rc != MIG_OK )
	{
		fprintf ( stderr , "\nERROR. mig_ut_fs_isfile ..." );
		fprintf ( stderr , "\nERROR. Reading %s..." , _Input );
		exit ( EXIT_FAILURE );
	}


	/* fill feature extraction info structure */
	_featparams.mip_ratio = MIP_RATIO;
	_featparams.mom_orders = mom_orders;
	_featparams.mom_orders_len = MOM_ORDERS_LEN;
	_featparams.resized_len = RESIZED_LEN;


	/* Moments data structure  creation */
	_featparams.mom_masks = mig_im_mom_get ( _featparams.resized_len ,
		_featparams.mom_orders , _featparams.mom_orders_len );

	if ( _featparams.mom_masks == NULL )
	{
		fprintf ( stderr , " Error filling moment masks..." );
		exit ( EXIT_FAILURE );
	}

	#ifdef _DEBUG_WIN_TRAIN_DUMP
	/* dump masks */
	
	for (i = 0; i!= _featparams.mom_orders_len; ++i )
	{
		sprintf (maskfname, "%sdump_re_%d.pgm", maskdir,i );
		_pgm_write(_featparams.mom_masks->values_re[i],
			_featparams.mom_masks->size,_featparams.mom_masks->size,
			maskfname);
	}
	#endif


	/* initialize feature list */
	feat_lst = mig_lst_alloc();

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
			/*BEGIN PATIENT PROCESSING */

			/* load _CadData for current patient */
			rc = _load_and_resize ( lst[i] );	 
			if ( rc != MIG_OK )
			{
				fprintf ( stderr , "\nERROR. loading and resizing %s..., skipping" , lst[i] );
				/*exit ( EXIT_FAILURE );*/
				_cleanup ();
				continue;
			}
			
			/* replace Original stack with segmented */
			rc = _stack2seg ();	 
			if ( rc != MIG_OK )
			{
				fprintf ( stderr , "\nERROR. Replacing data with segmented stack for %s..., skipping" , lst[i] );
				/*exit ( EXIT_FAILURE );*/
				_cleanup ();
				continue;
			}
			

			/* load GT */
			pos_lst = mig_lst_alloc();
			rc = _load_gt ( pos_lst );
			if ( rc != MIG_OK )
			{
				fprintf ( stderr , "\nERROR. loading gt for %s..., skipping..\n" , lst[i] );
				_cleanup ();
				continue;
			}


			/* create neg list */
			neg_lst = mig_lst_alloc ();
			if ( neg_type == RANDOM )
			{
				rc = _create_ran_list ( neg_lst , pos_lst, NFP_PER_PAT, MIN_DIAM, MAX_DIAM);
				if ( rc != MIG_OK )
				{
					fprintf ( stderr , "\nERROR. creating false random list for %s..." , lst[i] );
					exit ( EXIT_FAILURE );
				}
			}
			else if ( neg_type == DET_REGS )
			{
				rc = _create_det_negs ( neg_lst, pos_lst, MIN_DIAM, MAX_DIAM, DIAM_INC_FACT);
				if ( rc != MIG_OK )
				{
					fprintf ( stderr , "\nERROR. creating false random list for %s..." , lst[i] );
					exit ( EXIT_FAILURE );
				}
			}

			/* obtain features for pos and negs */
			int added_elem;
			added_elem = _add_features ( pos_lst, POS_LABEL, feat_lst, -1 );
			added_elem = _add_features ( neg_lst, NEG_LABEL, feat_lst, MIG_MIN2(added_elem * NEGPOSRATIO,MIN_NEG));

			

			_cleanup ();

		}
		
		/* compute scales, write them on disk, and apply them to train features */
		//rc = _train_scaling ( feat_lst, NSIGMA_WHITE );
		rc = _train_scaling_whitening ( feat_lst );
        if (rc != MIG_OK)
            {
                fprintf ( stderr , "\nERROR. Whitening not performed successfully, abort\n");
                exit(rc);
            }
        printf("\n Data whitened succesfully");
        
		feat_fname = (char*) calloc ( MAX_PATH, sizeof(char));
		snprintf ( feat_fname, MAX_PATH, "%s%s", _Output_Prefix, FEAT_FNAME );
        printf("\nwriting feature file: \n%s",feat_fname);
		rc = _write_feat_file ( feat_fname, feat_lst );
        if (rc == MIG_OK)
		    exit( EXIT_SUCCESS );
        else
        {
            fprintf ( stderr , "\nERROR. Whitening not performed successfully, abort\n");
            exit(rc);
        }

	}

	fprintf ( stderr , "\nERROR. File with directory names needed as input...");
	exit ( EXIT_FAILURE );        
}

/*******************************************************************/
/* PRIVATE */
/*******************************************************************/

static void
_usage ()
{
	printf ( "\nCreate training data from stack (with resizing in z).");
	printf ( "\nDicom stack is read from list." );
	printf ( "\n\nUsage : %s newzres listname GT_dir DetRegs_dir out_fname" , APP_NAME );
	printf ( "\n\t newzres    - target z resolution" );
	printf ( "\n\t listname   - name of input dicom directories list" );
	printf ( "\n\t GT_dir     - Ground Truth directory" );
	printf ( "\n\t DetRegs_dir- detection output directory" );
	printf ( "\n\t out_prefix - feature and scale file name prefix" );
	printf ( "\n" );
	printf ( "\nATTENTION : all paths should be absolute (full)\n" );
}

/*******************************************************************/
static int
_load_and_resize ( const char *entry )
{
	int rc;         /* return code */

	int flag_local;
	/* check if entry exists and if it is a directory */
	rc = mig_ut_fs_isdir ( (char*)entry , &flag_local );
	if ( rc != MIG_OK )
    {
      fprintf ( stderr , "\nERROR. mig_ut_fs_isdir ..." );
      fprintf ( stderr , "\nERROR. Reading %s..." , entry );
      return -1;
    }
	if ( flag_local == 0 )
   {
      fprintf ( stderr , "\nERROR. %s is not a directory..." , entry );
      return -1;
   }
   
	/* first load dicom directory information */
	printf( " Loading dicom info..." );

	snprintf ( _CadData.dicom_data.storage , MAX_PATH , "%s%c" , entry , MIG_PATH_SEPARATOR );

	rc = mig_dcm_get_info (  &( _CadData.dicom_data ) , &( _CadData.stack_s ) );
	if ( rc != MIG_OK )
	{
		printf( " Dicom info loader returned : %d", rc ) ;
		/*if (!_CadData.dicom_data)
		mig_free (_CadData.dicom_data);*/
		return MIG_ERROR_INTERNAL;
	}

	/* load dicom image data */
	printf( " Loading dicom image data..." );

	rc = mig_dcm_rdir_16u (  &( _CadData.stack ) , &( _CadData.dicom_data ) ,
		&( _CadData.stack_s ) , 
		_VoiLutType , 
		_WindowCenter , 
		_WindowWidth , 
		_MaxSliceNumber );
	if ( rc != MIG_OK )
	{
		printf ("Dicom image loader returned : %d" , rc );
		if (!_CadData.stack)
			mig_free ( _CadData.stack );
		return rc;
	}

	/* Resample data if asked to */
	if ( _ResampleFlg == 1 )
	{
		Mig16u *TmpBuffer = NULL;
		mig_size_t TmpSize;

		/* set TmpSize z_res to desired z resolution */
		TmpSize.z_res = _TargetZRes;

		/* resample */
		rc = mig_im_geom_resize_z ( _CadData.stack , &_CadData.stack_s , &TmpBuffer , &TmpSize );
		if ( rc == MIG_OK )
		{
			/* switch stacks */
			mig_free ( _CadData.stack );
			_CadData.stack = TmpBuffer;

			/* copy size information */
			memcpy ( &_CadData.raw_s ,  &_CadData.stack_s , sizeof( mig_size_t ) );
			memcpy ( &_CadData.stack_s , &TmpSize , sizeof( mig_size_t ) );

			/* signal resampled */
			_CadData.resampled = 1;
		}
		else
		{
			/* error resampling */
			printf( "Dicom resampler returned : " ,  rc );

			/* zero out raw_s as it is the same as stack_s */
			memset ( &_CadData.raw_s , 0 , sizeof(mig_size_t) );
			_CadData.resampled = 0;

			return rc;
		}
	}

	return MIG_OK;
}

/*************************************************************************************/

static int
_stack2seg()
{
	int rc;
	char fname[MAX_PATH];
	/* read seg data for left and right lungs */
	snprintf(fname , MAX_PATH, "%s%c%s_%s_%s_seg",
		_DetRegs_Dir, MIG_PATH_SEPARATOR,
		_CadData.dicom_data.patient_id,
		_CadData.dicom_data.study_uid,
		_CadData.dicom_data.series_uid);

	rc = _raw_r( fname, &_CadData);
	if ( rc != MIG_OK )
		return rc;


	/*for each element in Original, put it to 0 if both
		left and right segmented stacks are 0 there */
	rc = _replace_or_w_seg ( &_CadData);
	

	free (_CadData.stack_l);
	free (_CadData.stack_r);

	if ( rc != MIG_OK )
		return rc;

	return MIG_OK;
}

/*************************************************************************************/

static int
_load_gt ( mig_lst_t *nod_lst){
	char fname[MAX_PATH];
	int rc;
	mig_lst_iter iter;
	mig_im_region_t *nod = NULL;

	snprintf(fname , MAX_PATH, "%s%c%s_%s_%s.txt",
		_GT_Dir, MIG_PATH_SEPARATOR,
		_CadData.dicom_data.patient_id,
		_CadData.dicom_data.study_uid,
		_CadData.dicom_data.series_uid);
	rc = mig_tag_read ( fname ,  nod_lst );
	if ( rc != MIG_OK )
		return rc;

	/* iterate through elements and assign a 3d centroid value */
	mig_lst_iter_get ( &iter , nod_lst );
	while ( ( nod = (mig_im_region_t*) mig_lst_iter_next ( &iter ) ) != NULL )
	{
		if ( nod )
			obj3d_compact ( nod );
	}


	return rc;
}


static int
_create_det_negs ( mig_lst_t *nod_lst, mig_lst_t *remove_lst,
				  int min_diam_px, int max_diam_px, float diam_inc_fact){
	char fname[MAX_PATH];
	int rc;
	mig_lst_iter iter;
	mig_im_region_t *nod = NULL;
	
	mig_lst_t *tmp_lst;

	int diam;
	assert (diam_inc_fact > 1);
	
	/* read lists for left and right lungs */
	snprintf(fname , MAX_PATH, "%s%c%s_%s_%s_det_l.txt",
		_DetRegs_Dir, MIG_PATH_SEPARATOR,
		_CadData.dicom_data.patient_id,
		_CadData.dicom_data.study_uid,
		_CadData.dicom_data.series_uid);
	rc = mig_tag_read ( fname ,  nod_lst );
	if ( rc != MIG_OK )
		return rc;

	tmp_lst = mig_lst_alloc();

	snprintf(fname , MAX_PATH, "%s%c%s_%s_%s_det_r.txt",
		_DetRegs_Dir, MIG_PATH_SEPARATOR,
		_CadData.dicom_data.patient_id,
		_CadData.dicom_data.study_uid,
		_CadData.dicom_data.series_uid);
	rc = mig_tag_read ( fname ,  tmp_lst );
	if ( rc != MIG_OK )
		return rc;


	/*join lists */
	mig_lst_cat ( nod_lst, tmp_lst );
	/* iterate through elements, if centroid is not set create regions with different radii */
	mig_lst_iter_get ( &iter , tmp_lst );
	while ( ( nod = (mig_im_region_t*) mig_lst_iter_next ( &iter ) ) != NULL )
	{
		if ( !_possible_nodule ( nod, remove_lst ) )
		{
			if ( nod->radius == 0 ) /*no radius assigned, so create regions*/
			{

				diam = min_diam_px;
				while ( diam <= max_diam_px )
				{
					/*create region*/
					mig_im_region_t* newreg = (mig_im_region_t*) calloc ( 1 , sizeof (mig_im_region_t ));
					newreg->centroid[0] = nod->centroid[0];
					newreg->centroid[1] = nod->centroid[1];
					newreg->centroid[2] = nod->centroid[2];
					newreg->radius = diam /2;
					newreg->size = diam; /*TODO: this is not the convention, but I don't remember the convention..*/

					/* add region to nod_lst */
					mig_lst_put_tail( nod_lst, newreg );
					/* update diam */
					diam *= diam_inc_fact;
				}
			}
			else /* ok diam was set */
			{
				mig_im_region_t* newreg = (mig_im_region_t*) calloc ( 1 , sizeof (mig_im_region_t ));
				newreg->centroid[0] = nod->centroid[0];
				newreg->centroid[1] = nod->centroid[1];
				newreg->centroid[2] = nod->centroid[2];
				newreg->radius = nod->radius;
				newreg->size = nod->size;
				mig_lst_put_tail( nod_lst, newreg );
			}
		}
		
	}

	/*cleanup tmp_lst */
	mig_lst_free ( tmp_lst );


	return rc;
}


static int
_create_ran_list ( mig_lst_t *nod_lst, mig_lst_t *remove_lst,
				  int nelem, int min_diam_px, int max_diam_px)
{
	int ielem = 0;
	int rc;

	mig_im_region_t *reg = NULL;

	while (ielem != nelem)
	{
		reg = (mig_im_region_t*) calloc( 1 , sizeof(mig_im_region_t) );
		if (reg == NULL)
		{
			mig_lst_free ( nod_lst );
			return MIG_ERROR_MEMORY;
		}

		rc = _random_reg( 
			_CadData.stack_s.w, _CadData.stack_s.h, _CadData.stack_s.slices,
			min_diam_px, max_diam_px, reg);
		
		if ( rc != MIG_OK ){
			if ( reg )
				free ( reg );
			fprintf ( stderr, "\nWARNING: _random_reg exited with error" );
			continue;
		}

		if ( !_possible_nodule ( reg, remove_lst ) )
		{
			mig_lst_put_tail ( nod_lst, reg );
			++ielem;

		}
		else
		{
			free ( reg );
		}
	}
	return MIG_OK;
}


static int
_add_features ( mig_lst_t *lst , int label , mig_lst_t *feat_lst, int nelem)
{
	/*********************************************************************
	 * STEPS:
	 * 1. for each region:
	 *		a. pass obj3d to extract_features
	 *		b. fill feat structure (putting label too) and put in feat_lst
	 **********************************************************************/

	//if nelem <0 get all the elements
	//if nelem >0 get nelem
	//if nelem =0 exit

	if ( nelem == 0 )
		return MIG_OK;
	if ( nelem > lst->num)
		return MIG_ERROR_PARAM;

	//TODO: make it random
	int n_skip_elem = 0;
	if (nelem > 0)
		n_skip_elem = lst->num / nelem;

	
	
	
	mig_lst_iter iter;

	feat_t *featstruct;

	mig_im_region_t *reg;
	//iterate through positives (this can be moved outside
	mig_lst_iter_get ( &iter , lst );

	//iterate through nod_lst
	int ielem = 0;
	int added_elem = 0;
	while (  ( reg = (mig_im_region_t*) mig_lst_iter_next ( &iter ) ) != NULL )
	{
		//get rid of unwanted elements
		if (!((n_skip_elem >0) && (ielem % n_skip_elem)))
		{
			//remove if center of region is blank, only for positives: we don't want to add elements cut by segmentation
			if ( (label == 1) && _is_blank_reg ( reg, BLANK_RADIUS, BLANK_RATIO ) )
				continue;

			featstruct = feat_t_alloc ( NDIR, FEAT_LEN );
			if (extract_features ( reg, _CadData.stack,
				_CadData.stack_s.w , _CadData.stack_s.h , _CadData.stack_s.slices ,
				&_featparams, featstruct ) )
			{
				feat_t_free ( featstruct );
			}
			else
			{
				featstruct->label = label;	
				mig_lst_put_tail ( feat_lst, featstruct );
				++added_elem;
			}
		}
		++ielem;
	}
	
	return added_elem;
}

/*********************************************************
LIBSVM feature format, in case, consider other strategies 
The format of training and testing data file is:

<label> <index1>:<value1> <index2>:<value2> ...
.
.
.
**********************************************************/
static int
_write_feat_file ( char* fname, mig_lst_t *feat_lst )
{

	int rc;
	mig_lst_iter iter;

	feat_t *feat;
	
	int idir, ifeat;

	FILE *fp;
	fp = fopen ( fname, "w" );
	if ( fp == NULL )
	{
		return MIG_ERROR_IO;
	}


	//iterate through feat_lst
	mig_lst_iter_get ( &iter , feat_lst );

	
	while ( ( feat = (feat_t*) mig_lst_iter_next ( &iter ) ) != NULL )
	{
		
		for ( idir = 0; idir != feat->ndir; ++idir )
		{
			fprintf ( fp , "%d ", feat->label );
			for ( ifeat = 0; ifeat != feat->feat_len; ++ifeat )
				fprintf ( fp , "%d:%f ", ifeat, feat->feats[idir][ifeat] );
			fprintf ( fp, "\n");
		}

	}
	
	rc = fclose ( fp );
	if ( rc == EOF )
		return MIG_ERROR_IO;

	return MIG_OK;
}


static int
_train_scaling ( mig_lst_t *feat_lst, float nsigma )
{
	int rc;
	mig_svm_scale_t *scales;
	char *scale_fname;
	
	/* peek a feat to get feat_len */
	feat_t *feat = NULL;
	feat = (feat_t*) mig_lst_peek ( feat_lst , 0);
	
	/*obtain computed scales */
	scales = mig_svm_scale_alloc ( feat->feat_len );
	rc = _compute_scales ( feat_lst, scales );

	if (rc != MIG_OK)
	{
		mig_svm_scale_free ( scales );
		return MIG_ERROR_INTERNAL;
	}

	/* save scales to file */
	scale_fname = (char*) calloc ( MAX_PATH, sizeof(char));
	sprintf ( scale_fname, "%s%s", _Output_Prefix, SCALE_FNAME );
	
	rc = mig_svm_scale_params_write ( scale_fname , scales );

	free ( scale_fname );
	if (rc != MIG_OK)
	{
		mig_svm_scale_free ( scales );
		return MIG_ERROR_INTERNAL;
	}

	/* apply scales on the whole set */
	rc = _apply_scale ( feat_lst, scales, nsigma );

	if (rc != MIG_OK)
	{
		mig_svm_scale_free ( scales );
		return MIG_ERROR_INTERNAL;
	}

	/* clear scales */
	mig_svm_scale_free ( scales );
	
	return MIG_OK;
}

static int
_train_scaling_whitening ( mig_lst_t *feat_lst)
{
	int rc;
	char *scale_fname;
	
	/* peek a feat to get feat_len */
	feat_t *feat = NULL;
	feat = (feat_t*) mig_lst_peek ( feat_lst , 0);

	/* create matrix */
	int nfeat = feat->feat_len;
	int nfeatvec = mig_lst_len(feat_lst) * feat->ndir;
	float** feat_mat = (float**) calloc(nfeatvec,sizeof(float*));

	mig_lst_iter iter;
	//iterate through feat_lst
	mig_lst_iter_get ( &iter , feat_lst );

	int row = 0;
	int ifeat;
	while ( ( feat = (feat_t*) mig_lst_iter_next ( &iter ) ) != NULL )
	{
		for ( int idir = 0; idir != feat->ndir; ++idir )
		{
			feat_mat[row] = (float*) calloc(nfeat, sizeof(float));
			for ( ifeat = 0; ifeat != nfeat; ++ifeat )
			{
				feat_mat[row][ifeat] = feat->feats[idir][ifeat];
			
			}
			++row;
		}
	}


	mig_whitening_compute(&_whitener, feat_mat, nfeat, nfeatvec);
	
	/* cleanup feat_mat */
	for (int i = 0; i!=nfeatvec;++i)
	{
		free ( feat_mat[i]);
	}

	free(feat_mat);

	/* save scales to file */
	scale_fname = (char*) calloc ( MAX_PATH, sizeof(char));
	sprintf ( scale_fname, "%s%s", _Output_Prefix, SCALE_FNAME );
	
	rc  = mig_whitening_save(&_whitener, scale_fname );

	free ( scale_fname );
	
	/* apply scales on the whole set */
	mig_lst_iter_reset(&iter);
	
	while ( ( feat = (feat_t*) mig_lst_iter_next ( &iter ) ) != NULL )
	{
		for (int idir = 0; idir != feat->ndir; ++idir)
		{
			//copy to temp
            std::vector<float> tempfeat(feat->feats[idir], feat->feats[idir]+feat->feat_len);
			//replace with witened
			rc = mig_whitening_apply(&_whitener, &tempfeat[0], feat->feats[idir]);
		}
	}
	
	return MIG_OK;
}



static void _cleanup ()
{
	/* clean up */
	if ( _CadData.stack )
		mig_free ( _CadData.stack );

}




/*********************************************************************
 *   ACCESSORY FUNCTIONS
 *********************************************************************/

static int
_random_reg ( int w, int h, int t,
			 int min_diam_px,
			 int max_diam_px,
			 mig_im_region_t *reg )
{
	int x,y,z,diam;
	

	//TODO: now implemented with rand, to be improved

	//generate position and diameter
	x = rand() % w;
	y = rand() % h;
	z = rand() % t;
	diam = min_diam_px + rand() % (max_diam_px - min_diam_px);

	reg->centroid[0] = x;
	reg->centroid[1] = y;
	reg->centroid[2] = z;
	reg->radius = diam / 2;
	reg->size = diam;

	return MIG_OK;
}

/* return 0 if region center is not inside a nodule region */
static int
_possible_nodule ( mig_im_region_t *reg, mig_lst_t *nod_lst )
{
	mig_im_region_t *nod;
	int radius;

	mig_lst_iter iter;
	mig_lst_iter_get ( &iter , nod_lst );
	//iterate through nod_lst

	while ( ( nod = (mig_im_region_t*) mig_lst_iter_next ( &iter ) ) != NULL )
	{	
		radius = nod->radius;
		if (
			(reg->centroid[0] > nod->centroid[0] - radius)
			&& (reg->centroid[0] < nod->centroid[0] + radius)
			&&
			(reg->centroid[1] > nod->centroid[1] - radius)
			&& (reg->centroid[1] < nod->centroid[1] + radius)
			&&
			(reg->centroid[2] > nod->centroid[2] - radius)
			&& (reg->centroid[2] < nod->centroid[2] + radius)
			)
		{
			return 1;
		}
	}

	return 0;			
}




static int
_compute_scales ( mig_lst_t *feat_lst, mig_svm_scale_t *scales )
{
	mig_lst_iter iter;

	feat_t *feat;
	
	int idir, ifeat, i;
	int nfeat;

	int nelem;
	int addelem = 0;
	float invnelem;
	
	float val;

	double *mean;
	double *std;


	mean = (double*) calloc (scales->len, sizeof(double));
	if (mean == NULL)
		return MIG_ERROR_MEMORY;
	std = (double*) calloc (scales->len, sizeof(double));
	if (std == NULL)
	{
		free (mean);
		return MIG_ERROR_MEMORY;
	}

	for ( i = 0; i != scales->len; ++i)
	{
		mean[i] = 0.;
		std[i] = 0.;
	}

	//iterate through feat_lst
	mig_lst_iter_get ( &iter , feat_lst );

	nfeat = scales->len;	

	/*init scales */
	mig_svm_scale_zero ( scales );

	nelem = 0;
	while ( ( feat = (feat_t*) mig_lst_iter_next ( &iter ) ) != NULL )
	{
		for ( idir = 0; idir != feat->ndir; ++idir )
		{
			for ( ifeat = 0; ifeat != nfeat; ++ifeat )
			{
				val = feat->feats[idir][ifeat];
				if ( (int)val != 0 )
				{
					addelem = 1;
					mean[ifeat] += val;
					/* Now holds sum of squares */
					std[ifeat]  += MIG_POW2( val );
					/* TODO ADD ROUNDOFF WORKAROUND FROM NR */
				}
				else
				{
					addelem = 0;
					continue;
				}
			}
			
			nelem += addelem;

		}
	}

	invnelem = 1./nelem;
	for ( ifeat = 0; ifeat != nfeat; ++ifeat )
	{

		mean[ifeat] = (invnelem * mean[ifeat]);
		
		/* THIS IS 1/N, not 1/(N-1)*/
		std[ifeat] = sqrt ( invnelem * std[ifeat] - MIG_POW2( mean[ifeat] ) );


		scales->mean[ifeat] = (float) mean[ifeat];
		scales->std[ifeat] = (float) std[ifeat];
	}
	

	free (mean);
	free (std);
	return MIG_OK;
};




static int
_apply_scale (mig_lst_t *feat_lst, mig_svm_scale_t *scales, float nsigma )
{
	int idir;
	mig_lst_iter iter;
	feat_t *feat;

	//iterate through feat_lst
	mig_lst_iter_get ( &iter , feat_lst );

	while ( ( feat = (feat_t*) mig_lst_iter_next ( &iter ) ) != NULL )
	{
		for ( idir = 0; idir != feat->ndir; ++idir )
		{
			mig_im_scale_whitening_inplace ( feat->feats[idir],
				nsigma, scales->len , scales->mean ,  scales->std );
		}
	}

	return MIG_OK;
}




/*LOCAL VERSION!!!! changed for float!*/
static int
_pgm_write ( float *img, int w, int h,  char* fname )
{
	FILE *fp;
	int i,j;
	float *pimg;
	fp = fopen ( fname, "w" );
	if ( fp == NULL )
	{
		return MIG_ERROR_IO;
	}

	pimg = img;

	fprintf ( fp , "P2\n" );
	fprintf ( fp , "# %s\n", fname);
	fprintf ( fp , "%d %d\n", w, h );
	fprintf ( fp , "%d\n", MIG_MAX_16U );
	for ( i = 0; i != w; ++i )
	{
		for (j = 0; j != h; ++j )
		{
			fprintf ( fp , "%d ", (int)(*pimg++ * MIG_MAX_16S + MIG_MAX_16S));
		}
		fprintf ( fp , "\n");
	}
	fclose ( fp );
	return MIG_OK;
}

static int
_pgm_write_mig16u ( Mig16u *img, int w, int h,  char* fname )
{
	FILE *fp;
	int i,j;
	Mig16u *pimg;
	fp = fopen ( fname, "w" );
	if ( fp == NULL )
	{
		return MIG_ERROR_IO;
	}

	pimg = img;

	fprintf ( fp , "P2\n" );
	fprintf ( fp , "# %s\n", fname);
	fprintf ( fp , "%d %d\n", w, h );
	fprintf ( fp , "%d\n", MIG_MAX_16U );
	for ( i = 0; i != w; ++i )
	{
		for (j = 0; j != h; ++j )
		{
			fprintf ( fp , "%d ", (int)*pimg++);
		}
		fprintf ( fp , "\n");
	}
	fclose ( fp );
	return MIG_OK;
}




/**************************************************/
/* read raw segmented data from disk              */
/* AAA: cad version is slightly different         */
/**************************************************/
static int
_raw_r ( char *name , mig_cad_data_t *cad_data )
{
    int fid = -1 , len = -1, orig_dim = 0;

    cad_data->stack_l = cad_data->stack_r = NULL;

    fid = mig_open ( name , MIG_O_BINARY | MIG_O_RDONLY );
    if ( fid == -1 )
        return MIG_ERROR_IO;
	
	/*check: if stacks dimensions differ,
	  then there is incongruence between loaded stack and saved data */
	
	orig_dim = cad_data->stack_s.dim_stack;

    len = mig_read ( fid , (void*) &(cad_data->stack_s) , sizeof(mig_size_t) );
    if ( len != sizeof(mig_size_t) || (orig_dim != cad_data->stack_s.dim_stack) )
        goto error;

    /* read right stack size */
    len = mig_read ( fid , (void*) &cad_data->stack_r_s , sizeof(mig_size_t) );
    if ( len != sizeof(mig_size_t) )
        goto error;

    /* read right stack bounding box */
    len = mig_read ( fid , (void*) &cad_data->bb[0] , sizeof(mig_roi_t) );
    if ( len != sizeof(mig_roi_t) )
        goto error;

    /* read right stack data */
    cad_data->stack_r = (Mig16u*) mig_malloc ( cad_data->stack_r_s.size_stack );
    if ( cad_data->stack_r == NULL )
        goto error;

    len = mig_read ( fid , (void*) cad_data->stack_r , cad_data->stack_r_s.size_stack );
    if ( len != cad_data->stack_r_s.size_stack )
        goto error;

    /* read left stack size */
    len = mig_read ( fid , (void*) &cad_data->stack_l_s , sizeof(mig_size_t) );
    if ( len != sizeof(mig_size_t) )
        goto error;

    /* read left stack bounding box */
    len = mig_read ( fid , (void*) &cad_data->bb[1] , sizeof(mig_roi_t) );
    if ( len != sizeof(mig_roi_t) )
        goto error;

    cad_data->stack_l = (Mig16u*) mig_malloc ( cad_data->stack_l_s.size_stack );
    if ( cad_data->stack_l == NULL )
        goto error;

    /* read left stack data */
    len = mig_read ( fid , (void*) cad_data->stack_l , cad_data->stack_l_s.size_stack );
    if ( len != cad_data->stack_l_s.size_stack )
        goto error;

    /* close */
    mig_close ( fid );
        return MIG_OK;

error :

    if ( fid > 0 )
        mig_close ( fid );

    if ( cad_data->stack_l )
        mig_free ( cad_data->stack_l );
    cad_data->stack_l = NULL;

    if ( cad_data->stack_r )
        mig_free ( cad_data->stack_r );
    cad_data->stack_r = NULL;

    mig_memz ( &cad_data->stack_l_s , sizeof(mig_size_t) );
    mig_memz ( &cad_data->stack_r_s , sizeof(mig_size_t) );
    mig_memz ( &cad_data->bb[0] , sizeof(mig_roi_t) );
    mig_memz ( &cad_data->bb[1] , sizeof(mig_roi_t) );

    return MIG_ERROR_IO;
}

/* use data in stack_r and stack_s to replace original cad data with whole segmented stack */
static int _replace_or_w_seg ( mig_cad_data_t *cad_data )
{
	int i, j, k, is, js, ks;
	int w,h,sl;
	Mig16u r_val, l_val;

	Mig16u *pStack = cad_data->stack;
	for ( k=0; k != cad_data->stack_s.slices; ++k )
	{
		for ( j = 0; j!= cad_data->stack_s.h; ++j )
		{
			for ( i = 0; i != cad_data->stack_s.w; ++i)
			{
					
				/* right stack bb calculations */
				is = i - cad_data->bb[0].x0;
				js = j - cad_data->bb[0].y0;
				ks = k - cad_data->bb[0].z0;
				
				/* check boundaries and assign value */

				w = cad_data->stack_r_s.w;
				h = cad_data->stack_r_s.h;
				sl = cad_data->stack_r_s.slices;
				if ( is > 0 && js >0 && ks > 0 && 
					is < w && js < h && ks < sl)
				{
					r_val = cad_data->stack_r [ ks * h * w + js * w + is];
				}
				else r_val = 0;

				
				/* left stack bb calculations */
				is = i - cad_data->bb[1].x0;
				js = j - cad_data->bb[1].y0;
				ks = k - cad_data->bb[1].z0;
				
				/* check boundaries and assign value */

				w = cad_data->stack_l_s.w;
				h = cad_data->stack_l_s.h;
				sl = cad_data->stack_l_s.slices;
				if ( is > 0 && js >0 && ks > 0 && 
					is < w && js < h && ks < sl)
				{
					l_val = cad_data->stack_l [ ks * h * w + js * w + is];
				}
				else l_val = 0;

				/* if both values are 0 then put 0 in the stack
					it can be done more efficiently but this is
					way more clear */
				if (r_val == 0 && l_val == 0)
				{
					*pStack = 0;
				}

				/* go ahead in the main stack */
				++pStack;
			}
		}
	}
	/* dump result to be used with breackpoint */
#ifdef _DEBUG_WIN_TRAIN_DUMP
	for ( i = 0; i!= cad_data->stack_s.slices; ++i )
	{
		sprintf (segfname,"%s%s%03d%s",segdir,"seg_",i,".pgm");
		_pgm_write_mig16u(cad_data->stack + i * cad_data->stack_s.h * cad_data->stack_s.w,
			cad_data->stack_s.w,cad_data->stack_s.h, segfname );
	}
#endif
	return MIG_OK;
}

static int _is_blank_reg ( mig_im_region_t *reg, int radius, float blank_ratio)
{
	int x,y,z,i,count_not_blank,dstsize;
	float*  dst;
	
	assert (radius > 0);
	assert ( (blank_ratio >= 0) && (blank_ratio <= 1));

	x = reg->centroid[0];
	y = reg->centroid[1];
	z = reg->centroid[2];
	
	dstsize = MIG_POW3(2 * radius + 1);

	dst = (float*) calloc ( dstsize, sizeof (float) );

	mig_im_bb_cut_3d (_CadData.stack, _CadData.stack_s.w, _CadData.stack_s.h,
						_CadData.stack_s.slices, x , y, z , dst, radius );
	
	count_not_blank = 0;
	for (i = 0; i != dstsize; ++i)
	{
		if ( dst[i] > 0.1 ) //here assuming (float)MIG16U
			++count_not_blank;
	}

	free ( dst );

	if ((((float)count_not_blank) / dstsize) < blank_ratio )
		return 1;
	else
		return 0;
}
