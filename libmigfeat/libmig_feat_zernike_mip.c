#include "libmig_feat_zernike_mip.h"

#define NDIR 3


/*
 ***************************************************************
 *   LOCAL PROTOTYPES
 ***************************************************************
 */
static float
_zdiam_from_diam ( float diam, float xres, float zres);

/***************************************************************/



/*
 ***************************************************************
 *   GLOBAL FUNCTIONS
 ***************************************************************
 */

/* in order to handle mip, directions features is a float** */
int
extract_features ( mig_im_region_t *obj3d, Mig16u* src, int w, int h, int z,
					  fpr2_params_feat_t *featparams, feat_t *featstruct )
{
	int idir = 0;
	int diam_valid , r_valid;
	/*int diam_valid_z;*/
	float *cut = NULL , *mipped = NULL , *resized = NULL;
	
	//TODO: handle, differently
	/*#define _DEBUG_WIN_FEAT_DUMP*/
	#ifdef _DEBUG_WIN_FEAT_DUMP
	static int dumpresized = 1;
	char res_fname[MAX_PATH];
	char res_fnamedir[] = "D:\\MIG\\trabajo\\LUNG_training\\mip_images";
	static int ielem = 0;
	static int nelem = -1;
	#endif

	if ( featstruct == NULL )
	{
		return MIG_ERROR_PARAM;
	}

	/* centroid & radius */
	/* crop 3d */
	/* crop cutting radii */
	r_valid = (int) ( 2.0f *( obj3d->radius * 2.0f + 0.5f ) );

	/* crop cutting diameters */
	diam_valid = 2 * r_valid + 1;
	
	/*diam_valid_z = (int) _zdiam_from_diam((float) diam_valid,
		_CadData->stack_s.h_res, _CadData->stack_s.z_res );*/

	/* memory for cut */
	cut = (float*) calloc ( MIG_POW3( diam_valid ), sizeof(float) );
	if (cut == NULL)
		goto error;

	/* memory for mipped */
	mipped = (float*) calloc ( MIG_POW2( diam_valid ), sizeof(float) );
	if ( mipped == NULL )
		goto error;

	/* memory for resized */
	resized = (float*) calloc ( MIG_POW2( featparams->resized_len ), sizeof(float) );
	if ( resized == NULL )
		goto error;

	/* cut 3d volume */
	/* mig_im_bb_cut_3d ( _CadData->stack ,
		_CadData->stack_s.w , _CadData->stack_s.h , _CadData->stack_s.slices ,
                   obj3d->centroid[0] , obj3d->centroid[1] , obj3d->centroid[2] ,
                   cut ,
                   diam_valid / 2); */

	mig_im_bb_cut_3d ( src , w, h, z,
                   obj3d->centroid[0] , obj3d->centroid[1] , obj3d->centroid[2] ,
                   cut ,
                   diam_valid / 2);

	for (idir = 0; idir != NDIR; idir++){
		/* mip */
		mig_im_proj_mip_axes_vol_32f (  cut , mipped , diam_valid ,
			diam_valid, diam_valid, idir, featparams->mip_ratio );
		/* resize */
		mig_im_geom_resize ( mipped , diam_valid , diam_valid , resized ,
					featparams->resized_len , featparams->resized_len , BILINEAR );


		/* DEBUG: write down the first three images */
		#ifdef _DEBUG_WIN_FEAT_DUMP
		if ( dumpresized )
		{
			sprintf ( res_fname, "%s%s%d%s", res_fnamedir,
				"\\dump_resized_", idir + ielem * NDIR, ".pgm" );
			/*TODO: put in libmigim,  write resized values */
			_pgm_write ( resized, featparams->resized_len,
				featparams->resized_len,  res_fname );
			

			if ( idir == NDIR - 1)
			{
				ielem++;
				if (nelem >0 && ielem == nelem)
					dumpresized = 0;
			}
		}
		#endif

		/* moments */
		mig_im_mom_crop_2D ( resized,
					featparams->resized_len ,
                    featparams->mom_masks , 
					featstruct->feats[idir] );

		memset ( mipped , 0x00 ,  MIG_POW2( diam_valid ) * sizeof(float) );
		memset ( resized , 0x00 ,  MIG_POW2( featparams->resized_len ) * sizeof(float) );
	}

	
	free ( cut );
	free ( mipped );
	free ( resized );
	return MIG_OK;


	error :

	if ( cut )
		free ( cut );

	if ( mipped )
		free ( mipped );

	if ( resized )
		free ( resized );

	return MIG_ERROR_MEMORY;   
}


feat_t*
feat_t_alloc ( int ndir, int nfeats )
{
	int idir, i;
	float **moments;


	feat_t *featstruct;

	featstruct = (feat_t*) calloc ( 1, sizeof(feat_t));

	/*memory for moments */
	moments = (float**) calloc ( ndir, sizeof(float*));
	if ( moments == NULL )
		return NULL;
	
	for ( idir = 0; idir != ndir; idir++)
	{
		moments[idir] = (float*) calloc ( nfeats, sizeof(float) );
		if ( moments[idir] == NULL )
		{
			for ( i = 0; i < idir - 1; ++i )
			{
				free (moments[i]);
			}
			free ( moments );
			return NULL;
		}
	}

	featstruct->ndir = ndir;
	featstruct->feat_len = nfeats;
	featstruct->label = 0;
	featstruct->feats = moments;
	return featstruct;
}

void
feat_t_free ( feat_t* featstruct )
{
	int i;

	for ( i = 0; i < featstruct->ndir; ++i )
	{
		if ( featstruct->feats[i] )
			free ( featstruct->feats[i] );
	}

	free ( featstruct );
}









/* diam is in pixels, x and y res is usually the same, but z may vary, so we multiply
diam for the ratio between xres and zres  NOT USED RIGHT NOW, WE ASSUME PSEUDO CUBIC VOXELS,
due to initial resampling */
static float
_zdiam_from_diam ( float diam, float xres, float zres)
{
	float zdiam;
	float ratio = xres / zres;
	zdiam = diam * ratio;
	return zdiam;
}



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
			fprintf ( fp , "%d ", (int)*pimg++);
		}
		fprintf ( fp , "\n");
	}
	fclose ( fp );
	return MIG_OK;
}




/*
 **********************************************************************************
 *   MATLAB WRAPPER
 **********************************************************************************
 */

#ifdef MATLAB_FEATS


#endif /*MATLAB_FEATS*/


