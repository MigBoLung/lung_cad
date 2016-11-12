/*
******************************************************************************
*
* Author      : Todor Petkov
* Filename    : mig_im_fradial.c
* Created     : 2007/06/21
* Description : 3D Fast Radial Filter (Loy & Zelinski)
*
******************************************************************************
*/

#include "mig_im_fradial.h"
#include "mig_im_drv.h"
#include "mig_im_gauss.h"
#include "mig_im_regc.h"


//#define FR_NPARTS 4
#define FR_MAXSLICEPERPART 100


/* MATLAB */
//#define MATLAB_RADIAL

#if defined(MATLAB_RADIAL)

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
*               LOCAL PROTOTYPES DECLARATION
******************************************************************************
*/

#if defined(__cplusplus)
extern "C" {
#endif

#define NSIGMA_THR 1.f
#define THR_BOX_RADIUS 15

/*
******************************************************************************
*                       DUMP ARRAY TO DISK
*
* Description : This function writes data taken from input array to a binary
*               file on disk. 
*
* Arguments   : name - name of file where to write data
*               data - data array
*               w    - data array width
*               h    - data array height
*               z    - data array z
*
* Returns     :  0 on success
*               -1 on error
*
* Notes       :
*
******************************************************************************
*/

static int
_dump_to_disk ( const char *name , const float *data , int w , int h , int z );
/*
******************************************************************************
*                       MAGNITUDE AND ORIENTATION PROJECTIONS IN 3D
*
* Description : This function calculates contribution to radial simmetry
*               of every pixel to pixels at distance radius. Orientation
*               and Magnitude projection matrices are updated.
*
* Arguments   : dx     - input gradient in horizontal direction
*               dy     - input gradient in vertical direction
*               dz     - input gradient in z direction
*               dmag   - input gradient magnitude
*               o      - modified orientation projection matrix
*               m      - modified magnitude projection matrix
*               radius - input affected pixels distance
*               w      - input signals width
*               h      - input signals height
*               z      - input signals z
*
* Returns     :
*
* Notes       :
*
******************************************************************************
*/

static void
_proj_3d ( float *dx , float *dy , float *dz , float *dmag ,
           float *o , float *m , float radius ,
           int w , int h , int z );

/*
******************************************************************************
*                       MAGNITUDE AND ORIENTATION PROJECTIONS IN 2D
*
* Description : This function calculates contribution to radial simmetry
*               of every pixel to pixels at distance radius. Orientation
*               and Magnitude projection matrices are updated.
*
* Arguments   : dx     - input gradient in horizontal direction
*               dy     - input gradient in vertical direction
*               dmag   - input gradient magnitude
*               o      - modified orientation projection matrix
*               m      - modified magnitude projection matrix
*               radius - input affected pixels distance
*               w      - input signals width
*               h      - input signals height
*
* Returns     :
*
* Notes       :
*
******************************************************************************
*/

static void
_proj_2d ( float *dx , float *dy , float *dmag ,
           float *o , float *m , float radius ,
           int w , int h );

/*
******************************************************************************
*                       RADIAL SIMMETRY FUNCTION IN 3D
*
* Description : This function calculates radial simmetry coefficient for
*               every point of Orientation and Magnitude projection matrices.
*
* Arguments   : o      - input orientation projection matrix.
*               m      - magnitude projection matrix.
*               f      - output radial simmetry coefficient
*               radius - input affected pixels distance ( weighting factor )
*               w      - signals width
*               h      - signals height
*               z      - signals z
*
* Returns     :
*
* Notes       :
*
******************************************************************************
*/

static void
_f_3d ( float *o , float *m , float *f , float radius , int w , int h , int z );

/*
******************************************************************************
*                       RADIAL SIMMETRY FUNCTION IN 2D
*
* Description : This function calculates radial simmetry coefficient for
*               every point of Orientation and Magnitude projection matrices.
*
* Arguments   : o      - input orientation projection matrix.
*               m      - magnitude projection matrix.
*               f      - output radial simmetry coefficient
*               radius - input affected pixels distance ( weighting factor )
*               w      - signals width
*               h      - signals height
*
* Returns     :
*
* Notes       :
*
******************************************************************************
*/

static void
_f_2d ( float *o , float *m , float *f , float radius , int w , int h );

/*
******************************************************************************
*                       FAST RADIAL FILTERING
*
* Description : This function performs fast radial filtering on input signal.
*
* Arguments   : in        - input signal.
*               out       - output filtered signal.
*               w         - signals width.
*               h         - signals height.
*               z         - signals z.
*               radii     - input array of radii ( radial distances ).
*               num_radii - input number of radii inside radii array.
*
* Returns     : 0 on success
*               -1 on error
*
* Notes       :
*
******************************************************************************
*/

static int
_radial_3d ( float *in , float *out , int w , int h , int z ,
			float *radii , int num_radii , float beta);

/*
******************************************************************************
*                       FAST RADIAL FILTERING
*
* Description : This function performs fast radial filtering on input signal.
*
* Arguments   : in        - input signal.
*               out       - output filtered signal.
*               w         - signals width.
*               h         - signals height.
*               radii     - input array of radii ( radial distances ).
*               num_radii - input number of radii inside radii array.
*
* Returns     : 0 on success
*               -1 on error
*
* Notes       :
*
******************************************************************************
*/

static int
_radial_2d ( float *in , float *out , int w , int h , float *radii , int num_radii );

/*
******************************************************************************
*                       MAXIMUM VALUE
******************************************************************************
*/

static void
_find_max ( float *Input , int InputLen , float *MaxVal );

#if defined(__cplusplus)
}
#endif

/*
******************************************************************************
*               GLOBAL PROTOTYPES IMPLEMENTATION
******************************************************************************
*/

mig_fradial_t*
mig_im_fradial_get ( float *radii , int num_radii , float threshold , ThresholdType type , float beta_thr )
{
    mig_fradial_t *FastRadial;

    FastRadial = (mig_fradial_t*)
            calloc ( 1 , sizeof(mig_fradial_t) );
    if ( FastRadial == NULL )
            return NULL;

    FastRadial->radii = radii;
    FastRadial->num_radii = num_radii;
    FastRadial->threshold = threshold;
    FastRadial->thr_type = type;
	FastRadial->beta_threshold = beta_thr;

    return FastRadial;
}

/****************************************************************************/


//UNPARTITIONED
//int
//mig_im_fradial_3d ( unsigned short *Input , 
//                    int w , int h , int z , 
//                    mig_fradial_t *FastRadial ,
//                    mig_lst_t *Regions )
//{
//    int rc = 0;
//    float *Buffer;
//	/* float *Buffer1; */
//    float MaxResponse = 0.0f;
//    char fname[MAX_PATH];
//    float thr;
//
//    /* allocate 1 intermediate buffer for processing */
//    Buffer = (float*) calloc ( w * h * z , sizeof(float) );
//    if ( Buffer == NULL )
//        return -1;
//
//    /* perform fast radial processing */
//    rc = _radial_3d ( Input , Buffer , w , h , z ,
//                      FastRadial->radii , FastRadial->num_radii );
//    if ( rc != 0 )
//    {
//        free ( Buffer );
//        return -1;
//    }
//
//    /* dump fast radial result if asked to */
//    if ( FastRadial->dump == 1 )
//    {
//        snprintf ( fname , MAX_PATH , "%s_res" , FastRadial->prefix );
//        rc = _dump_to_disk ( fname , Buffer , w , h , z );
//    }
//
//    if ( FastRadial->thr_type == PERCENT )
//    {
//        /* find maximum response of fast radial filter */
//        _find_max ( Buffer , w * h * z , &MaxResponse );
//
//        if ( MaxResponse == 0.0f )
//        {
//            free ( Buffer );
//            return 0;
//        }
//
//        thr = FastRadial->threshold * MaxResponse;
//    }
//    else
//    {
//        thr = FastRadial->threshold;
//    }
//
//
//	/* apply fixed threshold on fast radial responses */
//	mig_im_thr_32f_i_val ( Buffer , w * h * z , thr * MIG_MAX_16U);
//
//	/*
//	the threshold above is ok to remove unwanted values,
//	but its output regions are too big (and this affects centroid calculation),
//	so it's necessary to add local adaptive thresholding
//	*/
//
//	/* apply mean threshold */
//	mig_im_thr_32f_3d_local_mean ( Buffer, w , h, z, THR_BOX_RADIUS );
//	
//    /* dump thresholded fast radial result if asked to */
//    if ( FastRadial->dump == 1 )
//    {
//        snprintf ( fname , MAX_PATH , "%s_thr" , FastRadial->prefix );
//        rc = _dump_to_disk ( fname , Buffer , w , h , z );
//    }
//
//
//    
//	/* TODO: SLOW, we should consider shrinking */
//	/* construct 3d regions from binarized volume */
//	rc = mig_im_regc_3d ( Buffer , w , h , z , Regions );
//
//	/* if we want to work on fr masks instead of centroids, 
//		comment previous and uncomment next*/
//	/*just create one region for each pixel "on" */
//	/*rc = mig_im_regc_3d_odd ( Buffer, w, h , z , Regions );*/
//    if ( rc != 0 )
//    {
//        free ( Buffer );
//        return -1;
//    }
//
//    free ( Buffer );
//    return 0;
//}


/**********************************************************************
 *	GF: NOTE: in order to avoid memory problems,
 *	fast radial is performed on volumes partitioned in z, with overlap.
 *	Results were tested and are the same with and without
 * 	partitioning, but this function requires way less memory.
 *	In order to choose between speed and memory, change the number
 *	of partitions, now hardcoded in the nparts variable
 **********************************************************************/

int
mig_im_fradial_3d ( unsigned short *Input , 
                    int w , int h , int z , 
                    mig_fradial_t *FastRadial ,
                    mig_lst_t *Regions )
{
    int rc = 0;
	float *in_float;
    float *Buffer;
	float *BufferFR;
    float MaxResponse = 0.0f;
    char fname[MAX_PATH];
    float thr;

	int z_part = 0;
	int z_overlap = 0;
	int z_part_overlap = 0;
	//int nparts = FR_NPARTS;
	int nparts = z / FR_MAXSLICEPERPART + 1;
	int ipart = 0;

	int partoffset = 0;

	/* allocate buffer for float conversion */
	in_float = (float*) calloc ( w * h * z , sizeof(float) );
    if ( in_float == NULL )
        return -1;

    /* allocate 1 intermediate buffer for processing */
    Buffer = (float*) calloc ( w * h * z , sizeof(float) );
    if ( Buffer == NULL )
	{
		free (in_float);
        return -1;
	}

	/*convert to [0,1] float*/
	mig_im_util_conv_16u_32f ( Input, in_float, w * h * z );
	mig_im_util_mat2gray_32f ( in_float, w * h * z , 0.f , (float) MIG_MAX_16U );
	
	/* partitioning */
	z_part = (z / nparts);
	z_overlap = 2 * FastRadial->radii[FastRadial->num_radii-1];
	

	for ( ipart = 0; ipart < nparts; ++ipart )
	{
	
		/*handle rounding: add the needed elements to the last partition*/
		if (ipart == nparts -1 )
		{
			z_part = z_part + (z - nparts * z_part );
		}

		/* this should suffice for correct buffer handling */
		if (nparts > 1)
		{
			z_part_overlap = (ipart == 0 || ipart == nparts-1)?
				z_part + z_overlap :
				z_part + 2 * z_overlap;
		}
		else
		{
			z_part_overlap = z_part;
		}

		/* allocate 1 intermediate buffer for processing */
		BufferFR = (float*) calloc ( w * h * z_part_overlap , sizeof(float) );
		if ( BufferFR == NULL )
		{
			free ( Buffer );
			free ( in_float );
			return -1;

		}
		
		/* perform fast radial processing */

		
		if ( ipart == 0 )
		{
			rc = _radial_3d ( in_float ,
				  BufferFR , w , h , z_part_overlap ,
				  FastRadial->radii , FastRadial->num_radii, FastRadial->beta_threshold );
		}
		else 
		{
			rc = _radial_3d ( in_float + (w * h * ( partoffset - z_overlap) ) ,
				  BufferFR , w , h , z_part_overlap ,
				  FastRadial->radii , FastRadial->num_radii, FastRadial->beta_threshold );
			
		}

		if ( rc != 0 )
		{
			free ( Buffer );
			free ( BufferFR );
			free ( in_float );
			return -1;
		}

		/* copy to buffer */
		if ( ipart == 0 )
		{
			/*starts from 0 */
			memcpy ( Buffer, BufferFR ,
				(w * h * z_part) * sizeof (float) );
		}
		else
		{
			/*skip overlap */
			memcpy ( Buffer + w * h * ( partoffset ),
				BufferFR + w * h * z_overlap,
				(w * h * z_part) * sizeof (float) );
		}
		
		/*advance offset in z */
		partoffset += z_part;

		free ( BufferFR );

	}/* end partitioning */

    /* dump fast radial result if asked to */
    if ( FastRadial->dump == 1 )
    {
        snprintf ( fname , MAX_PATH , "%s_res" , FastRadial->prefix );
        rc = _dump_to_disk ( fname , Buffer , w , h , z );
    }

    if ( FastRadial->thr_type == PERCENT )
    {
        /* find maximum response of fast radial filter */
        _find_max ( Buffer , w * h * z , &MaxResponse );

        if ( MaxResponse == 0.0f )
        {
            free ( Buffer );
			free ( in_float );
            return 0;
        }

        thr = FastRadial->threshold * MaxResponse;
    }
    else
    {
        thr = FastRadial->threshold;
    }


	/* apply fixed threshold on fast radial responses */
	mig_im_thr_32f_i_val ( Buffer , w * h * z , thr);

	/*
	the threshold above is ok to remove unwanted values,
	but its output regions are too big (and this affects centroid calculation),
	so it's necessary to add local adaptive thresholding
	*/

	/* apply mean threshold */
	mig_im_thr_32f_3d_local_mean ( Buffer, w , h, z, THR_BOX_RADIUS );
	
    /* dump thresholded fast radial result if asked to */
    if ( FastRadial->dump == 1 )
    {
        snprintf ( fname , MAX_PATH , "%s_thr" , FastRadial->prefix );
        rc = _dump_to_disk ( fname , Buffer , w , h , z );
    }


    
	/* TODO: SLOW, we should consider shrinking */
	/* construct 3d regions from binarized volume */
	rc = mig_im_regc_3d ( Buffer , w , h , z , Regions );

	/* if we want to work on fr masks instead of centroids, 
		comment previous and uncomment next*/
	/*just create one region for each pixel "on" */
	/*rc = mig_im_regc_3d_odd ( Buffer, w, h , z , Regions );*/
    if ( rc != 0 )
    {
        free ( Buffer );
        free ( in_float );
		return -1;

    }

    free ( Buffer );
	free ( in_float );
    return 0;
}




/****************************************************************************/

int
mig_im_fradial_2d ( float *Input , int w , int h , mig_fradial_t *FastRadial , mig_lst_t *Regions )
{
        int rc = 0;
        float *Buffer;
        float MaxResponse = 0.0f;
        char fname[MAX_PATH];
        float thr;

        /* allocate intermediate buffer for processing */
        Buffer = (float*) calloc ( w * h , sizeof(float) );
        if ( Buffer == NULL )
                return -1;

        /* perform 1st fast radial processing */
        rc = _radial_2d ( Input , Buffer , w , h , FastRadial->radii , FastRadial->num_radii );
        if ( rc != 0 )
        {
                free ( Buffer );
                return -1;
        }

        /* perform 2nd fast radial processing */
        rc = _radial_2d ( Buffer, Input , w , h , FastRadial->radii , FastRadial->num_radii );
        if ( rc != 0 )
        {
                free ( Buffer );
                return -1;
        }

        /* dump fast radial result if asked to */
        if ( FastRadial->dump == 1 )
        {
            snprintf ( fname , MAX_PATH , "%s_res" , FastRadial->prefix );
            rc = _dump_to_disk ( fname , Input , w , h , 1 );
        }

        if ( FastRadial->thr_type == PERCENT )
        {
            /* find maximum response of fast radial filter */
            _find_max ( Input , w * h , &MaxResponse );

            if ( MaxResponse == 0.0f )
            {
                free ( Buffer );
                return 0;
            }

            thr = FastRadial->threshold * MaxResponse;
        }
        else
        {
            thr = FastRadial->threshold;
        }

        /* binarize fast radial responses */
        mig_im_thr_32f_i_val ( Input , w * h , thr );

        /* dump thresholded fast radial result if asked to */
        if ( FastRadial->dump == 1 )
        {
            snprintf ( fname , MAX_PATH , "%s_thr" , FastRadial->prefix );
            rc = _dump_to_disk ( fname , Input , w , h , 1 );
        }

        /* construct 3d regions from binarized volume */
        rc = mig_im_regc_2d ( Input , w , h , Regions );
        if ( rc != 0 )
        {
                free ( Buffer );
                return -1;
        }

        free ( Buffer );
        return 0;
}

/****************************************************************************/

void
mig_im_fradial_del ( mig_fradial_t *FastRadial )
{
        if ( FastRadial != NULL )
                free ( FastRadial );
}

/*
******************************************************************************
*               LOCAL PROTOTYPES IMPLEMENTATION
******************************************************************************
*/

static int
_radial_3d ( float *in ,
             float *out ,
             int w , int h , int z ,
             float *radii , int num_radii, float beta )
{
    /* matrix data */
    float *dx = NULL;       /* gradient horizontal direction */
    float *dy = NULL;       /* gradient vertical direction */
    float *dz = NULL;       /* gradient z direction */
    float *dmag = NULL;     /* gradient magnitude */

    float *o = NULL;        /* orientation projection image */
    float *IdxO;
    float *m = NULL;        /* magnitutde projection image */
    float *IdxM;
    float *f = NULL;
    float *s = NULL;        /* accumulator for all radii */

    /* other vars */
    float maxr;             /* max input radius */
    float *idx;
    int n;                  /* current radius */
    int i , j , k;          /* pixel indices */
	int dim;
	
	dim = w * h * z;

    dx = (float*) malloc ( dim * sizeof(float) );
    if ( dx == NULL )
        goto error;

    dy = (float*) malloc ( dim * sizeof(float) );
    if ( dy == NULL )
        goto error;

    dz = (float*) malloc ( dim * sizeof(float) );
    if ( dz == NULL )
        goto error;

    dmag = (float*) malloc ( dim * sizeof(float) );
    if ( dmag == NULL )
        goto error;

    /* calculate 3D gradient */
    //mig_im_drv_3d_central_diffs ( in , w , h , z , dx , dy , dz , dmag );
	
	mig_im_sobel_3d ( in, w, h, z, dx, dy, dz, dmag, beta );


    /* radial filter matrices */
    s = (float*) malloc ( num_radii * dim * sizeof( float ) );
    if ( s == NULL )
        goto error;

    f = (float*) malloc ( dim * sizeof(float) );
    if ( f == NULL )
        goto error;

    /* get maximum input radius and allocate space for orientation and
       magnitude images so that we avoid boundary condition checking */
    maxr = floorf( radii[num_radii-1] + 0.5f );

    o = (float*) calloc ( ( w + 2.0f * maxr ) *
                          ( h + 2.0f * maxr ) *
                          ( z + 2.0f * maxr ) ,
                          sizeof(float) );
    if ( o == NULL )
        goto error;

    m = (float*) calloc ( ( w + 2.0f * maxr ) *
                          ( h + 2.0f * maxr ) *
                          ( z + 2.0f * maxr ) ,
                          sizeof(float) );
    if ( m == NULL )
        goto error;

    /* position variables to actual matrix starting positions */
    IdxO = o + (int)( maxr + maxr * w + maxr * w * h );
    IdxM = m + (int)( maxr + maxr * w + maxr * w * h );

    /* for each radius */
    idx = s;
    for ( n = 0 ; n < num_radii ; ++n , idx += w * h * z )
    {
        _proj_3d ( dx , dy , dz , dmag , IdxO , IdxM , radii[n] , w , h , z );
        _f_3d ( IdxO , IdxM , f , radii[n] , w , h , z );

        mig_im_gauss_iir_3d ( f , idx , w , h , z , 0.25f * radii[n] );
    }

    /* final result */
    memset ( out , 0x00 , w * h * z * sizeof( float ) );

    for ( k = 0 ; k < z ; ++k )                             /* z coordinate */
    {
        for ( j = 0 ; j < h ; ++j )                     /* y coordinate */
        {
            for ( i = 0 ; i < w ; ++i , ++out )     /* x coordinate */
            {
                /* current voxel */
                idx = s + i + j * w + k * w * h;

                for ( n = 0 ; n < num_radii ; ++n )     /* radius coordinate */
                    *out += idx[n*w*h*z] * radii[n];

                *out = ( *out < MIG_EPS_32F ) ? 0.0f : (*out) / num_radii;
            }
        }
    }

    free ( dx );
    free ( dy );
    free ( dz );
    free ( dmag );

    free ( o );
    free ( m );
    free ( s );
    free ( f );

    return 0;

error :

    if ( dx )
        free ( dx );
    if ( dy )
        free ( dy );
    if ( dz )
        free ( dz );

    if ( dmag )
        free ( dmag );
    if ( o )
        free ( o );
    if ( s )
        free ( s );
    if ( m )
        free ( m );
    if ( f )
        free ( f );

    return -1;
}

/****************************************************************************/

static int
_radial_2d ( float *in , float *out , int w , int h , float *radii , int num_radii )
{
    /* matrix data */
    float *dx = NULL;       /* gradient horizontal direction */
    float *dy = NULL;       /* gradient vertical direction */
    float *dmag = NULL;     /* gradient magnitude */

    float *o = NULL;        /* orientation projection image */
    float *IdxO;
    float *m = NULL;        /* magnitutde projection image */
    float *IdxM;
    float *f = NULL;
    float *s = NULL;        /* accumulator for all radii */

    /* other vars */
    float maxr;             /* max input radius */
    float *idx;
    int n;                  /* current radius */
    int i , j;              /* pixel indices */

    dx = (float*) malloc ( w * h * sizeof(float) );
    if ( dx == NULL )
        goto error;

    dy = (float*) malloc ( w * h * sizeof(float) );
    if ( dy == NULL )
        goto error;

    dmag = (float*) malloc ( w * h * sizeof(float) );
    if ( dmag == NULL )
        goto error;

    /* calculate 2D gradient */
    mig_im_drv_2d_central_diffs ( in , w , h , dx , dy , dmag );

    /* radial filter matrices */
    s = (float*) malloc ( num_radii * w * h * sizeof( float ) );
    if ( s == NULL )
        goto error;

    f = (float*) malloc ( w * h * sizeof(float) );
    if ( f == NULL )
        goto error;

    /* get maximum input radius and
       allocate sapce for orientation and
       magnitude images so that we avoid
       boundary condition checking */
    maxr = floorf( radii[num_radii-1] + 0.5f );

    o = (float*) calloc ( ( w + 2.0f * maxr ) * ( h + 2.0f * maxr ) , sizeof(float) );
    if ( o == NULL )
        goto error;

    m = (float*) calloc ( ( w + 2.0f * maxr ) * ( h + 2.0f * maxr ) , sizeof(float) );
    if ( m == NULL )
        goto error;

    /* position variables to actual matrix starting positions */
    IdxO = o + (int)( maxr + maxr * w );
    IdxM = m + (int)( maxr + maxr * w );

    /* for each radius */
    idx = s;
    for ( n = 0 ; n < num_radii ; ++n , idx += w * h )
    {
        _proj_2d ( dx , dy , dmag , IdxO , IdxM , radii[n] , w , h );
        _f_2d ( IdxO , IdxM , f , radii[n] , w , h );

        mig_im_gauss_iir_2d ( f , idx , w , h , 0.25f * radii[n] );
    }

    /* final result */
    memset ( out , 0x00 , w * h * sizeof( float ) );

    for ( j = 0 ; j < h ; ++j )                     /* y coordinate */
    {
        for ( i = 0 ; i < w ; ++i , ++out )         /* x coordinate */
        {
            /* current pixel */
            idx = s + i + j * w;

            for ( n = 0 ; n < num_radii ; ++n )     /* radius coordinate */
                *out += idx[n*w*h] * radii[n];
            
            *out = ( *out < MIG_EPS_32F ) ? 0.0f : (*out) / num_radii;
        }
    }
        
    free ( dx );
    free ( dy );
    free ( dmag );

    free ( o );
    free ( m );
    free ( s );
    free ( f );

    return 0;

error :

    if ( dx )
        free ( dx );
    if ( dy )
        free ( dy );
    if ( dmag )
        free ( dmag );
    if ( o )
        free ( o );
    if ( s )
        free ( s );
    if ( m )
        free ( m );
    if ( f )
        free ( f );

    return -1;
}

/****************************************************************************/

static void
_proj_3d ( float *dx , float *dy , float *dz , float *dmag ,
           float *o , float *m , float radius ,
           int w , int h , int z )
{
    int i , j , k;         /* counters */
    int x0 , y0 , z0;      /* affected pixel coordinates */

    for ( k = 0 ; k < z ; ++k )
    {
        for ( j = 0 ; j < h ; ++j )
        {
            for ( i = 0 ; i < w ; ++i , ++dx , ++dy , ++dz , ++dmag )
            {
                /* affected pixel coordinates */
                x0 = (int) floorf ( *dx * radius + 0.5f );
                y0 = (int) floorf ( *dy * radius + 0.5f );
                z0 = (int) floorf ( *dz * radius + 0.5f );

                o[(i+x0)+(j+y0)*w+(k+z0)*w*h] += 1.0f;
                o[(i-x0)+(j-y0)*w+(k-z0)*w*h] -= 1.0f;

                m[(i+x0)+(j+y0)*w+(k+z0)*w*h] += *dmag;
                m[(i-x0)+(j-y0)*w+(k-z0)*w*h] -= *dmag;
            }
        }
    }
}

/****************************************************************************/

static void
_proj_2d ( float *dx , float *dy , float *dmag ,
           float *o , float *m , float radius ,
           int w , int h )
{
    int i , j;     /* counters */
    int x0 , y0;   /* affected pixel coordinates */

    for ( j = 0 ; j < h ; ++j )
    {
        for ( i = 0 ; i < w ; ++i , ++dx , ++dy , ++dmag )
        {
            /* affected pixel coordinates */
            x0 = (int) floorf ( *dx * radius + 0.5f );
            y0 = (int) floorf ( *dy * radius + 0.5f );
            
            o[(i+x0)+(j+y0)*w] += 1.0f;
            o[(i-x0)+(j-y0)*w] -= 1.0f;

            m[(i+x0)+(j+y0)*w] += *dmag;
            m[(i-x0)+(j-y0)*w] -= *dmag;
        }
    }
}

/****************************************************************************/

static void
_f_3d ( float *o , float *m , float *f ,
        float radius , int w , int h , int z )
{
        int i , j , k;          /* voxel counter */
        float norm;             /* normalization factor */
        float *OVoxel;
        float *MVoxel;

        norm = ( radius == 1.0f ) ? 8.0f : 9.9f;

        for ( k = 0 ; k < z ; ++k )
        {
                for ( j = 0 ; j < h ; ++j )
                {
                        for ( i = 0 ; i < w ; ++i , ++f )
                        {
                                OVoxel = o + i + j * w + k * w * h;
                                MVoxel = m + i + j * w + k * w * h;

                                *OVoxel = ( (*OVoxel) > norm  ) ? (norm)  : (*OVoxel);
                                *OVoxel = ( (*OVoxel) < -norm ) ? (-norm) : (*OVoxel);
                                *f = ((*MVoxel) / norm ) * MIG_POW2( ( fabsf(*OVoxel) / norm ) );
                        }
                }
        }
}

/****************************************************************************/

static void
_f_2d ( float *o , float *m , float *f ,
        float radius , int w , int h )
{
    int i , j;          /* voxel counter */
    float norm;         /* normalization factor */
    float *OPixel;
    float *MPixel;

    norm = ( radius == 1.0f ) ? 8.0f : 9.9f;
    
    for ( j = 0 ; j < h ; ++j )
    {
        for ( i = 0 ; i < w ; ++i , ++f )
        {
            OPixel = o + i + j * w;
            MPixel = m + i + j * w;

            *OPixel = ( (*OPixel) > norm  ) ? (norm)  : (*OPixel);
            *OPixel = ( (*OPixel) < -norm ) ? (-norm) : (*OPixel);
            *f = ((*MPixel) / norm ) * MIG_POW2( ( fabsf(*OPixel) / norm ) );
        }
    }    
}

/****************************************************************************/

static void
_find_max ( float *Input , int InputLen , float *MaxVal )
{
        int i;
        *MaxVal = 0.0f;

        for ( i = 0 ; i < InputLen ; ++i , ++Input )
                *MaxVal = ( *Input > *MaxVal ) ? *Input : *MaxVal;
}

/****************************************************************************/

static int
_dump_to_disk ( const char *name , 
                const float *data ,
                int w , int h , int z )
{
    int fid = -1 , len = -1;
    unsigned int size = w * h * z * sizeof(float);

    
    fid = mig_open ( name , MIG_O_BINARY | 
                            MIG_O_WRONLY | 
                            MIG_O_CREAT | 
                            MIG_O_TRUNC );
	if ( fid == -1 )
        return -1;

    /* write sizes */
    len = mig_write ( fid , (const void*) &w , sizeof(w) );
    if ( len != sizeof(w) )
        goto error;

    len = mig_write ( fid , (const void*) &h , sizeof(h) );
    if ( len != sizeof(h) )
        goto error;

    len = mig_write ( fid , (const void*) &z , sizeof(z) );
    if ( len != sizeof(z) )
        goto error;

    /* write data */
    len = mig_write ( fid , (const void*) data , size );
    if ( len != size )
        goto error;

    /* close */
    mig_close ( fid );
    return 0;

error :

    if ( fid > 0 )
        mig_close ( fid );

    return -1;
}

/****************************************************/
/* MATLAB */
/****************************************************/
#if defined(MATLAB_RADIAL)


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
        float *Output = NULL;
        int    NumRadii;
        float *Radii;

		double *tempRadii;

		int Type; /* 0 = 1st pass; 1 = 1st+second; 2 = Thresholded NOT IMPLEMENTED */
		
		float thr;
		float thrParam;

        int Width , Height;
        int rc = 0;

        /* input image */
        Input  = (double*) mxGetPr( prhs[0] );
        Height = (int) mxGetM ( prhs[0] );
        Width  = (int) mxGetN ( prhs[0] );

        /* radii array */
        tempRadii = (double*)
                mxGetPr( prhs[1] );
        NumRadii = (int) mxGetN ( prhs[1] );
		
		Radii = (float*)
                calloc ( NumRadii , sizeof(float) );
		
		_cpy_dbl2flt ( tempRadii , Radii , NumRadii );

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

        /* Create Output */
        plhs[0] = mxCreateNumericMatrix ( Height , Width , mxSINGLE_CLASS , mxREAL );
        if ( plhs[0] == NULL )
                mexErrMsgTxt ( "\nMemory Error...\n" );
        Output = (float*)
                mxGetPr( plhs[0] );

		_cpy_dbl2flt ( Input , TmpBuffer2 , Height * Width );

        /* swap input to row major order */
        _swap ( TmpBuffer2 , TmpBuffer1 , Height , Width );

        /* fast radial 1 pass */
        rc = _radial_2d ( TmpBuffer1 , TmpBuffer2 , Width , Height , Radii , NumRadii );
        if ( rc != 0 )
                mexErrMsgTxt ("Fast Radial Error : 1 pass...\n");
		
		if ( Type == 0 ){
			/* swap fast radial output to column major order */
			_swap ( TmpBuffer2 , Output , Width , Height );
			//_cpy_flt2dbl ( TmpBuffer1 , Output , Width * Height);
		} else if ( Type > 0 ){


			/* fast radial 2 pass */
			rc = _radial_2d ( TmpBuffer2 , TmpBuffer1 , Width , Height , Radii , NumRadii );
			if ( rc != 0 )
                mexErrMsgTxt ("Fast Radial Error : 2 pass...\n");
			
			if ( Type == 1){
				/* swap fast radial output to column major order */
				_swap ( TmpBuffer1 , Output , Width , Height );
			} else if ( Type > 1 ){
				
				/* threshold value */
				thrParam = mxGetScalar ( prhs[3] );

				if ( Type == 2 ){

					float maxResp = 0;
					/* find maximum response of fast radial filter */
					_find_max ( TmpBuffer1 , Width * Height , &maxResp );
				
					thr = thrParam * maxResp ;

				} else if ( Type == 3 ){
					thr = thrParam ;
				}
				/* binarize fast radial responses */
				/* mig_im_thr_32f_i_val ( Input , w * h , ( FastRadial->threshold ) * MaxResponse ); */
				mig_im_thr_32f_i_val ( TmpBuffer1 , Width * Height , thr);
				

				/* construct 3d regions from binarized volume */
				//rc = mig_im_regc_2d ( Input , w , h , Regions );
			
				/* swap fast radial output to column major order */
				_swap ( TmpBuffer1 , Output , Width , Height );
			}

		}
        /* free resources */
        free ( TmpBuffer1 );
        free ( TmpBuffer2 );
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

#endif /* MATLAB_RADIAL */


#undef FR_NPARTS

