#include "mig_math_fit.h"

/********************************************************************************************************************/
/* MATLAB */
/********************************************************************************************************************/
#if defined(MATLAB_TEST_FIT)

#include "mex.h"
#include "matrix.h"

#endif

/********************************************************************************************************************/
/* a + bx */
void
mig_math_polyfit_linear ( float *x , float *y , int len , float *a , float *b )
{
    int i;
    double sx = 0.0;
	double sy = 0.0;
	double meanx , st2 = 0.0 , t;

    *a = *b = 0.0f;

    /* calculate sx and sy */
    for ( i = 0 ; i < len ; ++i )
    {
        sx += x[i];
        sy += y[i];
    }

    meanx = sx / len;

    for ( i = 0 ; i < len ; ++i )
    {
        t = x[i] - meanx;
        st2 += t * t;
        *b += t * y[i];
    }

    if ( st2 == 0 )
        *b = 0.0f;
    else
        *b /= st2;

    *a = ( sy - sx * ( *b ) ) / len;
}

/********************************************************************************************************************/
/* a + b * x */
void
mig_math_polyval_linear ( float *x , float *y , int len , float a , float b )
{
    int i;

    for ( i = 0 ; i < len ; ++i )
    {
        y[i] = a + b * x[i];
    }
}

/********************************************************************************************************************/
/* MATLAB */
/********************************************************************************************************************/
#if defined(MATLAB_TEST_FIT)

void
mexFunction ( int nlhs , mxArray *plhs[] , int nrhs , const mxArray *prhs[] )
{
    float *x, *y , *p;
    int len;

    /* get input parameters */
    x   = (float*) mxGetPr( prhs[0] );
    y   = (float*) mxGetPr( prhs[1] );
    len = ( (int) mxGetM ( prhs[0] ) ) * ( (int) mxGetN ( prhs[0] ) );

    /* prepare output */
    plhs[0] = mxCreateNumericMatrix ( 1 , 2 , mxSINGLE_CLASS , mxREAL );
    if ( plhs[0] == NULL )
        mexErrMsgTxt ( "\nMemory Error...\n" );

    p = (float*) mxGetPr( plhs[0] );

    mig_math_polyfit_linear ( x , y , len , &p[0] , &p[1] );
}

#endif /* MATLAB_TEST_FIT */