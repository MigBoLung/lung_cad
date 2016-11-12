/*
******************************************************************************
*
* Author      : Todor Petkov
* Filename    : mig_drv_3d.c
* Created     : 2007/06/21
* Description : 3d first order derivative operators
*
******************************************************************************
*/

#include "mig_im_drv.h"


/*
*****************************************************************************
* STATIC FUNCTIONS DECLARATIONS
*****************************************************************************
*/

/* TODO: we could use mig_im_conv instead */
static void _conv_sobel_3d ( float *in, float *out, int w, int h, int z, float *ker );


/*
*****************************************************************************
* PRIVATE VARIABLE DEFINITIONS
*****************************************************************************
*/


/*
****************************************************************************
* SOBEL KERNELS
*****************************************************************************
*/
static float _so_3d_x[] = {
    -0.03125f, 0.0f, 0.03125f,
    -0.06250f, 0.0f, 0.06250f,
    -0.03125f, 0.0f, 0.03125f,

    -0.06250f, 0.0f, 0.06250f,
    -0.12500f, 0.0f, 0.12500f,
    -0.06250f, 0.0f, 0.06250f,

    -0.03125f, 0.0f, 0.03125f,
    -0.06250f, 0.0f, 0.06250f,
    -0.03125f, 0.0f, 0.03125f
};

static float _so_3d_y[] = {
    -0.03125f, -0.06250f, -0.03125f,
    0.00000f, 0.00000f, 0.00000f,
    0.03125f, 0.06250f, 0.03125f,

    -0.06250f, -0.125f, -0.06250f,
    0.00000f, 0.000f, 0.00000f,
    0.06250f, 0.125f, 0.06250f,

    -0.03125f, -0.06250f, 0.03125f,
    0.00000f, 0.00000f, 0.00000f,
    0.03125f, 0.06250f, 0.03125f
};

static float _so_3d_z[] = {
    -0.03125f, -0.06250f, -0.03125f,
    -0.06250f, -0.12500f, -0.06250f,
    -0.03125f, -0.06250f, -0.03125f,

    0.00000f, 0.00000f, 0.00000f,
    0.00000f, 0.00000f, 0.00000f,
    0.00000f, 0.00000f, 0.00000f,

    0.03125f, 0.06250f, 0.03125f,
    0.06250f, 0.12500f, 0.06250f,
    0.03125f, 0.06250f, 0.03125f
};


/*
******************************************************************************
*               GLOBAL PROTOTYPES IMPLEMENTATION
******************************************************************************
*/

void
mig_im_sobel_3d ( float *data, int w, int h, int z, float *dx, float *dy, float *dz, float *dmag, float thr )
{
    int i;

    /* zero all output images */
    memset ( dx, 0x00, w * h * z * sizeof ( float ) );
    memset ( dy, 0x00, w * h * z * sizeof ( float ) );
    memset ( dz, 0x00, w * h * z * sizeof ( float ) );
    memset ( dmag, 0x00, w * h * z * sizeof ( float ) );

    /* dx */
    _conv_sobel_3d ( data, dx, w, h, z, _so_3d_x );

    /* dy */
    _conv_sobel_3d ( data, dy, w, h, z, _so_3d_y );

    /* dz */
    _conv_sobel_3d ( data, dz, w, h, z, _so_3d_z );

    for ( i = 0; i < w * h * z; ++i, ++dx, ++dy, ++dz, ++dmag )
    {
        *dmag = sqrtf ( MIG_POW2 ( *dx ) + MIG_POW2 ( *dy ) + MIG_POW2 ( *dz ) );

        if ( *dmag > thr )
        {
            *dx /= *dmag;
            *dy /= *dmag;
            *dz /= *dmag;
        }
        else
        {
            *dmag = 0.0f;
            *dx = 0.0f;
            *dy = 0.0f;
            *dz = 0.0f;
        }
    }
}


void
mig_im_drv_3d_central_diffs ( unsigned short *data ,
                              int w , int h , int z ,
                              float *dx , float *dy , float *dz , float *dmag )
{
        int i , j , k;
        unsigned short *VoxelData;
        float *VoxelDx;
        float *VoxelDy;
        float *VoxelDz;
        float *VoxelMag;

        /* zero all output images */
        memset ( dx   , 0x00 , w * h * z * sizeof(float) );
        memset ( dy   , 0x00 , w * h * z * sizeof(float) );
        memset ( dz   , 0x00 , w * h * z * sizeof(float) );
        memset ( dmag , 0x00 , w * h * z * sizeof(float) );

        for ( k = 1 ; k < z - 1 ; ++k )                 /* z */
                for ( j = 1 ; j < h - 1 ; ++j )         /* y */
                        for ( i = 1 ; i < w - 1 ; ++i ) /* x */
                        {
                                /* get pointers to images */
                                VoxelData = data + i + j * w + k * w * h;
                                VoxelDx   = dx   + i + j * w + k * w * h;
                                VoxelDy   = dy   + i + j * w + k * w * h;
                                VoxelDz   = dz   + i + j * w + k * w * h;
                                VoxelMag  = dmag + i + j * w + k * w * h;

                                /* calculate central differences */
                                *VoxelDx = 0.5f * ( (float)*( VoxelData + 1   ) - (float)*( VoxelData - 1 ) );
                                *VoxelDy = 0.5f * ( (float)*( VoxelData + w   ) - (float)*( VoxelData - w ) );
                                *VoxelDz = 0.5f * ( (float)*( VoxelData + w*h ) - (float)*( VoxelData - w*h ) );

                                /* calculate gradiant magnitude */
                                *VoxelMag = fabsf( *VoxelDx ) + fabsf( *VoxelDy ) + fabsf( *VoxelDz ) + MIG_EPS_32F;

                                /* normalize */
                                *VoxelDx /= *VoxelMag;
                                *VoxelDy /= *VoxelMag;
                                *VoxelDz /= *VoxelMag;
                        }
}

/******************************************************************************/

void
mig_im_drv_2d_central_diffs ( float *data , int w , int h , float *dx , float *dy , float *dmag )
{
    int i , j;
    float *VoxelData;
    float *VoxelDx;
    float *VoxelDy;
    float *VoxelMag;

    /* zero all output images */
    memset ( dx   , 0x00 , w * h * sizeof(float) );
    memset ( dy   , 0x00 , w * h * sizeof(float) );
    memset ( dmag , 0x00 , w * h * sizeof(float) );

    for ( j = 1 ; j < h - 1 ; ++j )         /* y */
    {
        for ( i = 1 ; i < w - 1 ; ++i ) /* x */
        {
            /* get pointers to images */
            VoxelData = data + i + j * w;
            VoxelDx   = dx   + i + j * w;
            VoxelDy   = dy   + i + j * w;
            VoxelMag  = dmag + i + j * w;
    
            /* calculate central differences */
            *VoxelDx = 0.5f * ( (float)*( VoxelData + 1   ) - (float)*( VoxelData - 1 ) );
            *VoxelDy = 0.5f * ( (float)*( VoxelData + w   ) - (float)*( VoxelData - w ) );
                                
            /* calculate gradiant magnitude */
            *VoxelMag = fabsf( *VoxelDx ) + fabsf( *VoxelDy ) + MIG_EPS_32F;

            /* normalize */
            *VoxelDx /= *VoxelMag;
            *VoxelDy /= *VoxelMag;                    
        }
    }
}




static void
_conv_sobel_3d ( float *in, float *out, int w, int h, int z, float *ker )
{
    int i, j, k, l, m, n;

    /* center kernel */
    ker += 13;

    for ( k = 1; k < z - 1; ++k )
    {
        for ( j = 1; j < h - 1; ++j )
        {
            for ( i = 1; i < w - 1; ++i )
            {
                for ( n = -1; n <= 1; ++n )
                {
                    for ( m = -1; m <= 1; ++m )
                    {
                        for ( l = -1; l <= 1; ++l )
                        {
                            out[i + j * w + k * w * h] +=
                                in[( i + l ) + ( j + m ) * w + ( k + n ) * w * h] * ker[l + m * 3 + n * 9];
                        }
                    }
                }
            }
        }
    }
}
