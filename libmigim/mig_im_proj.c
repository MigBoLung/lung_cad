#include "mig_im_proj.h"
/* #include "mig_ut_mem.h" */
/******************************************************************************/
/* local defines */
/******************************************************************************/
#ifndef MAX
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef PI
#define PI 3.141592653589
#endif
#ifndef MAXFLOAT
#define MAXFLOAT ((float)3.40282346638528860e+38) 
#endif

/*	

		0 (i), j , k	-	k * h + j
		1  i ,(j), k	-	k * w + i
		2  i , j ,(k)	-	j * w + i */
#define PIXOFFSET_2D(i,j,w) (j * w + i)
#define PIXOFFSET(i,j,k,w,h) ( i + j * w + k * w * h)
#ifdef MATLAB_MEX

#include "mex.h"
#include "matrix.h"

void
mexFunction ( int nlhs , mxArray *plhs[] ,
              int nrhs , const mxArray *prhs[] );

#endif  /* MATLAB_MEX */


/******************************************************************************/
/* private function declarations */
/******************************************************************************/

/******************************/
/* exported functions */
/******************************/

/* computation along axes of entire volume */
/* TODO: far better with ifs! in order to speed up, */
/* find a way to advance pointers instead of calculate */
/* positions every time */

/* in order to use SSE2 we could use in a smart way the fact that data are
   always adiacent in x (so 2 different approaches are required: one for mip along x
   and one for mip along other axes */

int
mig_im_proj_mip_axes_vol_32f(  float *src , float *dst , int w , int h, int d, int dir_idx, float ratio)
{	
	int i, j, k, start, stop, stoptoend, tostart;
	int rc = 0;
	
	float *pSrc = src;
	float *pDst = dst;

	/*float lastval = 0;*/

	if (ratio < 0 || ratio > 1) return MIG_ERROR_PARAM;


	switch (dir_idx)
	{
	case 0:
		k = 0;
		start = w * (1.0001 - ratio) / 2;
		stop = MIN(w - start + 1, w);
		stoptoend = w - stop ;

		for ( k = 0; k != d; ++k )
		{
			for (j = 0; j != h; ++j, ++pDst )
			{
				/*reach start position */
				pSrc += start;
				/* first time copy */
				*pDst = *pSrc;
				for ( i = start + 1; i <= stop; ++i , ++pSrc )
				{
					/*max*/
					if ( *pDst < *pSrc )
					{
						*pDst = *pSrc;
					}
					
				}
				/* reach end of row*/
				pSrc += stoptoend;	
			}
		}
			
		break;
	
	/*less efficient than the first one (we use PIXOFFSET_2D), but better than before,*/
	/*  (we don't use PIXOFFSET_3D )*/


		/*TODO: UNFINISHED! DOESN'T WORK! */
	case 1: 
		k = 0;
		start = h * (1.001 - ratio) / 2;
		stop = MIN(h - start + 1, h);
		tostart = ( w * ( start ));
		stoptoend = ( w * (h - stop -1)) ;

		for ( k = 0; k != d; ++k )
		{
			/*advance pSrc till start */
			pSrc += tostart;
			
			/*first time copy */
			for ( i = 0; i != w; ++i , ++pSrc)
			{
				pDst = dst + PIXOFFSET_2D(i,k,w);
				*pDst = *pSrc;
			}

			for (j = start+1; j <= stop; ++j )
			{
				for ( i = 0; i != w; ++i , ++pSrc)
				{
					pDst = dst + PIXOFFSET_2D(i,k,w);
					if ( *pDst < *pSrc )
					{
						*pDst = *pSrc;
					}
					
				}
			}

			/* advance to end */
			pSrc += stoptoend;
		}
			

		break;

	case 2:
		/*this is the simpler and most effective one, due to natural order */
		start = d * (1.001 - ratio) / 2;
		stop = MIN(d - start + 1, d);
		
		/*move pSrc to start */
		pSrc += w * h * start; 

		/*first time copy */
		for (j = 0; j != h; ++j )
		{
			for ( i = 0; i != w; ++i , ++pSrc, ++pDst){
				*pDst = *pSrc;
			}
		}

		for ( k = start+1; k <= stop; ++k ){
			pDst = dst;
			for (j = 0; j != h; ++j ){
				for ( i = 0; i != w; ++i , ++pSrc, ++pDst){
					if ( *pDst < *pSrc )
					{
						*pDst = *pSrc;
					}
					
				}
			}
		}

		break;

	default:
		rc = MIG_ERROR_PARAM;
	}
	return rc;

}


int mig_im_proj_mip_z_stack_single(  float *src , float *dst , int w , int h, int d, int z, int radius)
{
	int rc = 0;
	int start = MAX(z - radius,0);
	int stop = MIN(z + radius + 1, d);
	int i,j,k;
	for ( i = 0; i != w; i++ ){
		for (j = 0; j != h; j++ ){
			/* first time copy */	
			k = start;
			*(dst + PIXOFFSET_2D(i,j,w)) = *(src + PIXOFFSET(i,j,k,w,h));		
			for ( k = start + 1; k != stop; k++ ){
				*(dst + PIXOFFSET_2D(i,j,w)) = 
					MAX( *(dst + PIXOFFSET_2D(i,j,w)), *(src + PIXOFFSET(i,j,k,w,h)));
			}

		}
	}
	return rc;
}

int 
mig_im_proj_mip_z_stack_whole( float *src , float *dst , int w , int h, int d, int radius)
{
	int rc = 0;
	int z = 0;
	for (z = 0; z != d; z++){
		rc = mig_im_proj_mip_z_stack_single( src, dst + w * h * z, w, h, d, z, radius);
		if (rc != 0)
			break;
	}

	return rc;
}



/*******************************************************************/

/*******************************************************************/
#undef MIN
#undef MAX
#undef PIX_SORT
#undef PIX_SWAP
#undef MAX_USHORT
#undef PI
#undef PIXOFFSET
#undef PIXOFFSET_2D

/*******************************************************************/

#ifdef MATLAB_MEX
void
mexFunction ( int nlhs , mxArray *plhs[] ,
              int nrhs , const mxArray *prhs[] )
{
        float *Input = NULL;
        float *TmpBuffer1 , *TmpBuffer2;
        float *Output = NULL;
             
		int Width , Height;
		float ratio = 1.;
		int radius = 0; 
		int type;
        int rc = 0;
		int direction = 2; /*default z */
		int out_buf_len, out_buf_m, out_buf_n;

		int k = 0;
		
		int h,w,d,z;

        /* input volume */
        Input  = (float*) mxGetPr( prhs[0] );
        h = ( int ) mxGetDimensions(prhs[0])[0];
        w = ( int ) mxGetDimensions(prhs[0])[1];
		d = ( int ) mxGetDimensions(prhs[0])[2];

		
		type = (int) *mxGetPr( prhs[1] );
		if (type == 1)
		{
			direction	= (int) *mxGetPr( prhs[2] );
			ratio		= (float) *mxGetPr( prhs[3] );
		}
		else if (type == 2)
		{
			z = ((int) *mxGetPr( prhs[2] )) - 1;
			radius = (int) *mxGetPr( prhs[3] );
		}

		
		switch (direction)
		{
		case 0:
			out_buf_len = h * d;
			out_buf_m = d;
			out_buf_n = h;
			break;
		case 1:
			out_buf_len = w * d;
			out_buf_m = d;
			out_buf_n = w;
			break;
		case 2:
			out_buf_len = h * w;
			out_buf_m = w;
			out_buf_n = h;
			break;
		}

        /* Create Output */
        plhs[0] = mxCreateNumericMatrix ( out_buf_m , out_buf_n, mxSINGLE_CLASS , mxREAL );
        if ( plhs[0] == NULL )
                mexErrMsgTxt ( "\nMemory Error...\n" );
        Output = (float*)
                mxGetPr( plhs[0] );
				
		if (type == 1)
		{
			rc = mig_im_proj_mip_axes_vol(  Input , Output , w, h, d, direction, ratio);
		}
		else if (type == 2)
		{
			rc = mig_im_proj_mip_z_stack_single(  Input , Output , w, h, d, z, radius);
		}

}

#endif /* MATLAB_MEX */



