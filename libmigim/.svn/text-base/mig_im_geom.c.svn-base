#include <math.h>
#include "mig_im_geom.h"
#include "mig_ut_mem.h"

#if defined(MATLAB_SCALE)

#include "mex.h"
#include "matrix.h"

void
mexFunction ( int nlhs , mxArray *plhs[] ,
              int nrhs , const mxArray *prhs[] );

#endif  /* MATLAB_SCALE */

/******************************************/
/* PRIVATE */
/******************************************/

/*
        AAA
        In what follows we suppose destionation
        pixel positions are a linear transformation
        of source pixel positions. Thus
        Destination(x,y) = F(Source(u,v)), where
        F is linear transformation. Than
        u = x0 + x1 * x + x2 * y
        v = y0 + y1 * x + y2 * y

        x0 , x1 , x2 , y0 , y1 , y2
        are fixed for each type of transformation
        (translation,rotation,scaling).
*/


/* NEAREST NEIGHBOUR INTERPOLATION */
static void
_affine0 ( float *in ,
           float *out ,
           int inw , int inh ,
           int outw , int outh ,
           float x0 , float x1 , float x2 ,
           float y0 , float y1 , float y2 );

/* BILINEAR INTERPOLATION */
static void
_affine1 ( float *in ,
           float *out ,
           int inw , int inh ,
           int outw , int outh ,
           float x0 , float x1 , float x2 ,
           float y0 , float y1 , float y2 );

/* BICUBIC INTERPOLATION */
static void
_affine2 ( float *in ,
           float *out ,
           int inw , int inh ,
           int outw , int outh ,
           float x0 , float x1 , float x2 ,
           float y0 , float y1 , float y2 );

/******************************************/
#define BILINEAR( c , y1 , y2 , d ) \
do { \
        float p1 , p2; \
        p1 = (float) y1; \
        p2 = (float) y2 - (float) y1; \
        c = p1 + d * p2; \
} while(0)

/******************************************/
#define BICUBIC( c , y1 , y2 , y3 , y4 , d ) \
do { \
      float p1 , p2 , p3 , p4; \
      p1 =  (float) y2; \
      p2 =  - (float) y1 + (float) y3; \
      p3 = 2.0f * (float) y1 - 2.0f * (float) y2 + (float) y3 - (float) y4; \
      p4 = - (float) y1 + (float) y2 - (float) y3 + (float) y4; \
      c = p1 + d * ( p2 + d * ( p3 + d * p4 ) ); \
} while(0)

/******************************************/
/* EXPORTS */
/******************************************/

void
mig_im_geom_rotate ( float *Src , float *Dst ,
                     int Width , int Height ,
                     float Theta , InterpType Type )
{
        float x0 , x1 , x2;
        float y0 , y1 , y2;
        float cx , cy;

        /* transform angle to rads */
        Theta *=  ( 3.14f / 180.0f );

        cx = ( (float)Width  ) * 0.5f;
        cy = ( (float)Height ) * 0.5f;

	/* follows encoding of forward translation +
	   rotation + backward translation */

        x0 = -cosf( Theta ) * cx + sinf( Theta ) * cy + cx;
        x1 =  cosf( Theta );
        x2 = -sinf( Theta );

        y0 = -sinf( Theta ) * cx - cosf( Theta ) * cy + cy;
        y1 =  sinf( Theta );
        y2 =  cosf( Theta );

#if defined(MATLAB_ROT)
        mexPrintf ( "\n%d x %d" , Width , Height );
        mexPrintf ( "\n%f x %f" , cx , cy );
        mexPrintf ( "\n%f x %f x %f" , x0 , x1 , x2 );
        mexPrintf ( "\n%f x %f x %f" , y0 , y1 , y2 );
#endif

        switch( Type )
        {
                case 0 :

                        _affine0 ( Src , Dst ,
                                   Width , Height ,
                                   Width , Height ,
                                   x0 , x1 , x2 ,
                                   y0 , y1 , y2 );
                        break;

                case 1 :

                        _affine1 ( Src , Dst ,
                                   Width , Height ,
                                   Width , Height ,
                                   x0 , x1 , x2 ,
                                   y0 , y1 , y2 );
                        break;

                case 2 :

                        _affine2 ( Src , Dst ,
                                   Width , Height ,
                                   Width , Height ,
                                   x0, x1 , x2 ,
                                   y0 , y1 , y2 );
                        break;

                default :

                        break;
        }
}

/******************************************/
void
mig_im_geom_resize ( float *Src , int SrcWidth , int SrcHeight ,
                     float *Dst , int DstWidth , int DstHeight ,
                     InterpType Type )
{
    int j;

    float x0 , x1 , x2;
    float y0 , y1 , y2;

    float cu , cv;
    float cx , cy;
    float dx , dy;

    cu = ( (float) SrcWidth  ) * 0.5f;
    cv = ( (float) SrcHeight ) * 0.5f;

    cx = ( (float)DstWidth  ) * 0.5f;
    cy = ( (float)DstHeight ) * 0.5f;

    dx = (float) DstWidth  / (float) SrcWidth;
    dy = (float) DstHeight / (float) SrcHeight;

	/* follows encoding of forward translation + scaling + backward translation */

    x0 = cu - ( cx / dx );
    x1 = (float) SrcWidth / (float) DstWidth;
    x2 = 0.0f;

    y0 = cv - ( cy / dy );
    y1 = 0.0f;
    y2 = (float) SrcHeight / (float) DstHeight;

#if defined(MATLAB_SCALE)
    mexPrintf ( "\n%d x %d" , SrcWidth , SrcHeight );
    mexPrintf ( "\n%d x %d" , DstWidth , DstHeight );
    mexPrintf ( "\n%f x %f" , cu , cv );
    mexPrintf ( "\n%f x %f" , cx , cy );
    mexPrintf ( "\n%f x %f" , dx , dy );
    mexPrintf ( "\n%f x %f x %f" , x0 , x1 , x2 );
    mexPrintf ( "\n%f x %f x %f" , y0 , y1 , y2 );
#endif

    switch( Type )
    {
        case 0 :

            _affine0 ( Src , Dst , SrcWidth , SrcHeight , DstWidth , DstHeight ,
                       x0 , x1 , x2 , y0 , y1 , y2 );

            /* correction */
			//memcpy ( Dst + DstWidth * ( DstHeight - 1 ) ,
			//         Dst + DstWidth * ( DstHeight - 2 ) ,
			 //        DstWidth * sizeof(float) );

			for ( j = 0 ; j < DstHeight ; ++j , Dst += DstWidth )
			      Dst[DstWidth-1] = Dst[DstWidth-2];

            break;

        case 1 :

            _affine1 ( Src , Dst , SrcWidth , SrcHeight , DstWidth , DstHeight ,
                       x0 , x1 , x2 , y0 , y1 , y2 );

            /* correction */
			//memcpy ( Dst + DstWidth * ( DstHeight - 2 ) ,
			//         Dst + DstWidth * ( DstHeight - 3 ) ,
			//         DstWidth * sizeof(float) );

			//memcpy ( Dst + DstWidth * ( DstHeight - 1 ) ,
			//         Dst + DstWidth * ( DstHeight - 3 ) ,
			//         DstWidth * sizeof(float) );

			for ( j = 0 ; j < DstHeight ; ++j , Dst += DstWidth )
			      Dst[DstWidth-1] = Dst[DstWidth-2] = Dst[DstWidth-3];

            break;

        case 2 :

            _affine2 ( Src , Dst , SrcWidth , SrcHeight , DstWidth , DstHeight ,
                x0 , x1 , x2 , y0 , y1 , y2 );

            /* correction */

			/* top */
			//memcpy ( Dst + 0 * DstWidth , Dst + 3 * DstWidth , DstWidth * sizeof(float) );
			//memcpy ( Dst + 1 * DstWidth , Dst + 3 * DstWidth , DstWidth * sizeof(float) );
			//memcpy ( Dst + 2 * DstWidth , Dst + 3 * DstWidth , DstWidth * sizeof(float) );

			/* bottom */
			//memcpy ( Dst + DstWidth * ( DstHeight - 2 ) , Dst + DstWidth * ( DstHeight - 3 ) , DstWidth * sizeof(float) );
			//memcpy ( Dst + DstWidth * ( DstHeight - 1 ) , Dst + DstWidth * ( DstHeight - 3 ) , DstWidth * sizeof(float) );

			for ( j = 0 ; j < DstHeight ; ++j , Dst += DstWidth )
			{
				Dst[0] = Dst[1] = Dst[2] = Dst[3];

				Dst[DstWidth-1] = Dst[DstWidth-2] = Dst[DstWidth-3];
			}

            break;

        default :

            break;
        }
}

/****************************************************/
int
mig_im_geom_resize_z ( Mig16u *Src , mig_size_t *SrcSize , Mig16u **Dst , mig_size_t *DstSize )
{
        int i , j , k;
        int NewSliceNumber;
        float u , k0 , k1 , t0 , t1;
        float SrcZRes = SrcSize->z_res;
        float DstZRes = DstSize->z_res;
        Mig16u *Slice0 , *Slice1 , *SliceOut;
        Mig16u *Tmp;

        /* zero out output */
        *Dst = NULL;

        /* if desired resolution is equal to
           input resolution do nothing */
        if ( SrcZRes == DstZRes )
           return MIG_ERROR_INTERNAL;
        
        /* check input -> only one slice do nothing ...*/
        if ( SrcSize->slices <= 1 )
                return MIG_ERROR_INTERNAL;                

        /* copy size */
        memcpy ( DstSize , SrcSize , sizeof(mig_size_t) );

        /* calculate number of slices for new z resolution */
        NewSliceNumber = (int)
                floorf ( ( SrcSize->slices * ( SrcZRes / DstZRes ) - 1.0f  ) + 0.5f );

        if ( NewSliceNumber <= 0 )
                return MIG_ERROR_INTERNAL;

        /* allocate memory for output */
        Tmp = (Mig16u*)
                mig_malloc ( DstSize->dim * 
                             NewSliceNumber * sizeof(Mig16u) );
        if ( Tmp == NULL )
                return MIG_ERROR_MEMORY;
        
        /* set up all DstSize data */
        DstSize->slices = NewSliceNumber;
        DstSize->dim_stack = DstSize->dim * NewSliceNumber;
        DstSize->size_stack = DstSize->size * NewSliceNumber;
        DstSize->z_res = DstZRes;

        /* now do processing for rest of slices */
        for ( k = 0 ; k < NewSliceNumber ; ++k )
        {
                /* find two slices in original stack
                   that are closest to our new slice */
                
                /* float coordinate in source stack 
                   where we come from */
           u = ( (float) k ) * ( DstZRes / SrcZRes ); 
                      
                /* stamo attenti a non cagar
                   fora dal bocal */
                if ( u >= SrcSize->slices - 1 )
                        u = SrcSize->slices - 2;
                
                /* first source slice -> above */
                k0 = floorf ( u );
                
                /* second source slice -> below */
                k1 = k0 + 1.0f;

                t0 = 1.0f - ( u - k0 );
                t1 = 1.0f - t0;

                Slice0   = Src + ( ( (int)k0 ) * SrcSize->dim );
                Slice1   = Slice0 + SrcSize->dim;
                SliceOut = Tmp + k * DstSize->dim;

                /* for all pixels ion current slice */
                for ( j = 0 ; j < DstSize->h ; ++j )
                {
                        for ( i = 0 ; 
                              i < DstSize->w ; ++i , 
                              ++SliceOut , ++Slice0 , ++Slice1 )
                        {
                                *SliceOut = (Mig16u) (
                                        t0 * ((float)*Slice0) + 
                                        t1 * ((float)*Slice1) );
                        } /* i */
                } /* j */
        } /* k */

        /* copy local buffer to output */
        *Dst = Tmp;
        return MIG_OK;  
}

/******************************************/
/* PRIVATE */
/******************************************/

/* NEAREST NEIGHBOUR INTERPOLATION */
static void
_affine0 ( float *in ,
           float *out ,
           int inw , int inh ,
           int outw , int outh ,
           float x0 , float x1 , float x2 ,
           float y0 , float y1 , float y2 )
{
        int i , j;
        int u , v;

        /* for all output pixels */
        for ( j = 0 ; j < outh ; ++j )
        {
                for ( i = 0 ; i < outw ; ++i , ++out )
                {
                        /* source horizontal coordinate */
                        u = (int) floorf ( x0 + x1 * i + x2 * j + 0.5 );

                        /* source vertical coordinate */
                        v = (int) floorf ( y0 + y1 * i + y2 * j + 0.5 );

			if ( ( u < 0 )||
			     ( u > ( inw - 1 ) ) ||
			     ( v < 0 ) ||
			     ( v > ( inh - 1 ) ) )
			{
				*out = 0.0f;
			}
			else
			{
				*out = in[u+v*inw];
			}
                }
        }
}

/******************************************/
/* BI-LINEAR INTERPOLATION ( 2D LINEAR ) */
static void
_affine1 ( float *in ,
           float *out ,
           int inw , int inh ,
           int outw , int outh ,
           float x0 , float x1 , float x2 ,
           float y0 , float y1 , float y2 )

{
        int i , j , l , k;
        float u , v;
        float du , dv;
        float bi1 , bi2 , bi3;

        for ( j = 0 ; j < outh ; ++j )
        {
                for ( i = 0 ; i < outw ; ++i , ++out )
                {
                        u = x0 + x1 * i + x2 * j;
                        v = y0 + y1 * i + y2 * j;

                        k = floorf( u - 1.0f );
                        l = floorf( v - 1.0f );

                        //if negative take the first value
						if ( ( k < 0 ) ||
                             ( l < 0 ) )
                        {

							k = MIG_MAX2(0,k);
							l = MIG_MAX2(0,l);
                        //        *out = in[MIG_MAX2(0,k) + MIG_MAX2(0,l) * inw]; 
                        //        continue;
                        } else if ( ( k > ( inw - 2 ) ) ||
									( l > ( inh - 2 ) ) )
						{
								*out = 0.0f;
						//		*out = in[MIG_MIN2(inw,k) + MIG_MIN2 (inh,l) * inw];
								continue;
						}



                        du = u - k;
                        dv = v - l;

                        /* 1st linear
                           interpolation in
                           horizontal direction */
                        BILINEAR( bi1 , in[k+  l*inw] , in[k+1+l*inw] , du );

                        /* 2nd linear
                           interpolation in
                           horizontal direction */
                        BILINEAR( bi2 , in[k+  (l+1)*inw] , in[k+1+(l+1)*inw] , du );

                        /* final linear
                           interpolation in
                           vertical direction */
                        BILINEAR( bi3 , bi1 , bi2 , dv );
                        *out = bi3;
                }
        }
}

/******************************************/
/* BI-CUBIC INTERPOLATION ( 2D CUBIC ) */
static void
_affine2 ( float *in ,
           float *out ,
           int inw , int inh ,
           int outw , int outh ,
           float x0 , float x1 , float x2 ,
           float y0 , float y1 , float y2 )
{
        int i , j , k , l;
        float u , v;
        float du , dv;

        float cu1 , cu2 , cu3 , cu4 , cu5;

        for ( j = 0 ; j < outh ; ++j )
        {
                for ( i = 0 ; i < outw ; ++i , ++out )
                {
                        u = x0 + x1 * i + x2 * j;
                        v = y0 + y1 * i + y2 * j;

                        k = floorf( u );
                        l = floorf( v );

			if ( ( k < 1 ) ||
                             ( k > ( inw - 2 ) ) ||
                             ( l < 1 ) ||
                             ( l > ( inh - 2 ) ) )
                        {
                                *out = 0.0f;
                                continue;
                        }

                        du = u - k;
                        dv = v - l;

                        BICUBIC( cu1 ,
                            in[k-1 +(l-1)*inw] ,
                            in[k+0 +(l-1)*inw] ,
                            in[k+1 +(l-1)*inw] ,
                            in[k+2 +(l-1)*inw] , du );

                        BICUBIC( cu2 ,
                            in[k-1 + (l+0)*inw] ,
                            in[k+0 + (l+0)*inw] ,
                            in[k+1 + (l+0)*inw] ,
                            in[k+2 + (l+0)*inw] ,
                            du );

                        BICUBIC( cu3 ,
                            in[k-1 + (l+1)*inw] ,
                            in[k+0 + (l+1)*inw ] ,
                            in[k+1 + (l+1)*inw ] ,
                            in[k+2 + (l+1)*inw ] ,
                            du );

                        BICUBIC( cu4 ,
                            in[k-1 + (l+2)*inw] ,
                            in[k+0 + (l+2)*inw] ,
                            in[k+1 + (l+2)*inw] ,
                            in[k+2 + (l+2)*inw] ,
                            du );

                        BICUBIC( cu5 , cu1 ,
                                 cu2 , cu3 ,
                                 cu4 , dv );

                        *out = cu5;
                }
        }
}


/****************************************************/
/* MATLAB */
/****************************************************/
#if defined(MATLAB_SCALE)

void
mexFunction ( int nlhs , mxArray *plhs[] ,
              int nrhs , const mxArray *prhs[] )
{
    float *Input = NULL;
    float *Output = NULL;
    int InputWidth , InputHeight;
    int OutputWidth , OutputHeight;
    int IntType;

    /* get inputs */
    Input = (float*) mxGetPr( prhs[0] );
    InputHeight = (int) mxGetM ( prhs[0] );
    InputWidth  = (int) mxGetN ( prhs[0] );
    
    OutputWidth = (int) mxGetScalar( prhs[1] );
    OutputHeight = (int) mxGetScalar( prhs[2] );

    IntType = (int) mxGetScalar( prhs[3] );

    /* print debugging info */
    mexPrintf( "\n%dx%d,%dx%d,%d\n" , InputWidth, InputHeight,
        OutputWidth, OutputHeight, IntType );
    
    /* prepare output */
    plhs[0] = mxCreateNumericMatrix ( OutputWidth , OutputHeight , mxSINGLE_CLASS , mxREAL );
    if ( plhs[0] == NULL )
        mexErrMsgTxt ( "\nMemory Error...\n" );
    
    Output = (float*) mxGetPr( plhs[0] );

    /* perform resampling */
    mig_im_geom_resize ( Input , InputWidth , InputHeight ,
                         Output , OutputWidth , OutputHeight ,
                         (InterpType) IntType );
}

#endif /* MATLAB_SCALE */


