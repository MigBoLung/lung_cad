#include "mig_io_mat.h"
#include "mig_ut_mem.h"
#include "mig_ut_bit.h"

#include "mig_error_codes.h"

#if defined( MATLAB )

/*************************************************/
/* name of mat variables */

#define CT_NAME          "CT"
#define MASKS_NAME       "MASKS"
#define SUPP_NAME        "SUPP"
#define BB_NAME          "BB"
#define SUPP_ORIG_NAME   "SUPP_ORIG"

/*************************************************/
/* image characteristics */

#define SUPP_WIDTH               "width"
#define SUPP_HEIGHT              "height"
#define SUPP_SLICES              "n"
#define SUPP_PIXEL_SPACING       "PixelSpacing"
#define SUPP_SLICE_SPACING       "SliceSpacing"
#define SUPP_SLICE_THICKNESS     "SliceThickness"
#define SUPP_CONVOLUTION_KERNEL  "ConvolutionKernel"

/*************************************************/
/* original image characteristics */

#define SUPP_ORIG_WIDTH               "width"
#define SUPP_ORIG_HEIGHT              "height"
#define SUPP_ORIG_SLICES              "n"

/*************************************************/
/* dicom characteristics */

#define SUPP_PATIENT_ID          "PatientID"
#define SUPP_PATIENT_NAME        "PatientName"
#define SUPP_STUDY_UID           "StudyUID"
#define SUPP_SERIES_UID          "SeriesUID"

/* total number of fields inside SUPP structure */
#define SUPP_FIELDS 11

/* total number of fields inside SUPP_ORIG structure */
#define SUPP_ORIG_FIELDS 3

/*************************************************/
const char
*_supp_field_names[] =
{
	SUPP_WIDTH ,
	SUPP_HEIGHT ,
	SUPP_SLICES ,
	SUPP_PIXEL_SPACING ,
	SUPP_SLICE_SPACING ,
	SUPP_SLICE_THICKNESS ,
	SUPP_CONVOLUTION_KERNEL ,
	SUPP_PATIENT_ID ,
	SUPP_PATIENT_NAME ,
	SUPP_STUDY_UID ,
	SUPP_SERIES_UID
};

/*************************************************/
const char
*_supp_orig_field_names[] =
{
	SUPP_ORIG_WIDTH ,
	SUPP_ORIG_HEIGHT ,
	SUPP_ORIG_SLICES
};

/*************************************************/
/* bounding box fields */

#define BB_X0   "x0"
#define BB_Y0   "y0"
#define BB_Z0   "z0"
#define BB_X1   "x1"
#define BB_Y1   "y1"
#define BB_Z1   "z1"
#define BB_W    "w"
#define BB_H    "h"
#define BB_Z    "z"

/* total number of fields inside BB structure */
#define BB_FIELDS 9

/*************************************************/
const char
*_bb_field_names[] =
{
        BB_X0 ,
        BB_Y0 ,
        BB_Z0 ,
        BB_X1 ,
        BB_Y1 ,
        BB_Z1 ,
        BB_W ,
        BB_H ,
        BB_Z
};

/*************************************************/
/* LOCAL FUNCTIONS */
/*************************************************/
static int
_write_supp ( MATFile *f ,
              mig_dcm_data_t *info ,
              mig_size_t *s );

static int
_write_supp_orig ( MATFile *f ,
                   mig_size_t *s );

static int
_write_bb ( MATFile *f ,
            mig_roi_t *roi );

static void
_swap_32f ( float *in ,
            float *out ,
            int x , int y , int z );

static void
_swap_16u ( Mig16u *in ,
            Mig16u *out ,
            int x , int y , int z );

static void
_swap_8u ( Mig8u *in ,
           Mig8u *out ,
           int x , int y , int z );

static void
_msk_pack ( Mig8u *in , int s );

static void
_msk_upack ( Mig8u *in , int s );

/*************************************************/
/* EXPORTS */
/*************************************************/

int
mig_io_mat_w_ct ( char *mat_name ,
                  Mig16u *src ,
                  mig_dcm_data_t *info ,
                  mig_size_t *s ,
                  mig_size_t *orig ,
                  mig_roi_t *roi )
{
    MATFile	*f = NULL;
    int	ret;
    mxArray *MatData = NULL;
    Mig16u *Dest;
    mwSize Dims[3];

	/* open matlab file for writing compressed data */
    f = matOpen ( mat_name , "wz" );
    if ( f == NULL )
    {
        ret = MIG_ERROR_IO;
        goto error;
    }

    /* write SUPP information first */
    ret = _write_supp ( f , info , s );
    if ( ret != MIG_OK )
        goto error;

    /* write SUPP_ORIG information first */
    ret = _write_supp_orig ( f , orig );
    if ( ret != MIG_OK )
        goto error;

    /* if present write bounding box information */
    if ( roi != NULL )
    {
        ret = _write_bb ( f , roi );
        if ( ret != MIG_OK )
            goto error;
    }

    /* prepare data */
    Dims[0] = s->h;
    Dims[1] = s->w;
    Dims[2] = s->slices;

	/* create space for ct data */
    MatData = mxCreateNumericArray ( 3 , (const mwSize*) &Dims , mxUINT16_CLASS , mxREAL);
    if ( MatData == NULL )
    {
        ret = MIG_ERROR_MEMORY;
        goto error;
    }

    Dest = (Mig16u*) mxGetPr ( MatData );
    if ( Dest == NULL )
    {
        ret = MIG_ERROR_INTERNAL;
        goto error;
    }

    /* flip rows / columns for matlab */
    _swap_16u ( src , Dest , s->w , s->h , s->slices );

    /* write data to mat variable */
    if ( matPutVariable ( f , CT_NAME , MatData ) )
    {
        ret = MIG_ERROR_INTERNAL;
        goto error;
    }

    matClose ( f );
    mxDestroyArray ( MatData );
    return MIG_OK;

error:

    if ( f )
        matClose ( f );

    if ( MatData )
        mxDestroyArray ( MatData );

    return ret;
}

/*************************************************/

int
mig_io_mat_w_float ( char *mat_name ,
                     float *src ,
                     mig_dcm_data_t *info ,
                     mig_size_t *s ,
                     mig_size_t *orig ,
                     mig_roi_t *roi )
{
    MATFile	*f = NULL;
    int	ret;
    mxArray *MatData = NULL;
    float *Dest;
    mwSize Dims[3];

	/* open matlab file for writing compressed data */
    f = matOpen ( mat_name , "wz" );
    if ( f == NULL )
    {
        ret = MIG_ERROR_IO;
        goto error;
    }

    /* write SUPP information first */
    ret = _write_supp ( f , info , s );
    if ( ret != MIG_OK )
        goto error;

    /* write SUPP_ORIG information */
    ret = _write_supp_orig ( f , orig );
    if ( ret != MIG_OK )
        goto error;

    /* if present write bounding box information */
    if ( roi != NULL )
    {
        ret = _write_bb ( f , roi );
        if ( ret != MIG_OK )
            goto error;
    }

    /* prepare data */
    Dims[0] = s->h;
    Dims[1] = s->w;
    Dims[2] = s->slices;

	/* create space for ct data */
    MatData = mxCreateNumericArray ( 3 , (const mwSize*) &Dims , mxSINGLE_CLASS , mxREAL );
    if ( MatData == NULL )
    {
        ret = MIG_ERROR_MEMORY;
        goto error;
    }

    Dest = (float*) mxGetPr ( MatData );
    if ( Dest == NULL )
    {
        ret = MIG_ERROR_INTERNAL;
        goto error;
    }

    /* flip rows / columns for matlab */
    _swap_32f ( src , Dest , s->w , s->h , s->slices );

    /* write data to mat variable */
    if ( matPutVariable ( f , CT_NAME , MatData ) )
    {
        ret = MIG_ERROR_INTERNAL;
        goto error;
    }

    matClose ( f );
    mxDestroyArray ( MatData );
    return MIG_OK;

error:

    if ( f )
        matClose ( f );

    if ( MatData )
        mxDestroyArray ( MatData );

    return ret;
}

/*************************************************/

int
mig_io_mat_w_masks ( char *mat_name ,
                     Mig8u *src ,
                     mig_dcm_data_t *info ,
                     mig_size_t *s ,
                     mig_size_t *orig ,
                     mig_roi_t *roi )
{
        MATFile *f = NULL;
        int ret;
        mxArray *MatData = NULL;
        mwSize Dims[3];
        Mig8u *Dest = NULL;

        /* open mat file for writing
           compressed data */
        f = matOpen ( mat_name , "wz" );
        if ( f == NULL )
        {
                ret = MIG_ERROR_IO;
                goto error;
        }

        /* first write SUPP data */
        ret = _write_supp ( f , info , s );
        if ( ret != MIG_OK )
                goto error;

        /* write SUPP_ORIG data */
        ret = _write_supp_orig ( f , orig );
        if ( ret != MIG_OK )
                goto error;

        /* if present write bounding box information */
        if ( roi != NULL )
        {
                ret = _write_bb ( f , roi );
                if ( ret != MIG_OK )
                        goto error;
        }

        /* prepare data */
        Dims[0] = s->h;
        Dims[1] = s->w;
        Dims[2] = s->slices;

	/* make space for masks data */
        MatData =
        	mxCreateNumericArray ( 3 , (const mwSize*) &Dims ,
        		mxLOGICAL_CLASS , mxREAL );
        if ( MatData == NULL )
        {
                ret = MIG_ERROR_MEMORY;
                goto error;
        }

        Dest = (Mig8u*) mxGetPr( MatData );
        if ( Dest == NULL )
        {
                ret = MIG_ERROR_INTERNAL;
                goto error;
        }

	/* flip rows / columns -> MATLAB is column major order */
        _swap_8u ( src , Dest , s->w , s->h , s->slices );

        /* make sure every masks pixel value is
           either 0x00 or 0x01 */
        _msk_pack ( Dest , s->dim_stack );

        /* put data */
        if ( matPutVariable ( f , MASKS_NAME , MatData ) )
        {
                ret = MIG_ERROR_INTERNAL;
                goto error;
        }

        matClose ( f );
        mxDestroyArray ( MatData );
                return MIG_OK;

error:

        if ( f )
                matClose ( f );

        if ( MatData )
                mxDestroyArray ( MatData );

        return ret;
}

/*************************************************/
/* PRIVATE FUNCTIONS */
/*************************************************/

static int
_write_supp ( MATFile *f ,
              mig_dcm_data_t *info ,
              mig_size_t *s )
{
        int ret = MIG_OK;

        mxArray *MatField;      /* Field in MatStruct */
        mxArray *MatStruct;     /* SUPP structure */

        double *PixelSpacing;   /* PixelSpacing array */

        /* create SUPP structure */
        MatStruct = mxCreateStructMatrix ( 1 , 1 , SUPP_FIELDS , _supp_field_names );
        if ( MatStruct == NULL )
        {
                ret = MIG_ERROR_MEMORY;
                goto error;
        }

        /* fill in SUPP values */

        /* width */
        MatField = mxCreateDoubleScalar ( s->w );
        if ( MatField == NULL )
        {
                ret = MIG_ERROR_INTERNAL;
                goto error;
        }
        mxSetField ( MatStruct , 0 , 
                SUPP_WIDTH , MatField );

        /* height */
        MatField = mxCreateDoubleScalar ( s->h );
        if ( MatField == NULL )
        {
                ret = MIG_ERROR_INTERNAL;
                goto error;
        }
        mxSetField ( MatStruct , 0 , 
                SUPP_HEIGHT , MatField );

        /* num slices */
        MatField = mxCreateDoubleScalar ( s->slices );
        if ( MatField == NULL )
        {
                ret = MIG_ERROR_INTERNAL;
                goto error;
        }
        mxSetField ( MatStruct , 0 , 
                SUPP_SLICES , MatField );

        /* pixel spacing */
        MatField = mxCreateDoubleMatrix ( 2 , 1 , mxREAL );
        if ( MatField == NULL )
        {
                ret = MIG_ERROR_INTERNAL;
                goto error;
        }

        PixelSpacing = mxGetPr ( MatField );
        PixelSpacing[0] = s->h_res;
        PixelSpacing[1] = s->v_res;
        mxSetField ( MatStruct , 0 , 
                SUPP_PIXEL_SPACING , MatField );

        /* slice spacing */
        MatField = mxCreateDoubleScalar ( s->z_res );
        if ( MatField == NULL )
        {
                ret = MIG_ERROR_INTERNAL;
                goto error;
        }
        mxSetField ( MatStruct , 0 , 
                SUPP_SLICE_SPACING , MatField );

        /* slice thickness */
        MatField = mxCreateDoubleScalar ( s->thickness );
        if ( MatField == NULL )
        {
                ret = MIG_ERROR_INTERNAL;
                goto error;
        }
        mxSetField ( MatStruct , 0 , 
                SUPP_SLICE_THICKNESS , MatField );

        /* patient id */
        MatField = mxCreateString ( info->patient_id );
        if ( MatField == NULL )
        {
                ret = MIG_ERROR_INTERNAL;
                goto error;
        }
        mxSetField ( MatStruct , 0 , 
                SUPP_PATIENT_ID , MatField );

        /* patient name */
        MatField = mxCreateString ( info->patient_name );
        if ( MatField == NULL )
        {
        	ret = MIG_ERROR_INTERNAL;
                goto error;
        }
        mxSetField ( MatStruct , 0 , 
                SUPP_PATIENT_NAME , MatField );

        /* study uid */
        MatField = mxCreateString ( info->study_uid );
        if ( MatField == NULL )
        {
                ret = MIG_ERROR_INTERNAL;
                goto error;
        }
        mxSetField ( MatStruct , 0 , 
                SUPP_STUDY_UID , MatField );

        /* series uid */
        MatField = mxCreateString ( info->series_uid );
        if ( MatField == NULL )
        {
        	ret = MIG_ERROR_INTERNAL;
                goto error;
        }
        mxSetField ( MatStruct , 0 , 
                SUPP_SERIES_UID , MatField );

        /* convolution kernel */
        MatField = mxCreateString ( "UNKNOWN" );
        if ( MatField == NULL )
        {
        	ret = MIG_ERROR_INTERNAL;
                goto error;
        }
        mxSetField ( MatStruct , 0 ,
                SUPP_CONVOLUTION_KERNEL , MatField );

        /* put SUPP variable into matlab mat file */
        if ( matPutVariable ( f , SUPP_NAME , MatStruct ) )
        {
                ret = MIG_ERROR_INTERNAL;
                goto error;
        }

        mxDestroyArray ( MatStruct );        
        return ret;

error :

        if ( MatStruct )
                mxDestroyArray ( MatStruct );

        return ret;
}

/*************************************************/
static int
_write_supp_orig ( MATFile *f , mig_size_t *s )
{
    int ret = MIG_OK;

    mxArray *MatField;      /* Field in MatStruct */
    mxArray *MatStruct;     /* SUPP_ORIG structure */

    /* create SUPP_ORIG structure */
    MatStruct = mxCreateStructMatrix ( 1 , 1 , SUPP_ORIG_FIELDS , _supp_orig_field_names );
    if ( MatStruct == NULL )
    {
        ret = MIG_ERROR_MEMORY;
        goto error;
    }

    /* fill in SUPP_ORIG values */

    /* width */
    MatField = mxCreateDoubleScalar ( s->w );
    if ( MatField == NULL )
    {
        ret = MIG_ERROR_INTERNAL;
        goto error;
    }
    mxSetField ( MatStruct , 0 , SUPP_ORIG_WIDTH , MatField );

    /* height */
    MatField = mxCreateDoubleScalar ( s->h );
    if ( MatField == NULL )
    {
        ret = MIG_ERROR_INTERNAL;
        goto error;
    }
    mxSetField ( MatStruct , 0 , SUPP_ORIG_HEIGHT , MatField );

    /* num slices */
    MatField = mxCreateDoubleScalar ( s->slices );
    if ( MatField == NULL )
    {
        ret = MIG_ERROR_INTERNAL;
        goto error;
    }
    mxSetField ( MatStruct , 0 , SUPP_ORIG_SLICES , MatField );

    /* put SUPP_ORIG variable into matlab mat file */
    if ( matPutVariable ( f , SUPP_ORIG_NAME , MatStruct ) )
    {
        ret = MIG_ERROR_INTERNAL;
        goto error;
    }

    mxDestroyArray ( MatStruct );        
    return ret;

error :

    if ( MatStruct )
        mxDestroyArray ( MatStruct );

    return ret;
}

/*************************************************/
static int
_write_bb ( MATFile *f , mig_roi_t *roi )
{
        int ret = MIG_OK;

        mxArray *MatField = NULL;	/* single field in MatStruct */
        mxArray *MatStruct;		/* BB structure */

        /* create BB structure */
        MatStruct = mxCreateStructMatrix ( 1 , 1 , BB_FIELDS , _bb_field_names );
        if ( MatStruct == NULL )
        {
                ret = MIG_ERROR_MEMORY;
                goto error;
        }

        /* fill in BB values */

        /* x0 */
        MatField = mxCreateDoubleScalar ( roi->x0 );
        if ( MatField == NULL )
        {
                ret = MIG_ERROR_INTERNAL;
                goto error;
        }
        mxSetField ( MatStruct , 0 , BB_X0 , MatField );

        /* y0 */
        MatField = mxCreateDoubleScalar ( roi->y0 );
        if ( MatField == NULL )
        {
                ret = MIG_ERROR_INTERNAL;
                goto error;
        }
        mxSetField ( MatStruct , 0 , BB_Y0 , MatField );

        /* z0 */
        MatField = mxCreateDoubleScalar ( roi->z0 );
        if ( MatField == NULL )
        {
                ret = MIG_ERROR_INTERNAL;
                goto error;
        }
        mxSetField ( MatStruct , 0 , BB_Z0 , MatField );

        /* x1 */
        MatField = mxCreateDoubleScalar ( roi->x1 );
        if ( MatField == NULL )
        {
                ret = MIG_ERROR_INTERNAL;
                goto error;
        }
        mxSetField ( MatStruct , 0 , BB_X1 , MatField );

        /* y1 */
        MatField = mxCreateDoubleScalar ( roi->y1 );
        if ( MatField == NULL )
        {
                ret = MIG_ERROR_INTERNAL;
                goto error;
        }
        mxSetField ( MatStruct , 0 , BB_Y1 , MatField );

        /* z1 */
        MatField = mxCreateDoubleScalar ( roi->z1 );
        if ( MatField == NULL )
        {
                ret = MIG_ERROR_INTERNAL;
                goto error;
        }
        mxSetField ( MatStruct , 0 , BB_Z1 , MatField );

        /* w */
        MatField = mxCreateDoubleScalar ( roi->w );
        if ( MatField == NULL )
        {
                ret = MIG_ERROR_INTERNAL;
                goto error;
        }
        mxSetField ( MatStruct , 0 , BB_W , MatField );

        /* h */
        MatField = mxCreateDoubleScalar ( roi->h );
        if ( MatField == NULL )
        {
                ret = MIG_ERROR_INTERNAL;
                goto error;
        }
        mxSetField ( MatStruct , 0 , BB_H , MatField );

        /* z */
        MatField = mxCreateDoubleScalar ( roi->z );
        if ( MatField == NULL )
        {
                ret = MIG_ERROR_INTERNAL;
                goto error;
        }
        mxSetField ( MatStruct , 0 , BB_Z , MatField );
        
        /* put BB variable into matlab mat file */
        if ( matPutVariable ( f , BB_NAME , MatStruct ) )
        {
                ret = MIG_ERROR_INTERNAL;
                goto error;
        }

        mxDestroyArray ( MatStruct );
        return ret;

error :

        if ( MatStruct )
                mxDestroyArray ( MatStruct );
        return ret;
}

/*************************************************/
static void
_swap_32f ( float *in ,
            float *out ,
            int x , int y , int z )
{
        int i , j , k;

        for ( k = 0 ; k < z ; ++ k , in += ( x * y ) , out += ( x * y ) )
                for ( j = 0 ; j < y ; ++ j )
                        for ( i = 0 ; i < x ; ++ i )
                                *( out + j + i * y ) = *( in + i + j * x );
}

/*************************************************/
static void
_swap_16u ( Mig16u *in ,
            Mig16u *out ,
            int x , int y , int z )
{
        int i , j , k;

        for ( k = 0 ; k < z ; ++ k , in += ( x * y ) , out += ( x * y ) )
                for ( j = 0 ; j < y ; ++ j )
                        for ( i = 0 ; i < x ; ++ i )
                                *( out + j + i * y ) = *( in + i + j * x );
}

/*************************************************/
static void
_swap_8u ( Mig8u *in ,
           Mig8u *out ,
           int x , int y , int z )
{
        int i , j , k;

        for ( k = 0 ; k < z ; ++ k , in += ( x * y ) , out += ( x * y ) )
                for ( j = 0 ; j < y ; ++ j )
                        for ( i = 0 ; i < x ; ++ i )
                                *( out + j + i * y ) = *( in + i + j * x );
}

/*************************************************/
static void
_msk_pack ( Mig8u *in , int s )
{
        int i;
        for ( i = 0 ; i < s ; ++ i , ++ in )
                *in &= 0x01;
}

/*************************************************/
static void
_msk_upack ( Mig8u *in , int s )
{
        int i;
        for ( i = 0 ; i < s ; ++ i , ++ in )
                *in = ( *in ) ? 0xFF : 0x00;
}

#endif /* MATLAB */

