#include "mig_im_polar.h"
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

#define MIN_R_DIFF 0.1
#define PI 3.141592653589

#if defined(MATLAB_MEX)

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

#endif  /* MATLAB_MEX */


/******************************************************************************/
/* private function declarations */
/******************************************************************************/

/******************************/
/* exported functions */
/******************************/

void
mig_im_polar (  float *src , float *dst , int w , int h, int nr , int nphy , float min_r, float max_r)
{	
	int idx_r, idx_phy, x, y, min_dim;

	float r, r_inc, phy_inc, phy;
	
	float *pout;

	if (nr == 0) nr = 1;
	if (nphy == 0) nphy = 1;

	/* AAAAA This should be checked */
	if (min_r < 0 || min_r >= 1) min_r = 0.;
	if (max_r <= min_r + MIN_R_DIFF || max_r > 1.) max_r = min_r + MIN_R_DIFF;
	
	min_dim = MIN(w,h);

	r_inc = ((max_r - min_r) * 0.5 * min_dim) / nr;
	phy_inc = 2. * PI / nphy;
	r = min_r * 0.5 * min_dim; /* AAA take care of center point (but is it needed?) */
	phy = 0;
	
	pout = dst;
	for (idx_r = 0; idx_r != nr; idx_r++, r += r_inc){
		phy = 0;
		for (idx_phy = 0; idx_phy != nphy; idx_phy++, phy += phy_inc){	
			
			x = MIN(MAX(0, r * cos(phy) + 0.5 * w), w);
			y = MIN(MAX(0, 0.5 * h - r * sin(phy)), h); /*correction for inverse ordinates */
			*pout++ = *(src + y * w + x);
			

			
			/* AAAA here we don't do interpolation,
			it should be ok for FPR-2 purposes because
			we then compute moments */
		}
	}
}

/*******************************************************************/

/*******************************************************************/
#undef MIN
#undef MAX
#undef PIX_SORT
#undef PIX_SWAP
#undef MAX_USHORT
#undef PI
/*******************************************************************/

#if defined(MATLAB_MEX)
void
mexFunction ( int nlhs , mxArray *plhs[] ,
              int nrhs , const mxArray *prhs[] )
{
        double *Input = NULL;
        float *TmpBuffer1 , *TmpBuffer2, *TmpBuffer3;
        float *Output = NULL;
             
		int Width , Height, nr, nphy;
		float min_r, max_r;
        int rc = 0;

        /* input image */
        Input  = (double*) mxGetPr( prhs[0] );
        Height = (int) mxGetM ( prhs[0] );
        Width  = (int) mxGetN ( prhs[0] );

		nr		= (int) *mxGetPr( prhs[1] );
		nphy	= (int) *mxGetPr( prhs[2] );
		min_r	= (float) *mxGetPr ( prhs[3] );
		max_r	= (float) *mxGetPr ( prhs[4] );

        /* Create intermediate buffers */
        TmpBuffer1 = (float*)
                calloc ( Width * Height , sizeof(float) );
        if ( TmpBuffer1 == NULL )
                mexErrMsgTxt ( "\nMemory Error...\n" );
		
		TmpBuffer2 = (float*)
                calloc ( nr * nphy , sizeof(float) );
        if ( TmpBuffer2 == NULL )
                mexErrMsgTxt ( "\nMemory Error...\n" );

		TmpBuffer3 = (float*)
                calloc ( Width * Height , sizeof(float) );
        if ( TmpBuffer3 == NULL )
                mexErrMsgTxt ( "\nMemory Error...\n" );

        /* Create Output */
        plhs[0] = mxCreateNumericMatrix ( nr , nphy, mxSINGLE_CLASS , mxREAL );
        if ( plhs[0] == NULL )
                mexErrMsgTxt ( "\nMemory Error...\n" );
        Output = (float*)
                mxGetPr( plhs[0] );
		
		_cpy_dbl2flt ( Input, TmpBuffer3, Height * Width);
		
		/* swap input to row major order */
		_swap ( TmpBuffer3 , TmpBuffer1 , Height, Width );

        
        

        mig_im_polar ( TmpBuffer1, TmpBuffer2 , Width , Height, nr , nphy , min_r, max_r);
		
		/* swap output to column major order */
		_swap ( TmpBuffer2 , Output , nphy, nr );
		
        /* free resources */
        free ( TmpBuffer1 );
		free ( TmpBuffer2 );
		free ( TmpBuffer3 );
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

#endif /* MATLAB_MEX */



