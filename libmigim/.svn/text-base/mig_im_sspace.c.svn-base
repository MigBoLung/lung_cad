/*
******************************************************************************
*
* Author      : Todor Petkov
* Filename    : mig_im_sspace.c
* Created     : 2007/06/21
* Description : 2D - 3D Scale Space
*
******************************************************************************
*/

#include "mig_im_sspace.h"


/* MATLAB */
//#define MATLAB_SSPACE

//uncomment this for big radii compensation:
//AAA: highly experimental and totally heuristic, 
// not enough checks done
//#define SS_BIG_RADII_COMPENSATION 0.1f

#if defined(MATLAB_SSPACE)

#include "mex.h"
#include "matrix.h"

void
mexFunction ( int nlhs , mxArray *plhs[] ,
              int nrhs , const mxArray *prhs[] );

static void
_swap ( float *Src ,
        float *Dst ,
        int SrcWidth ,
        int SrcHeight );

static void
_cpy_flt2dbl ( float *Src, double *Dst, int len );

static void
_cpy_dbl2flt ( double *Src, float *Dst, int len );

#endif  /* MATLAB */




/*
******************************************************************************
*                       LOCAL PROTOTYPES DECLARATION
******************************************************************************
*/

/*
******************************************************************************
*                       BUILD ARRAY OF LoG KERNELS
*
* Description : This function builds an array of mig_kernel_t LoG kernels
*
* Arguments   : spacing      - how to build sigmas array ( GEOMETRIC or ARITHMETIC progression )
*               sigma_start  - starting sigma ( > 0 )
*               sigma_end    - end sigma ( > sigma_start )
*               sigma_inc    - linear increment between adjacent sigmas
*               num_sigmas   - on return will contain the total number
*                              of LoG kernels
*				type		 - type of scale space analysis 0 3d, 1 2d
*
* Returns     : NULL terminated array of mig_kernel_t on success
*               NULL on error
*
* Notes       : LoG kernels are spaced linearly!
*
******************************************************************************
*/

static mig_kernel_t**
_get_log_kernels ( SigmaSpacing spacing ,
                   float sigma_start ,
                   float sigma_end ,
                   float sigma_inc ,
                   int   *num_sigmas,
				   int type);

/*
******************************************************************************
*                       FREE ARRAY OF LoG KERNELS
*
* Description : This function frees an array of mig_kernel_t LoG kernels
*
* Arguments   : kernels - kernels to free
*
* Returns     :
*
* Notes       : kernels array is freed but pointer is not zeroed !
*
******************************************************************************
*/

static void
_del_log_kernels ( mig_kernel_t **kernels );

/*
******************************************************************************
*                       BUILD WORKING BUFFERS
*
* Description : This function allocates memory for working buffers. Working buffers
*               are the containers for the scale space representation of the input
*               signal as well as the buffers for scale space local extrema search.
*               The dimension of each allocated buffer is window_voxels *
*               sizeof(float). There are a total of num_sigmas buffers.
*
* Arguments   : window_voxels - dimension in voxels of a single scale cube ( window )
*               num_sigmas    - total number of scales ( buffers )
*
* Returns     : Allocated buffers on success
*               NULL on memory error
*
* Notes       : Buffers are accessed as buffer[scale_index][index into pixels of buffer] !
*
******************************************************************************
*/

static float**
_get_buffers ( int window_voxels ,
               int num_sigmas );

/*
******************************************************************************
*                       FREE WORKING BUFFERS
*
* Description : This function frees memory used by working buffers.
*
* Arguments   : buffers - buffer memory to be freed
*
* Returns     :
*
* Notes       : buffers pointer if freed but not zeroed !
*
******************************************************************************
*/

static void
_del_buffers ( float **buffers );

/*
******************************************************************************
*                       FIND MAXIMUM IN 3D BUFFER
*
* Description : This function finds the maximum value and its coordinates inside
*               a 3D buffer.
*
* Arguments   : buffer - input 3D buffer to be searched.
*               w      - buffer width.
*               h      - buffer height.
*               z      - buffer z.
*               xc     - maximum horizontal coordinate (returned).
*               yc     - maximum vertical coordinate (returned).
*               zc     - maximum depth coordinate (returned).
*               val    - maximum value (returned).
*
* Returns     :
*
* Notes       :
*
******************************************************************************
*/

static void
_find_max_3d ( float *buffer ,
               int w ,
               int h ,
               int z ,
               float *xc ,
               float *yc ,
               float *zc ,
               float *val );

/*
******************************************************************************
*                       FIND MAXIMUM IN 2D BUFFER
*
* Description : This function finds the maximum value and its coordinates inside
*               a 2D buffer.
*
* Arguments   : buffer - input 3D buffer to be searched.
*               w      - buffer width.
*               h      - buffer height.
*               xc     - maximum horizontal coordinate (returned).
*               yc     - maximum vertical coordinate (returned).
*               val    - maximum value (returned).
*
* Returns     :
*
* Notes       :
*
******************************************************************************
*/

static void
_find_max_2d ( float *buffer ,
               int w ,
               int h ,
               float *xc ,
               float *yc ,
               float *val );

/*
******************************************************************************
*                       THRESHOLD SCALE SPACE EXTREMA
*
* Description : This function thresholds the scale space extrema buffers :
*               ScaleSpace->extrema using ScaleSpace->threshold.
*
* Arguments   : ScaleSpace - scale space representation
*
* Returns     :
*
* Notes       :
*
******************************************************************************
*/

static void
_sspace_thr ( mig_sspace_t *ScaleSpace );

/*
******************************************************************************
*                       FIND SCALE SPACE EXTREMA
*
* Description : This function locates pixels which are local maxima inside
*               the scale space representation buffers ( ScaleSpace->data ).
*               A 3x3x3 neighbourhood around the center of each scale space
*               data buffer is scanned. For each pixel inside this neighbourhood
*               we look at adjacent 8 connected pixels and see if the our current
*               central pixel is the maximum amongs its neighbours. If so the center
*               pixel value is copies inside the ScaleSpace->extrema[current scale]
*               array.
*
*
* Arguments   : ScaleSpace - scale space representation
*
* Returns     : total number of found extrema.
*
* Notes       : This could be sped up because we make too many comparaisons !
*
******************************************************************************
*/

static int
_sspace_extrema_3d ( mig_sspace_t *ScaleSpace );

/*
******************************************************************************
*                       FIND SCALE SPACE EXTREMA
*
* Description : This function locates pixels which are local maxima inside
*               the scale space representation buffers ( ScaleSpace->data ).
*               A 3x3x3 neighbourhood around the center of each scale space
*               data buffer is scanned. For each pixel inside this neighbourhood
*               we look at adjacent 8 connected pixels and see if the our current
*               central pixel is the maximum amongs its neighbours. If so the center
*               pixel value is copies inside the ScaleSpace->extrema[current scale]
*               array.
*
*
* Arguments   : ScaleSpace - scale space representation
*
* Returns     : total number of found extrema.
*
* Notes       : This could be sped up because we make too many comparaisons !
*
******************************************************************************
*/

static int
_sspace_extrema_2d ( mig_sspace_t *ScaleSpace );

/*
******************************************************************************
*                       BUILD 3D REGION FROM SCALE SPACE DATA
*
* Description : This function scans the ScaleSpace->extrema array and finds the
*               scale and coordinates of the global maximum pixel. To estimate
*               coordinates and radius a 2 degree interpolation is performed
*               between maximum scale pixel and maxima in the scale below and
*               above the global maximum scale.
*
* Arguments   : ScaleSpace - scale space representation
*
* Returns     : 3D region region_t ( to be free using free ) on success.
*               NULL if no region was detected or there was an error.
*
* Notes       : Output Region coordinates are offsets from cube center !
*
******************************************************************************
*/

static mig_im_region_t*
_sspace_reg_3d ( mig_sspace_t *ScaleSpace );


/*
******************************************************************************
*                       BUILD 2D REGION FROM SCALE SPACE DATA
*
* Description : This function scans the ScaleSpace->extrema array and finds the
*               scale and coordinates of the global maximum pixel. To estimate
*               coordinates and radius a 2 degree interpolation is performed
*               between maximum scale pixel and maxima in the scale below and
*               above the global maximum scale.
*
* Arguments   : ScaleSpace - scale space representation
*
* Returns     : 2D region region_t ( to be free using free ) on success.
*               NULL if no region was detected or there was an error.
*
* Notes       : Output Region coordinates are offsets from cube center !
*
******************************************************************************
*/

static mig_im_region_t*
_sspace_reg_2d ( mig_sspace_t *ScaleSpace );

/*
******************************************************************************
*                       BUILD 3D SCALE SPACE REPRESENTATION
*
* Description : This function builds the 3D scale space representation of
*               an input signal which amounts to convolving the input 3D
*               signal with a sequence of 3D LoG filters. Each scale representation
*               is normalized by multiplying its values by -( sigma * sigma ).
*
* Arguments   : SrcSignal - input 3D signal.
*               ScaleSpace - scale space representation.
*
* Returns     : 0 on success.
*               -1 on error.
*
* Notes       : Normalized LoG responses : multiplied by -sigma*sigma !
*
******************************************************************************
*/

static int
_sspace_build ( float *SrcSignal ,
                mig_sspace_t *ScaleSpace );



/*
******************************************************************************
*           BUILD 3D SCALE SPACE REPRESENTATION EVALUATING ONLY THE CENTER OF ROI
*
* Description : This function builds the 3D scale space representation of
*               an input signal which amounts to convolving the input 3D
*               signal with a sequence of 3D LoG filters, but only on the center point of roi.
*               Each scale representation
*               is normalized by multiplying its values by -1.
*
* Arguments   : SrcSignal - input 3D signal.
*               ScaleSpace - scale space representation.
*
* Returns     : 0 on success.
*               -1 on error.
*
* Notes       : Normalized LoG responses : multiplied by -1 !
*
******************************************************************************
*/

static int
_sspace_build_oncenter ( float *SrcSignal ,
                mig_sspace_t *ScaleSpace );


/*
******************************************************************************
*               GLOBAL PROTOTYPES IMPLEMENTATION
******************************************************************************
*/

mig_sspace_t*
mig_im_sspace_get ( int SSpaceType ,
                    SigmaSpacing spacing ,
                    float sigma_start , float sigma_end , float sigma_inc ,
                    float threshold )
{
        mig_sspace_t *ScaleSpace = NULL;

        ScaleSpace = (mig_sspace_t*) calloc ( 1 , sizeof( mig_sspace_t ) );
        if ( ScaleSpace == NULL )
                return NULL;

        /* build up LoG kernels array*/
        ScaleSpace->kernels =
                _get_log_kernels ( spacing , sigma_start , sigma_end , sigma_inc ,
                                   &( ScaleSpace->num_sigmas ), ScaleSpace->type );
        if ( ScaleSpace->kernels == NULL )
        {
                free ( ScaleSpace );
                return NULL;
        }

        ScaleSpace->type = SSpaceType;
		ScaleSpace->window_radius = (int)( 3.0f * sigma_end +  1.5f );
        ScaleSpace->window_len    = 2 * ScaleSpace->window_radius + 1;
        ScaleSpace->window_voxels = (SSpaceType == 0 ) ? \
            MIG_POW3( ScaleSpace->window_len ) : MIG_POW2( ScaleSpace->window_len );

        ScaleSpace->sigma_start = sigma_start;
        ScaleSpace->sigma_end   = sigma_end;
        ScaleSpace->sigma_inc   = sigma_inc;
        ScaleSpace->threshold   = threshold;
		
		
		
		if ( ScaleSpace->type == 0)/* 3d boiled down */
		{
			/* allocate memory for scale space representation */
			ScaleSpace->data = _get_buffers ( 1 ,
                                          ScaleSpace->num_sigmas );
			if ( ScaleSpace->data == NULL )
			{
                _del_log_kernels ( ScaleSpace->kernels );
                free ( ScaleSpace );
                return NULL;
			}
			
			
			/* allocate memory for scale space extrema JUST FOR COMPATIBILITY */
			//ScaleSpace->extrema = _get_buffers ( 1 ,
            //                                 ScaleSpace->num_sigmas );
			//if ( ScaleSpace->extrema == NULL )
			//{
            //    _del_log_kernels ( ScaleSpace->kernels );
            //    _del_buffers     ( ScaleSpace->data );
            //    free ( ScaleSpace );
            //    return NULL;
			//}
		}
		else
		{
			/* allocate memory for scale space representation */
			ScaleSpace->data = _get_buffers ( ScaleSpace->window_voxels ,
                                          ScaleSpace->num_sigmas );
			if ( ScaleSpace->data == NULL )
			{
			    _del_log_kernels ( ScaleSpace->kernels );
                free ( ScaleSpace );
                return NULL;
			}
		
			/* allocate memory for scale space extrema */
			ScaleSpace->extrema = _get_buffers ( ScaleSpace->window_voxels ,
                                             ScaleSpace->num_sigmas );
			if ( ScaleSpace->extrema == NULL )
			{
                _del_log_kernels ( ScaleSpace->kernels );
                _del_buffers     ( ScaleSpace->data );
                free ( ScaleSpace );
                return NULL;
			}

		}
        return ScaleSpace;
}

/*****************************************************************************/

mig_im_region_t*
mig_im_sspace ( float *Input , mig_sspace_t *ScaleSpace )
{
        int rc = 0;

        if ( Input == NULL || ScaleSpace == NULL )
                return NULL;

        /* build scale space representation for input signal */
        rc = _sspace_build ( Input , ScaleSpace );
        if ( rc != 0 )
                return NULL;

        /* find scale space extrema */
        if ( ScaleSpace->type == 0 )
            rc = _sspace_extrema_3d ( ScaleSpace );
        else
            rc = _sspace_extrema_2d ( ScaleSpace );

        /* if there are no local extrema or
           inside the scale space representation
           return NULL */
        if ( rc == 0 )
                return NULL;

        /* threshold scale space extrema */
        _sspace_thr ( ScaleSpace );

        if ( ScaleSpace->type == 0 )
            return _sspace_reg_3d ( ScaleSpace );
        else
            return _sspace_reg_2d ( ScaleSpace );
}


/*****************************************************************************/

void
mig_im_sspace_del ( mig_sspace_t *ScaleSpace )
{
        if ( ScaleSpace == NULL )
                return;

        _del_log_kernels ( ScaleSpace->kernels );
        _del_buffers     ( ScaleSpace->data );
        _del_buffers     ( ScaleSpace->extrema );
        free ( ScaleSpace );
}



mig_im_region_t*
mig_im_sspace_radius ( float *Input , mig_sspace_t *ScaleSpace )
{
        int rc = 0;

		int scale = 0;

		float response;



		float weighted_scale,sum_weights;

		mig_im_region_t *Region;

        if ( Input == NULL || ScaleSpace == NULL )
                return NULL;

		/* 2D, not yet implemented */
		if (ScaleSpace->type == 1)
		{
			return NULL;
		}


        /* build scale space representation for input signal */
		
        rc = _sspace_build_oncenter ( Input , ScaleSpace );
        if ( rc != 0 )
                return NULL;
		
		/* find radius */
		/* weighted sum of square responses, with threshold*/ 
		weighted_scale = 0.0f;
		sum_weights = 0.0f;

		for ( scale = 0 ; scale < ScaleSpace->num_sigmas ; ++scale )
        {
			response = ScaleSpace->data[scale][0];
			/*consider only positive responses higher then set threshold*/
			if ( response > MIG_MAX2(ScaleSpace->threshold,0) )
			{
				weighted_scale += response * ScaleSpace->kernels[scale]->scale;
				sum_weights += response;
			}
		}
		if (sum_weights > 0.0f)
			weighted_scale /= sum_weights;


		/* create new reg */
		/*send this check to FPR-1  and substitude with the commented one*/
		/*if (weighted_scale > ScaleSpace->kernels[0]->scale - 0.001)*/
		if (weighted_scale > ScaleSpace->kernels[1]->scale - 0.001 &&
			weighted_scale < ScaleSpace->kernels[ScaleSpace->num_sigmas - 2]->scale + 0.001)/*scalemax < ScaleSpace->num_sigmas - 1)*/
		{
			 Region = (mig_im_region_t*)
                calloc ( 1 , sizeof( mig_im_region_t ) );
			 if ( Region == NULL )
                return NULL;
			Region->centroid[0] = 0.0f;
			Region->centroid[1] = 0.0f;
			Region->centroid[2] = 0.0f;
			/*Region->radius    = 0.5f * ( ( ScaleSpace->kernels[scalemax]->scale ));*/
			#if defined(SS_BIG_RADII_COMPENSATION)
			/* big radii compensation: big radii are usually a little underestimated */
			/* the reason may reside in more detail in the nodule surface, giving lower responses */
			Region->radius    =
				( 0.5f + SS_BIG_RADII_COMPENSATION * sqrt(
					( weighted_scale - ScaleSpace->kernels[0]->scale )/
					( ScaleSpace->kernels[ScaleSpace->num_sigmas - 1]->scale - ScaleSpace->kernels[0]->scale)
					)
				)
				* ( weighted_scale );
			#else
			Region->radius    = 0.5f * ( weighted_scale );
			#endif

			Region->size      = (int) ( (4.0f/3.0f) * MIG_PI * MIG_POW3( Region->radius ) );

			return Region;
		}
		else
		{
			return NULL;
		}

}


/*
******************************************************************************
*               LOCAL PROTOTYPES IMPLEMENTATION
******************************************************************************
*/

#define SS_K    1.148698f

static mig_kernel_t**
_get_log_kernels ( SigmaSpacing spacing ,
                   float sigma_start ,
                   float sigma_end ,
                   float sigma_inc ,
                   int   *num_sigmas,
				   int type)
{
        mig_kernel_t **KernelList = NULL;
        float sigma_curr = sigma_start;
        float tmp;
        int i;

        /* arithmetic spacing between adjacent sigmas */

        if ( spacing == ARITHMETIC )
        {
                *num_sigmas = (int)( ( ( sigma_end - sigma_start ) / sigma_inc ) + 1.0f );

				/* +1 because of null handling when deallocating */
                KernelList = (mig_kernel_t**)
                        calloc ( *num_sigmas + 1 , sizeof( mig_kernel_t* ) );
                if ( KernelList == NULL )
                        return NULL;

                /* we calculate number of sigmas again */
                i = 0;
                /**num_sigmas = 0;*/

                /* start populating KernelList */
                while ( i < *num_sigmas -1)
				{
						assert ( sigma_curr < sigma_end );
                
						if (type == 1)
							KernelList[i] = mig_im_kernel_get_log_2d ( sigma_curr );
						else if (type == 0)
							KernelList[i] = mig_im_kernel_get_log_3d ( sigma_curr );
						else
							goto error;
                        
                        if ( KernelList[i] == NULL )
                                goto error;

                        ++i ;
                        sigma_curr += sigma_inc;
                        /**num_sigmas = *num_sigmas + 1;*/
                }
        }
        /* geometric spacing between adjacent sigmas */
        else
        {
                tmp = logf( sigma_end / sigma_start );
                tmp /= logf(SS_K);
                *num_sigmas = (int)( tmp + 1.0f );

				/* +1 because of null handling when deallocating */
                KernelList = (mig_kernel_t**)
                        calloc ( *num_sigmas + 1 , sizeof( mig_kernel_t* ) );
                if ( KernelList == NULL )
                        return NULL;

                /* we DON'T calculate number of sigmas again */
                i = 0;
                /* *num_sigmas = 0;*/

                /* start populating KernelList */
                while ( i < *num_sigmas -1)
                {
						assert (sigma_curr < sigma_end);

						if (type == 1)
							KernelList[i] = mig_im_kernel_get_log_2d ( sigma_curr );
						else if (type == 0)
							KernelList[i] = mig_im_kernel_get_log_3d ( sigma_curr );
						else
							goto error;

                        if ( KernelList[i] == NULL )
                                goto error;

                        ++i ;
                        sigma_curr *= SS_K;
                        /* *num_sigmas = *num_sigmas + 1; */
                }
        }
		
        /* last element is sigma_end */
		if (type == 1)
			KernelList[*num_sigmas-1] = mig_im_kernel_get_log_2d ( sigma_end );
		else if (type == 0)
			KernelList[*num_sigmas-1] = mig_im_kernel_get_log_3d ( sigma_end );
		else
			goto error;
        if ( KernelList[*num_sigmas-1] == NULL )
            goto error;

		/* this should keep things portable */
		KernelList[*num_sigmas] = NULL;

        return KernelList;

error :

        _del_log_kernels ( KernelList );
        return NULL;
}

/*****************************************************************************/

static void
_del_log_kernels ( mig_kernel_t **kernels )
{
    int i;

    if ( kernels == NULL )
        return;
    
    for ( i = 0 ; ; ++i )
    {
        if ( kernels[i] == NULL )
			break;
		mig_im_kernel_delete ( kernels[i] );

    }

    free ( kernels );
}

/*****************************************************************************/

static float**
_get_buffers ( int window_voxels ,
               int num_sigmas )
{
        float **Buffers = NULL;
        int i;
		
		/* +1 because of null handling when deallocating */
        Buffers = (float**)
                calloc ( num_sigmas +1 , sizeof( float* ) );
        if ( Buffers == NULL )
                return NULL;

        /* start allocating buffers */
        for ( i = 0 ; i < num_sigmas ; ++i )
        {
                Buffers[i] = malloc ( window_voxels * sizeof( float ) );
                if ( Buffers[i] == NULL )
                        goto error;
        }
		/* GF: this keeps things portable */
		Buffers [num_sigmas] = NULL;

        return Buffers;

error :

        _del_buffers ( Buffers );
        return NULL;
}

/*****************************************************************************/

static void
_del_buffers ( float **buffers )
{
        float **idx = buffers;

        if ( buffers == NULL )
                return;

        do
        {
                free ( *idx );
        } while ( *idx++ );

        free ( buffers );
}

/*****************************************************************************/

static void
_find_max_3d ( float *buffer ,
               int w , int h , int z ,
               float *xc , float *yc , float *zc ,
               float *val )
{
    int i , j , k;
    int num = 0;

    *val = 0.0f;
    *xc = *yc = *zc = 0.0f;

    for ( k = 0 ; k < z ; ++k )
    {
        for ( j = 0 ; j < h ; ++j )
        {
            for ( i = 0 ; i < w ; ++i , ++buffer )
            {
                if ( *buffer > *val )
                {
                    *val = *buffer;
                    *xc  = (float)i;
                    *yc  = (float)j;
                    *zc  = (float)k;
                    num = 1;
                }
                else
                {
                    if ( *buffer == *val )
                    {
                        *xc += (float)i;
                        *yc += (float)j;
                        *zc += (float)k;
                        num ++;
                    }
                }
            }
        }
    }

    *xc /= num;
    *yc /= num;
    *zc /= num;
}

/*****************************************************************************/

static void
_find_max_2d ( float *buffer ,
               int w , int h ,
               float *xc , float *yc ,
               float *val )
{
    int i , j;
    int num = 0;

    *val = 0.0f;
    *xc = *yc = 0.0f;

    for ( j = 0 ; j < h ; ++j )
    {
        for ( i = 0 ; i < w ; ++i , ++buffer )
        {
            if ( *buffer > *val )
            {
                *val = *buffer;
                *xc  = (float)i;
                *yc  = (float)j;
                num = 1;
            }
            else
            {
                if ( *buffer == *val )
                {
                    *xc += (float)i;
                    *yc += (float)j;
                    num ++;
                }
            }
        }
    }

    *xc /= num;
    *yc /= num;
}

/*****************************************************************************/

int
_sspace_build ( float *SrcSignal , mig_sspace_t *ScaleSpace )
{
    int scale , i , rc;
	
	/*DEBUG*************************************
	int l, m;

	
	FILE *f_in_im = fopen("D:\\trabajo\\input_img","w");
	FILE *f_sigma1_im = fopen("D:\\trabajo\\sigma1_img","w");
	
	float *tmp_float_p;
	
	tmp_float_p = SrcSignal;
	for (l = 0; l<ScaleSpace->window_len; l++){
		for ( m = 0; m<ScaleSpace->window_len; m++ ) {
			fprintf ( f_in_im, "%f ", *tmp_float_p++);
		}
		fprintf ( f_in_im, "\n" );
	}
	****************************************/

    if ( ScaleSpace->type == 0 )    /* 3D scale space */
    {
        for ( scale = 0 ; scale < ScaleSpace->num_sigmas ; ++scale )
        {
            rc = mig_im_log_3d_sep ( SrcSignal ,
                    ScaleSpace->data[scale] ,
                    ScaleSpace->window_len ,
                    ScaleSpace->window_len ,
                    ScaleSpace->window_len ,
                    ScaleSpace->kernels[scale] );
            if ( rc != 0 )
                return -1;

            for ( i = 0 ; i < ScaleSpace->window_voxels ; ++i )
                ScaleSpace->data[scale][i] *=
                    -MIG_POW2( ScaleSpace->kernels[scale]->sigma );
        }

        return 0;
    }
	
    /* 2D scale space */
    for ( scale = 0 ; scale < ScaleSpace->num_sigmas ; ++scale )
    {
        rc = mig_im_log_2d_full ( SrcSignal ,
                    ScaleSpace->data[scale] ,
                    ScaleSpace->window_len ,
                    ScaleSpace->window_len ,
                    ScaleSpace->kernels[scale] );
        if ( rc != 0 )
            return -1;

        for ( i = 0 ; i < ScaleSpace->window_voxels ; ++i )
            ScaleSpace->data[scale][i] *=
                -MIG_POW2( ScaleSpace->kernels[scale]->sigma );
    }
	
	/*DEBUG**********************************
	tmp_float_p = ScaleSpace->data[0];
	for (l = 0; l<ScaleSpace->window_len; l++){
		for ( m = 0; m<ScaleSpace->window_len; m++ ) {
			fprintf ( f_sigma1_im, "%f ", *tmp_float_p++);
		}
		fprintf ( f_sigma1_im, "\n" );
	}
	
	fclose(f_in_im);
	fclose(f_sigma1_im);
	/************************************/


    return 0;    
}

/*****************************************************************************/

static int
_sspace_build_oncenter ( float *SrcSignal , mig_sspace_t *ScaleSpace )
{
    int scale ,  rc;
	
    if ( ScaleSpace->type == 0 )    /* 3D scale space */
    {
        for ( scale = 0 ; scale < ScaleSpace->num_sigmas ; ++scale )
        {

			/* here compute only on the center voxel */
            rc = mig_im_log_3d_center ( SrcSignal ,
                    ScaleSpace->data[scale] ,
                    ScaleSpace->window_len ,
                    ScaleSpace->window_len ,
                    ScaleSpace->window_len ,
                    ScaleSpace->kernels[scale] );
            if ( rc != 0 )
                return -1;
			
			/*why this? */
            //ScaleSpace->data[scale][0] *=
            //        -MIG_POW2( ScaleSpace->kernels[scale]->sigma );

			/* just change sign */
			ScaleSpace->data[scale][0] *= -1.0;
        }

        return 0;
    }
	

	

    /* 2D scale space */
	/* TODO:not implemented yet */
	return -1;
}


/*****************************************************************************/

static void
_sspace_thr ( mig_sspace_t *ScaleSpace )
{
        int i;
        float **idx;

        idx = ScaleSpace->extrema;

        for ( i = 0 ; i < ScaleSpace->num_sigmas ; ++i , ++idx )
        {
            mig_im_thr_32f_i_val ( *idx , ScaleSpace->window_voxels , ScaleSpace->threshold );
        }
}

/*****************************************************************************/

static int
_sspace_extrema_3d ( mig_sspace_t *ScaleSpace )
{
    int scale;
    int i , j , k;
    int l , m , n;
    int num_extrema = 0 , r , d;
    float max;
    float *data;
    float *extrema;
    float *curr;

    r = ScaleSpace->window_radius;
    d = ScaleSpace->window_len;

    for ( scale = 0 ; scale < ScaleSpace->num_sigmas ; ++scale , ++data )
    {
        data    = ScaleSpace->data[scale];
        extrema = ScaleSpace->extrema[scale];

        /* zero out extrema buffer */
        memset ( extrema , 0x00 , ScaleSpace->window_voxels * sizeof(float) );
    
        /* test 25 pixels around cube center to see if they are local maxima */
        for ( k = -1 ; k <= 1 ; ++k )
        {
            for ( j = -1 ; j <= 1 ; ++j )
            {
                for ( i = -1 ; i <= 1 ; ++i )
                {
                    /* current pixel inside cube */
                    curr = data + ( r + i ) + ( r + j ) * d + ( r + k ) * d * d;
                    max = *curr;
                    
                    /* test a 3x3x3 neighbourhood around current pixel */
                    for ( n = -1 ; n <= 1 ; ++n )
                    {
                        for ( m = -1 ; m <= 1 ; ++m )
                        {
                            for ( l = -1 ; l <= 1 ; ++l )
                            {
                                if ( curr[l+m*d+n*d*d] > max )
                                {
                                    max = curr[l+m*d+n*d*d];
                                    goto out;
                                }
                            } /* l */
                        } /* m */
                    } /* n */

                    out :

                    /* if the current pixel is the maximum save its value inside output buffer */
                    if ( ( *curr == max ) && ( max != 0.0f ) )
                    {
                        extrema[(r+i)+(r+j)*d+(k+r)*d*d] = max;
                        ++ num_extrema;
                    }
                } /* i */
            } /* j */
        } /* k */

    } /* scale */

    return num_extrema;
}

/*****************************************************************************/

static int
_sspace_extrema_2d ( mig_sspace_t *ScaleSpace )
{
    int scale;
    int i , j;
    int l , m;
    int num_extrema = 0 , r , d;
    float max;
    float *data;
    float *extrema;
    float *curr;

    r = ScaleSpace->window_radius;
    d = ScaleSpace->window_len;

    for ( scale = 0 ; scale < ScaleSpace->num_sigmas ; ++scale , ++data )
    {
        data    = ScaleSpace->data[scale];
        extrema = ScaleSpace->extrema[scale];

        /* zero out extrema buffer */
        memset ( extrema , 0x00 , ScaleSpace->window_voxels * sizeof(float) );
    
        /* test 8 pixels around cube center and
           cube center to see if they are local maxima
        */
        for ( j = -1 ; j <= 1 ; ++j )
        {
            for ( i = -1 ; i <= 1 ; ++i )
            {
                /* current pixel inside cube */
                curr = data + ( r + i ) + ( r + j ) * d;
                max = *curr;
                    
                /* test a 3x3x3 neighbourhood around current pixel */
                for ( m = -1 ; m <= 1 ; ++m )
                {
                    for ( l = -1 ; l <= 1 ; ++l )
                    {
                        if ( curr[l+m*d] > max )
                        {
                            max = curr[l+m*d];
                            goto out;
                        }
                    } /* l */
                } /* m */

                out :

                /* if the current pixel is the maximum save its value inside output buffer */
                if ( ( *curr == max ) && ( max != 0.0f ) )
                {
                    extrema[(r+i)+(r+j)*d] = max;
                    ++ num_extrema;
                }
            } /* i */
        } /* j */
    } /* scale */

    return num_extrema;
}

/*****************************************************************************/

static mig_im_region_t*
_sspace_reg_3d ( mig_sspace_t *ScaleSpace )
{
        mig_im_region_t *Region;
        int scale_idx;
        float **extrema_idx;

        int   scalemax = 0;
        float xmax = 0.0f , ymax = 0.0f , zmax = 0.0f , max = 0.0f;
        float x1 = 0.0f , y1 = 0.0f , z1 = 0.0f , val1 = 0.0f;
        float x2 = 0.0f , y2 = 0.0f , z2 = 0.0f , val2 = 0.0f;

        Region = (mig_im_region_t*)
                calloc ( 1 , sizeof( mig_im_region_t ) );
        if ( Region == NULL )
                return NULL;

        /* find global maximum and its coordinates
         * inside scale space local maxima
         */
        extrema_idx = ScaleSpace->extrema;
        for ( scale_idx = 0 ; scale_idx < ScaleSpace->num_sigmas ; ++ scale_idx )
        {
                _find_max_3d ( extrema_idx[scale_idx] ,
                               ScaleSpace->window_len ,
                               ScaleSpace->window_len ,
                               ScaleSpace->window_len ,
                               &x1 , &y1 , &z1 , &val1 );

                if ( val1 > max )
                {
                        max      = val1;
                        xmax     = x1;
                        ymax     = y1;
                        zmax     = z1;
                        scalemax = scale_idx;
                }
        }

        if ( max <= 0.0f )
        {
                free ( Region );
                return NULL;
        }

        /* if we are working with only one sigma
         * thre is little else to do : pack
         * results and return
         */
        if ( ScaleSpace->num_sigmas == 1 )
        {
                Region->centroid[0] = xmax;
                Region->centroid[1] = ymax;
                Region->centroid[2] = zmax;
                Region->radius    = ScaleSpace->kernels[0]->scale;
                Region->size      = (int) ( (4.0f/3.0f) * MIG_PI * MIG_POW3( Region->radius ) );
                return Region;
        }

        /* if we have more than one sigma in our
         * scale space try cubic interpolation
         * to find approximate region radius.
         * Region centroid coordinates are calculated
         * as mean value
         */

        /* case where maxium has been found for first sigma */
        if ( scalemax == 0 )
        {
                /* find values inside next sigma : sigma with index 2 */
                _find_max_3d ( extrema_idx[1] ,
                               ScaleSpace->window_len ,
                               ScaleSpace->window_len ,
                               ScaleSpace->window_len ,
                               &x2 , &y2 , &z2 , &val2 );

                x1 = xmax;
                y1 = ymax;
                z1 = zmax;
                val1 = val2;
        }
        else
        {
                /* case where maxium has been found for last sigma */
                if ( scalemax == ScaleSpace->num_sigmas - 1 )
                {
                        x1 = y1 = z1 = val1 = 0.0f;

                        /* find values inside next sigma : sigma with index num_signmas - 2 */
                        _find_max_3d ( extrema_idx[ScaleSpace->num_sigmas-2] ,
                                       ScaleSpace->window_len ,
                                       ScaleSpace->window_len ,
                                       ScaleSpace->window_len ,
                                       &x1 , &y1 , &z1 , &val1 );

                        x2 = xmax;
                        y2 = ymax;
                        z2 = zmax;
                        val2 = val1;
                }

                /* case where maxium has been found any other sigma */
                else
                {
                        /* previous sigma cube */
                        _find_max_3d ( extrema_idx[scalemax-1] ,
                                       ScaleSpace->window_len ,
                                       ScaleSpace->window_len ,
                                       ScaleSpace->window_len ,
                                       &x1 , &y1 , &z1 , &val1 );

                        /* next sigma cube */
                        _find_max_3d ( extrema_idx[scalemax+1] ,
                                       ScaleSpace->window_len ,
                                       ScaleSpace->window_len ,
                                       ScaleSpace->window_len ,
                                       &x2 , &y2 , &z2 , &val2 );
                }
        }

        /* pack results */

        Region->centroid[0] = ( xmax + x1 + x2 ) / 3.0f;
        Region->centroid[1] = ( ymax + y1 + y2 ) / 3.0f;
        Region->centroid[2] = ( zmax + z1 + z2 ) / 3.0f;
        Region->radius    = 0.5f * ( ( ScaleSpace->kernels[scalemax]->scale ) +
                            0.5f * ( ( val1 - val2 ) / ( val1 + val2 - 2.0f * max ) ) );
        Region->size      = (int) ( (4.0f/3.0f) * MIG_PI * MIG_POW3( Region->radius ) );

        /* code cordinates as offsets from scale space
         * cube center
         */
        Region->centroid[0] -= ScaleSpace->window_radius;
        Region->centroid[1] -= ScaleSpace->window_radius;
        Region->centroid[2] -= ScaleSpace->window_radius;

        return Region;
}

/*****************************************************************************/

static mig_im_region_t*
_sspace_reg_2d ( mig_sspace_t *ScaleSpace )
{
        mig_im_region_t *Region;
        int scale_idx;
        float **extrema_idx;

        int   scalemax = 0;
        float xmax = 0.0f , ymax = 0.0f , max = 0.0f;
        float x1 = 0.0f , y1 = 0.0f , val1 = 0.0f;
        float x2 = 0.0f , y2 = 0.0f , val2 = 0.0f;

        Region = (mig_im_region_t*)
                calloc ( 1 , sizeof( mig_im_region_t ) );
        if ( Region == NULL )
                return NULL;

        /* find global maximum and its coordinates
         * inside scale space local maxima
         */
        extrema_idx = ScaleSpace->extrema;
        for ( scale_idx = 0 ; scale_idx < ScaleSpace->num_sigmas ; ++ scale_idx )
        {
                _find_max_2d ( extrema_idx[scale_idx] ,
                               ScaleSpace->window_len ,
                               ScaleSpace->window_len ,
                               &x1 , &y1 , &val1 );

                if ( val1 > max )
                {
                        max      = val1;
                        xmax     = x1;
                        ymax     = y1;
                        scalemax = scale_idx;
                }
        }

        if ( max <= 0.0f )
        {
                free ( Region );
                return NULL;
        }

        /* if we are working with only one sigma
         * thre is little else to do : pack
         * results and return
         */
        if ( ScaleSpace->num_sigmas == 1 )
        {
                Region->centroid[0] = xmax;
                Region->centroid[1] = ymax;
                Region->radius    = ScaleSpace->kernels[0]->scale;
                Region->size      = (int) ( MIG_PI * MIG_POW2( Region->radius ) );
                return Region;
        }

        /* if we have more than one sigma in our
         * scale space try cubic interpolation
         * to find approximate region radius.
         * Region centroid coordinates are calculated
         * as mean value
         */

        /* case where maxium has been found for first sigma */
        if ( scalemax == 0 )
        {
                /* find values inside next sigma : sigma with index 2 */
                _find_max_2d ( extrema_idx[1] ,
                               ScaleSpace->window_len ,
                               ScaleSpace->window_len ,
                               &x2 , &y2 , &val2 );

                x1 = xmax;
                y1 = ymax;
                val1 = val2;
        }
        else
        {
                /* case where maxium has been found for last sigma */
                if ( scalemax == ScaleSpace->num_sigmas - 1 )
                {
                        x1 = y1 = val1 = 0.0f;

                        /* find values inside next sigma : sigma with index num_signmas - 2 */
                        _find_max_2d ( extrema_idx[ScaleSpace->num_sigmas-2] ,
                                       ScaleSpace->window_len ,
                                       ScaleSpace->window_len ,
                                       &x1 , &y1 , &val1 );

                        x2 = xmax;
                        y2 = ymax;
                        val2 = val1;
                }

                /* case where maxium has been found any other sigma */
                else
                {
                        /* previous sigma cube */
                        _find_max_2d ( extrema_idx[scalemax-1] ,
                                       ScaleSpace->window_len ,
                                       ScaleSpace->window_len ,
                                       &x1 , &y1 , &val1 );

                        /* next sigma cube */
                        _find_max_2d ( extrema_idx[scalemax+1] ,
                                       ScaleSpace->window_len ,
                                       ScaleSpace->window_len ,
                                       &x2 , &y2 , &val2 );
                }
        }

        /* pack results */

        Region->centroid[0] = ( xmax + x1 + x2 ) / 3.0f;
        Region->centroid[1] = ( ymax + y1 + y2 ) / 3.0f;
        Region->radius    = 0.5f * ( ( ScaleSpace->kernels[scalemax]->scale ) +
                            0.5f * ( ( val1 - val2 ) / ( val1 + val2 - 2.0f * max ) ) );
        Region->size      = (int) ( MIG_PI * MIG_POW2( Region->radius ) );

        /* code cordinates as offsets from scale space cube center */
        Region->centroid[0] -= ScaleSpace->window_radius;
        Region->centroid[1] -= ScaleSpace->window_radius;

        return Region;
}

/****************************************************/
/* MATLAB */
/****************************************************/
#if defined(MATLAB_SSPACE)


static void
_swap ( float *Src ,
	   float *Dst ,
	   int SrcWidth ,
	   int SrcHeight )
{
	int i , j;

	for ( j = 0 ; j < SrcHeight ; ++j )
		for ( i = 0 ; i < SrcWidth ; ++i , ++Src )
			*( Dst + j + i * SrcHeight ) = *Src;
}






void
mexFunction ( int nlhs , mxArray *plhs[] ,
			 int nrhs , const mxArray *prhs[] )
{
	double *Input = NULL;
	float *TmpBuffer1 , *TmpBuffer2;
	float *nlap = NULL;
	float *sigma = NULL;
	float *SSpaceInput;



	int Type; /* */

	mig_sspace_t    *SSpace;

	/* scale space parameters */
	const mxArray *matParams;
	int ss_type;
	int ss_spacing;
	float ss_sigma_start;
	float ss_sigma_end;
	float ss_sigma_inc;
	float ss_threshold;
	float ss_hres; 
	int ss_centroid_x;
	int ss_centroid_y;
	/***************************/


	int Width , Height;
	int rc = 0;


	int dims[3];

	/* input image */
	Input  = (double*) mxGetPr( prhs[0] );
	Height = (int) mxGetM ( prhs[0] );
	Width  = (int) mxGetN ( prhs[0] );



	/* parameters structure */
	matParams = prhs[1];

	ss_type = (int) mxGetScalar (  mxGetField(matParams, 0, "ss_type" ) );
	ss_spacing = (int) mxGetScalar (  mxGetField(matParams, 0, "ss_spacing" ) ); 
	ss_sigma_start = (float) mxGetScalar (  mxGetField(matParams, 0, "ss_sigma_start" ) ); 
	ss_sigma_end = (float) mxGetScalar (  mxGetField(matParams, 0, "ss_sigma_end" ) ); 
	ss_sigma_inc = (float) mxGetScalar (  mxGetField(matParams, 0, "ss_sigma_inc" ) ); 
	ss_threshold = (float) mxGetScalar (  mxGetField(matParams, 0, "ss_threshold" ) );
	ss_hres = (float) mxGetScalar ( mxGetField(matParams, 0, "ss_hres" ) );
	ss_centroid_x = (int) mxGetScalar ( mxGetField(matParams, 0, "ss_centroid_x" ) );
	ss_centroid_y = (int) mxGetScalar ( mxGetField(matParams, 0, "ss_centroid_y" ) );

	/* function type */
	Type = (int) floor( mxGetScalar ( prhs[2] ) );
	mexPrintf( "Type of call = %d\n" , Type);

	/* Create intermediate buffers */
	TmpBuffer1 = (float*)
		calloc ( Width * Height , sizeof(float) );
	if ( TmpBuffer1 == NULL )
		mexErrMsgTxt ( "\nMemory Error...\n" );

	TmpBuffer2 = (float*)
		calloc ( Width * Height , sizeof(float) );
	if ( TmpBuffer2 == NULL )
		mexErrMsgTxt ( "\nMemory Error...\n" );

	/*** get parameter structure and map into SSpace ***/	
	


	/* prepare scale space structure */
	SSpace = mig_im_sspace_get ( ss_type , /* scale space type -> 2D */
		ss_spacing ,
		ss_sigma_start / ss_hres ,
		ss_sigma_end   / ss_hres ,
		ss_sigma_inc ,
		ss_threshold );

	/* prepare scale space input buffer */
    SSpaceInput = (float*) calloc ( SSpace->window_voxels , sizeof(float) );

	/* convert input from double to float */
	_cpy_dbl2flt ( Input , TmpBuffer2 , Height * Width );
	/* swap input to row major order */
	_swap ( TmpBuffer2 , TmpBuffer1 , Height , Width );

	/****** FROM LIBMIGDET_2D*********/
	/* copy data from original stack into scale space input buffer */
    /*in mex we use 32f instead of mig16u*/
	mig_im_bb_cut_2d_32f ( TmpBuffer1 , 
                           Width , Height , 
                           ss_centroid_x , ss_centroid_y,
                           SSpaceInput , SSpace->window_radius );

    /* scale to [0,1] interval */
	/*in mex we consider input already grayed*/
    mig_im_util_mat2gray_32f ( SSpaceInput , SSpace->window_voxels , 0.0f , 1.0f );
    
	/**********************************/

	/* build scale space representation for input signal */
	rc = _sspace_build ( SSpaceInput , SSpace );
	if ( rc != 0 )
		mexErrMsgTxt ("Scale Space Error : _sspace_build ...\n");


	/*** Function type selection ***/
	if (Type == 0){ /* scale space representations */

		float *TmpBuffer3;
		/* Create Output */

		/*we are returning two arrays (nlap (3d) and sigma (1d) ) for now we let them be real*/
		dims[0]= SSpace->window_len;
		dims[1]= SSpace->window_len;
		dims[2]= SSpace->num_sigmas;

		plhs[0] = mxCreateNumericArray ( 3, dims , mxSINGLE_CLASS , mxREAL );
		if ( plhs[0] == NULL )
			mexErrMsgTxt ( "\nMemory Error...\n" );
		nlap = (float*)
			mxGetPr( plhs[0] );

		plhs[1] = mxCreateNumericArray ( 1, &SSpace->num_sigmas, mxSINGLE_CLASS , mxREAL );
		if ( plhs[1] == NULL )
			mexErrMsgTxt ( "\nMemory Error...\n" );
		sigma = (float*) mxGetPr( plhs[1] );

		/* Create intermediate buffers */
		TmpBuffer3 = (float*)
			calloc ( MIG_POW2(SSpace->window_len) , sizeof(float) );
		if ( TmpBuffer3 == NULL )
			mexErrMsgTxt ( "\nMemory Error...\n" );


		if (ss_type == 1){//2d
			/* copy data into nlap */
			int iSigma;
			int iElement;
			float* tempData;
			float* tempNlap;
			tempNlap = nlap;

			for (iSigma = 0; iSigma != SSpace->num_sigmas; iSigma++){
				/* swap data output to column major order */
				_swap ( SSpace->data[iSigma] , TmpBuffer3 , dims[0] , dims[1] );
				tempData = TmpBuffer3;
				for (iElement = 0; iElement != (dims[0] * dims[1]); iElement++){
					*tempNlap++ = *tempData++;
				}

				//_cpy_flt2dbl ( TmpBuffer2, SSpace->data[iSigma], Width * Height );
			}


			/*recalculate used sigmas (they are not saved)*/
			{
				float *tmpSigma = sigma;
				float sigma_curr = SSpace->sigma_start;

				/*AAAA LINEAR NOT TESTED */
				if (ss_spacing == 1){ /*LINEAR*/
					while ( sigma_curr < ss_sigma_end ){
						*tmpSigma++ = sigma_curr;
						sigma_curr += ss_sigma_inc;
					}
				}
				else if (ss_spacing == 0){
					while ( sigma_curr < SSpace->sigma_end){                        
						*tmpSigma++ = sigma_curr;
						sigma_curr *= SS_K;
					}
				}
				// /* copy data into sigma */
				//for (iSigma = 0; iSigma != SSpace->num_sigmas; iSigma++){
				//	*sigma++ = (double) SSpace->kernels[iSigma];
				//}

			}
		}
		else{
			mexPrintf("Sorry, 3d scale space mex not implemented yet");
			//_fakeReturn(2);
		}
		free( TmpBuffer3 );
	}else if (Type == 1){ /* extrema */

		mig_im_region_t *Region = NULL;
		float *pExtrema = NULL;
		int i = 0;
		int iSigma = 0;

		/* find scale space extrema */
		if ( SSpace->type == 0 ){
			/* rc = _sspace_extrema_3d ( SSpace ); */
			mexPrintf("Sorry, 3d scale space mex not implemented yet");
			return;
			//_fakeReturn(1);
		}
		else
			rc = _sspace_extrema_2d ( SSpace );

		/* create output */
		plhs[0] = mxCreateNumericMatrix ( SSpace->window_voxels , SSpace->num_sigmas , mxSINGLE_CLASS, mxREAL);

		pExtrema = (float*) mxGetPr( plhs[0] );

		/*threshold scale space extrema */
		//_sspace_thr ( SSpace );*/

		//TO TEST FILLING OF STRUCTURE. SEEMS OK
		//for (i = 0; i!= SSpace->window_voxels; i++){
		//	SSpace->extrema[0][i] = i;
		//}
		

		for (iSigma = 0; iSigma != SSpace->num_sigmas; iSigma++){
			for (i = 0; i != SSpace->window_voxels; i++){
				*pExtrema++ = SSpace->extrema[iSigma][i];
			}

			//     /* threshold scale space extrema */
			//     _sspace_thr ( SSpace );*/

			//     if ( SSpace->type == 0 )
			//         Region = _sspace_reg_3d ( SSpace );
			//     else
			//         Region = _sspace_reg_2d ( SSpace );

			//
			//  /* Create Output */

			//plhs[0] = mxCreateNumericMatrix ( 1 , 3 , mxDOUBLE_CLASS, mxREAL); /* centroid coordinates */
			//pCentroid = (double*) mxGetPr( plhs[0] );
			//pCentroid[0] = Region->centroid[0];
			//pCentroid[1] = Region->centroid[1];
			//pCentroid[2] = Region->centroid[2];

			//plhs[1] = mxCreateDoubleScalar ( Region->radius );
			//plhs[2] = mxCreateDoubleScalar ( Region->size );

		}

	}
	/* free resources */
	free ( TmpBuffer1 );
	free ( TmpBuffer2 );
	free (SSpaceInput );
	mig_im_sspace_del ( SSpace );

}
	static void _cpy_dbl2flt ( double *Src, float *Dst, int len ){
		int i = 0;
		for (i = 0; i != len; i++){
			*Dst++ = *Src++;
		}
	}

	static void _cpy_flt2dbl ( float *Src, double *Dst, int len ){
		int i = 0;
		for (i = 0; i != len; i++){
			*Dst++ = *Src++;
		}
	}

#endif /* MATLAB_SSPACE */


