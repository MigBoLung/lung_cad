/*
******************************************************************************
*
* Author      : Todor Petkov
* Filename    : mig_gauss_3d.c
* Created     : 2007/06/21
* Description : 3D Gaussian IIR filter (Van Vliet)
*
******************************************************************************
*/

#include "mig_im_gauss.h"

/*
******************************************************************************
*               LOCAL PROTOTYPES DECLARATION
******************************************************************************
*/

/*
******************************************************************************
*                       IIR GAUSS FILTER IN HORIZONTAL DIRECTION
*
* Description : This function filters a single row of an input 3D array in horizontal
*               direction using 1D Gaussian IIR filter.
*
* Arguments   : in    - input row to filter
*               out   - output of row filtering operation
*               w     - total width of input signal
*               sigma - gaussian sigma
*
* Returns     :
*
* Notes       :
*
******************************************************************************
*/

static void
_gauss_iir_1d_x ( float *in , float *out , const int w , const float sigma );

/*
******************************************************************************
*                       IIR GAUSS FILTER IN VERTICAL DIRECTION
*
* Description : This function filters a single column of an input 3D array in vertical
*               direction using 1D Gaussian IIR filter.
*
* Arguments   : in    - input column to filter
*               out   - output of column filtering operation
*               w     - total width of input signal
*               h     - total height of input signal
*               sigma - gaussian sigma
*
* Returns     :
*
* Notes       :
*
******************************************************************************
*/

static void
_gauss_iir_1d_y ( float *in , float *out , const int w , const int h , const float sigma );

/*
******************************************************************************
*                       IIR GAUSS FILTER IN Z DIRECTION
*
* Description : This function filters a single z dimension of an input 3D array in z
*               direction using 1D Gaussian IIR filter.
*
* Arguments   : in    - input z signal to filter
*               out   - output of z filtering operation
*               w     - total width of input 3D signal
*               h     - total height of input 3D signal
*               sigma - gaussian sigma
*
* Returns     :
*
* Notes       :
*
******************************************************************************
*/

static void
_gauss_iir_1d_z ( float *in , float *out , const int w , const int h , const int z , const float sigma );

/*
******************************************************************************
*                       IIR GAUSS FILTER IN HORIZONTAL DIRECTION
*
* Description : This function filters a single row of an input 3D array in horizontal
*               direction using 1D Gaussian IIR filter.
*
* Arguments   : in    - input row to filter
*               out   - output of row filtering operation
*               w     - row width
*               sigma - gaussian sigma
*
* Returns     :
*
* Notes       :
*
******************************************************************************
*/

static void
_FilterCoeffs_TriggsCoeffs ( float *b , double *M , const float sigma );

/*
******************************************************************************
*               GLOBAL PROTOTYPES IMPLEMENTATION
******************************************************************************
*/

void
mig_im_gauss_iir_3d ( float *in , float *out , int w , int h , int z , float sigma )
{
        int i , j , k;
        float *idx;

        /* first convolution goes in horizontal direction ( x's ) */
        idx = out;
        for ( k = 0 ; k < z ; ++k )
        {
                for ( j = 0 ; j < h ; ++j , in += w , idx += w )
                {
                        /* perform 1d convolution */
                        _gauss_iir_1d_x ( in , idx , w , sigma );
                }
        }

        /* second convolution goes in vertical direction ( y's ) */
        for ( k = 0 ; k < z ; ++k )
        {
                idx = out + k * w * h;

                for ( i = 0 ; i < w ; ++i , ++idx )
                {
                        /* perform 1 d convolution */
                        _gauss_iir_1d_y ( idx , idx , w , h , sigma );
                }
        }

        /* third and final convolution goes in z direction ( z's ) */
        for ( j = 0 ; j < h ; ++j )
        {
                for ( i = 0 ; i < w ; ++i , ++out )
                {
                        /* perform 1 d convolution */
                        _gauss_iir_1d_z ( out , out , w , h , z , sigma );
                }
        }
}

/******************************************************************************/

void
mig_im_gauss_iir_2d ( float *in , float *out , int w , int h , float sigma )
{
    int i;
    float *idx;

    /* first convolution goes in horizontal direction ( x's ) */
    idx = out;
    for ( i = 0 ; i < h ; ++i , in += w , idx += w )
    {
        /* perform 1d convolution */
        _gauss_iir_1d_x ( in , idx , w , sigma );
    }
        
    /* second convolution goes in vertical direction ( y's ) */
    for ( i = 0 ; i < w ; ++i , ++out )
    {
        /* perform 1 d convolution */
        _gauss_iir_1d_y ( out , out , w , h , sigma );
    }
}

/*
******************************************************************************
*               LOCAL PROTOTYPES IMPLEMENTATION
******************************************************************************
*/

static void
_gauss_iir_1d_x ( float *in , float *out , const int w , const float sigma )
{
        int i;
        float iminus;   //  hypothetical signal value before start time, i.e. t=-1,-2,-3
        float iplus;    // last sample of original signal, needed for right hand boundary values

        float  b[6];
        double M[9];
        float  v[4];
        float uu[3];

        _FilterCoeffs_TriggsCoeffs ( b , M , sigma );
        iplus = in[w-1];

        // Initialize first three steps of the causal filter
        iminus = in[0];
        out[0] = in[0] - ( - b[1]*(iminus-in[0]) + b[2]*(iminus-in[0]) - b[3]*(iminus-in[0]) );
        out[1] = in[1] - ( - b[1]*(out[0]-in[1]) + b[2]*(iminus-in[1]) - b[3]*(iminus-in[1]) );
        out[2] = in[2] - ( - b[1]*(out[1]-in[2]) + b[2]*(out[0]-in[2]) - b[3]*(iminus-in[2]) );

        // the remainder of the causal filter
        for ( i = 3 ; i < w ; ++i )
                out[i] = in[i] - ( - b[1]*(out[i-1]-in[i]) + b[2]*(out[i-2]-in[i]) - b[3]*(out[i-3]-in[i]) );

        // The first three steps of the anticausal filter
        uu[0] = out[w-1] - iplus;
        uu[1] = out[w-2] - iplus;
        uu[2] = out[w-3] - iplus;

        v[0] = M[0]*uu[0] - M[1]*uu[1] + M[2]*uu[2] + iplus;
        v[1] = M[3]*uu[0] - M[4]*uu[1] + M[5]*uu[2] + iplus;
        v[2] = M[6]*uu[0] - M[7]*uu[1] + M[8]*uu[2] + iplus;

        out[w-1] -= - b[1]*(v[0]      -out[w-1]) + b[2]*(v[1]-out[w-1]) - b[3]*(v[2]-out[w-1]);
        out[w-2] -= - b[1]*(out[w-1]-out[w-2]) + b[2]*(v[0]-out[w-2]) - b[3]*(v[1]-out[w-2]);
        out[w-3] -= - b[1]*(out[w-2]-out[w-3]) + b[2]*(out[w-1]-out[w-3]) - b[3]*(v[0]-out[w-3]);

        // the remainder of the anti-causal filter
        for ( i = w - 4 ; i >= 0 ; --i )
                out[i] -= - b[1]*(out[i+1]-out[i]) + b[2]*(out[i+2]-out[i]) - b[3]*(out[i+3]-out[i]);
}

/*****************************************************************************/

static void
_gauss_iir_1d_y ( float *in , float *out , const int w , const int h , const float sigma )
{
        int i;
        float iminus;   //  hypothetical signal value before start time, i.e. t=-1,-2,-3
        float iplus;    // last sample of original signal, needed for right hand boundary values

        float  b[6];
        double M[9];
        float  v[4];
        float uu[3];

        _FilterCoeffs_TriggsCoeffs ( b , M , sigma );
        iplus = in[(h-1)*w];

        // Initialize first three steps of the causal filter
        iminus = in[0];
        out[0] = in[0] - ( - b[1]*(iminus-in[0]) + b[2]*(iminus-in[0]) - b[3]*(iminus-in[0]) );
        out[w] = in[w] - ( - b[1]*(out[0]-in[w]) + b[2]*(iminus-in[w]) - b[3]*(iminus-in[w]) );
        out[2*w] = in[2*w] - ( - b[1]*(out[w]-in[2*w]) + b[2]*(out[0]-in[2*w]) - b[3]*(iminus-in[2*w]) );

        // the remainder of the causal filter
        for ( i = 3 ; i < h ; ++i )
                out[i*w] = in[i*w] - ( - b[1]*(out[(i-1)*w]-in[i*w]) + b[2]*(out[(i-2)*w]-in[i*w]) - b[3]*(out[(i-3)*w]-in[i*w]) );

        // The first three steps of the anticausal filter
        uu[0] = out[w*(h-1)] - iplus;
        uu[1] = out[w*(h-2)] - iplus;
        uu[2] = out[w*(h-3)] - iplus;

        v[0] = M[0]*uu[0] - M[1]*uu[1] + M[2]*uu[2] + iplus;
        v[1] = M[3]*uu[0] - M[4]*uu[1] + M[5]*uu[2] + iplus;
        v[2] = M[6]*uu[0] - M[7]*uu[1] + M[8]*uu[2] + iplus;

        out[w*(h-1)] -= - b[1]*(v[0] - out[w*(h-1)]) + b[2]*(v[1]-out[w*(h-1)] ) - b[3]*(v[2]-out[w*(h-1)]);
        out[w*(h-2)] -= - b[1]*(out[w*(h-1)]-out[w*(h-2)]) + b[2]*(v[0]-out[w*(h-2)] ) - b[3]*(v[1]-out[w*(h-2)]);
        out[w*(h-3)] -= - b[1]*(out[w*(h-2)]-out[w*(h-3)]) + b[2]*(out[w*(h-1)]-out[w*(h-3)]) - b[3]*(v[0]-out[w*(h-3)]);

        // the remainder of the anti-causal filter
        for ( i = h - 4 ; i >= 0 ; --i )
                out[i*w] -= - b[1]*(out[w*(i+1)]-out[w*i]) + b[2]*(out[w*(i+2)]-out[w*i]) - b[3]*(out[w*(i+3)]-out[w*i]);
}

/*****************************************************************************/

static void
_gauss_iir_1d_z ( float *in , float *out , const int w , const int h , const int z , const float sigma )
{
        int i;
        float iminus;   //  hypothetical signal value before start time, i.e. t=-1,-2,-3
        float iplus;    // last sample of original signal, needed for right hand boundary values

        float  b[6];
        double M[9];
        float  v[4];
        float uu[3];

        _FilterCoeffs_TriggsCoeffs ( b , M , sigma );
        iplus = in[w*h*(z-1)];

        // Initialize first three steps of the causal filter
        iminus = in[0];
        out[0] = in[0] - ( - b[1]*(iminus-in[0]) + b[2]*(iminus-in[0]) - b[3]*(iminus-in[0]) );
        out[w*h] = in[w*h] - ( - b[1]*(out[0]-in[w*h]) + b[2]*(iminus-in[w*h]) - b[3]*(iminus-in[w*h]) );
        out[2*w*h] = in[2*w*h] - ( - b[1]*(out[w*h]-in[2*w*h]) + b[2]*(out[0]-in[2*w*h]) - b[3]*(iminus-in[2*w*h]) );

        // the remainder of the causal filter
        for ( i = 3 ; i < z ; ++i )
                out[i*w*h] = in[i*w*h] - ( - b[1]*(out[(i-1)*w*h]-in[i*w*h]) + b[2]*(out[(i-2)*w*h]-in[i*w*h]) - b[3]*(out[(i-3)*w*h]-in[i*w*h]) );

        // The first three steps of the anticausal filter
        uu[0] = out[w*h*(z-1)] - iplus;
        uu[1] = out[w*h*(z-2)] - iplus;
        uu[2] = out[w*h*(z-3)] - iplus;

        v[0] = M[0]*uu[0] - M[1]*uu[1] + M[2]*uu[2] + iplus;
        v[1] = M[3]*uu[0] - M[4]*uu[1] + M[5]*uu[2] + iplus;
        v[2] = M[6]*uu[0] - M[7]*uu[1] + M[8]*uu[2] + iplus;

        out[w*h*(z-1)] -= - b[1]*(v[0] - out[w*h*(z-1)]) + b[2]*(v[1]-out[w*h*(z-1)] ) - b[3]*(v[2]-out[w*h*(z-1)]);
        out[w*h*(z-2)] -= - b[1]*(out[w*h*(z-1)]-out[w*h*(z-2)]) + b[2]*(v[0]-out[w*h*(z-2)] ) - b[3]*(v[1]-out[w*h*(z-2)]);
        out[w*h*(z-3)] -= - b[1]*(out[w*h*(z-2)]-out[w*h*(z-3)]) + b[2]*(out[w*h*(z-1)]-out[w*h*(z-3)]) - b[3]*(v[0]-out[w*h*(z-3)]);

        // the remainder of the anti-causal filter
        for ( i = z - 4 ; i >= 0 ; --i )
                out[i*w*h] -= - b[1]*(out[w*h*(i+1)]-out[w*h*i]) + b[2]*(out[w*h*(i+2)]-out[w*h*i]) - b[3]*(out[w*h*(i+3)]-out[w*h*i]);
}

/*****************************************************************************/

static void
_FilterCoeffs_TriggsCoeffs ( float *b , double *M , const float sigma )
{
        /** Compute the coefficients for the recursive filter.
         *  @see Young and Van Vliet, Signal Processing (2002)
        **/
        double a1;
        double a2;
        double a3;
        double norm;

        double m0 = 1.16680;
        double m1 = 1.10783;
        double m2 = 1.40586;
        double b1,b2,b3;
        double q,scale;

        //  q = 1.31564 * (sqrt(1.+0.490811*sigma*sigma)-1.);
        if ( sigma < 3.3556 )
  	        q = -0.2568 + 0.5784*sigma + 0.0561*sigma*sigma;
        else
  	        q = 2.5091 + 0.9804*(sigma - 3.556);

        scale = (m0+q)*(m1*m1+m2*m2+2*m1*q+q*q);
        b1 = q*(2*m0*m1+m1*m1+m2*m2+(2*m0+4*m1)*q+3*q*q)/scale;
        b2 = q*q*(m0+2*m1+3*q)/scale;
        b3 = q*q*q/scale;

        b[0] = 1.;
        b[1] = b1;
        b[2] = b2;
        b[3] = b3;

        /** Compute the matrix M necessary to apply the Triggs boundary conditions.
         *  @see Triggs and Sdika, IEEE Trans. Signal Processing (to appear)
        **/

        a1 = b1;
        a2 = -b2;
        a3 = b3;
        norm = 1.0/((1.0+a1-a2+a3)*(1.0+a2+(a1-a3)*a3));

        M[0] = norm*(-a3*a1+1.0-a3*a3-a2) ;
        M[1] = -norm*(a3+a1)*(a2+a3*a1) ;
        M[2] = norm*a3*(a1+a3*a2) ;
        M[3] = norm*(a1+a3*a2) ;
        M[4] = norm*(a2-1.0)*(a2+a3*a1 ) ;
        M[5] = -norm*a3*(a3*a1+a3*a3+a2-1.0) ;
        M[6] = norm*(a3*a1+a2+a1*a1-a2*a2) ;
        M[7] = -norm*(a1*a2+a3*a2*a2-a1*a3*a3-a3*a3*a3-a3*a2+a3) ;
        M[8] = norm*a3*(a1+a3*a2) ;
}

