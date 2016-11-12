
/* 
by now only a copy if _classify_obj_3d from libmigfpr_2_zernike_mip.cpp

The idea is to extract common things ad put create a common interface like:
extract_features (obj3d, *_CadData)
this way the trainer should only check GT, load _CadData, create an obj3d and use the function
so that if fpr2 changes, the trainer changes as well

*/


static int 
_classify_obj_3d ( mig_im_region_t *obj3d , int *label )
{
	mig_lst_iter it;

	int rc;

	int idir = 0;
	mig_svm_example_t x;

	int diam_valid , r_valid, diam_valid_z;
	float *cut = NULL , *mipped = NULL , *moments = NULL , *resized = NULL, *whitened = NULL;
	int pos = 0 , neg = 0;

	rc = MIG_OK;

	/* centroid & radius */
	/* crop 3d */
	/* crop cutting radii */
	r_valid = (int) ( 2.0f *( obj3d->radius * 2.0f + 0.5f ) );

	/* crop cutting diameters */
	diam_valid = 2 * r_valid + 1;
	
	diam_valid_z = (int) _zdiam_from_diam((float) diam_valid,
		_CadData->stack_s.h_res, _CadData->stack_s.z_res );

	/* memoty for cut */
	cut = (float*) calloc ( MIG_POW3( diam_valid ), sizeof(float) );

	/* memory for mipped */
	mipped = (float*) calloc ( MIG_POW2( diam_valid ), sizeof(float) );
	if ( mipped == NULL )
		return MIG_ERROR_MEMORY;

	/* memory for resized */
	resized = (float*) calloc ( MIG_POW2( _Fpr2Params.resized_len ), sizeof(float) );
	if ( resized == NULL )
		return MIG_ERROR_MEMORY;

	/* memory for whitened */
	whitened = (float*) calloc ( MIG_POW2( _Fpr2Params.resized_len ), sizeof(float) );
	if ( whitened == NULL )
		return MIG_ERROR_MEMORY;

	/*memory for moments */
	moments = (float*) calloc ( _Fpr2Params.mom_orders_len, sizeof(float) );
	if ( moments == NULL )
		return MIG_ERROR_MEMORY;


	/* cut 3d volume */
	mig_im_bb_cut_3d ( _CadData->stack ,
		_CadData->stack_s.w , _CadData->stack_s.h , _CadData->stack_s.slices ,
                   obj3d->centroid[0] , obj3d->centroid[1] , obj3d->centroid[2] ,
                   cut ,
                   diam_valid / 2);

	for (idir = 0; idir != 3; idir++){
		/* mip */
		mig_im_proj_mip_axes_vol_32f (  cut , mipped , diam_valid ,
			diam_valid, diam_valid, idir, _Fpr2Params.mip_ratio );
		/* resize */
		mig_im_geom_resize ( mipped , diam_valid , diam_valid , resized ,
					_Fpr2Params.resized_len , _Fpr2Params.resized_len , BILINEAR );

		/* moments */
		mig_im_mom_crop_2D ( resized,
					_Fpr2Params.resized_len ,
                    _Fpr2Params.mom_masks , 
                    moments );

		/* TODO: here create REAL whitening  (it gives way better results) */
		mig_im_scale_whitening ( moments, whitened , NSIGMA_WHITE,
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
		memset ( cut     , 0x00 ,  MIG_POW2( diam_valid ) * sizeof(float) );
		memset ( resized , 0x00 ,  MIG_POW2( _Fpr2Params.resized_len ) * sizeof(float) );
		memset ( whitened , 0x00 ,  MIG_POW2( _Fpr2Params.resized_len ) * sizeof(float) );
		memset ( moments , 0x00 , _Fpr2Params.mom_orders_len * sizeof(float) );
	}
	if (pos >= _Fpr2Params.min_pos_labels)
		*label = 1;
	else
		*label = 0;   


	free ( cut );
	free ( mipped );
	free ( resized );
	free ( whitened );
	free ( moments );

	return MIG_OK;

error :

	if ( cut )
		free ( cut );

	if ( mipped )
		free ( mipped );

	if ( resized )
		free ( resized );

	if ( whitened )
		free ( whitened );

	if ( moments )
		free ( moments );

	return MIG_ERROR_MEMORY;     
}