/*
******************************************************************************
*
* Author      : Todor Petkov
* Filename    : mig_im_log.c
* Created     : 2007/06/21
* Description : 2D - 3D Laplacian of Gaussian
*
******************************************************************************
*/

#include "mig_im_log.h"
#include "mig_im_conv.h"

/*
******************************************************************************
*               GLOBAL PROTOTYPES IMPLEMENTATION
******************************************************************************
*/


/*AAA! GF: As far as I know log is not separable! */
int
mig_im_log_3d_sep ( float *src , float *dst ,
                    int w , int h , int z ,
                     mig_kernel_t *log )
{
    int i , j , rc;
 
    if ( ( w < log->r ) || ( h < log->r ) || ( z < log->r ) )
        return -1;

    /* convolution in x */
    for ( j = 0 ; j < z ; ++j )
    {
        for ( i = 0 ; i < h ; ++i )
        {
            rc = mig_im_conv_1d_x ( src + i * w + j * w * h , 
                    dst + i * w + j * w * h , w , log );
            if ( rc != 0 )
                return rc;
        }
    }

    /* convolution in y */
    for ( j = 0 ; j < z ; ++j )
    {
        for ( i = 0 ; i < w ; ++i )
        {
            rc = mig_im_conv_1d_y ( dst + i + j * w * h ,
                    dst + i + j * w * h , w , h , log );
            if ( rc != 0 )
                return rc;
        }
    }

    /* convolution in z */
    for ( j = 0 ; j < h ; ++j )
    {
        for ( i = 0 ; i < w ; ++i )
        {
            rc = mig_im_conv_1d_z ( dst + i + j * w ,
                    dst + i + j * w , w , h , z , log );
            if ( rc != 0 )
                return rc;
        }
    }

    return 0;
}

/******************************************************************************/

int
mig_im_log_2d_sep ( float *src , float *dst ,
                    int w , int h ,
                     mig_kernel_t *log )
{
    int i , rc;

    if ( ( w < log->r ) || ( h < log->r ) )
        return -1;

    /* convolution in x */
    for ( i = 0 ; i < h ; ++i )
    {
        rc = mig_im_conv_1d_x ( src + i * w , dst + i * w  , w , log );
        if ( rc != 0 )
            return -1;
    }
   
    /* convolution in y */
    for ( i = 0 ; i < w ; ++i )
    {
        rc = mig_im_conv_1d_y ( dst + i , dst + i , w , h , log );
        if ( rc != 0 )
            return -1;
    }

    return 0;
}

/******************************************************************************/

int
mig_im_log_2d_full ( float *src , float *dst ,
                     int w , int h ,
                     mig_kernel_t *log )
{
    int i , j , k , l;
    int paddedWidth , paddedHeight;
    float *kernel;
    float *srcpix , *dstpix;
    float *padded = NULL;
    float *idx1 , *idx2;

    paddedWidth  = w + ( log->r << 1 );
    paddedHeight = h + ( log->r << 1 );

    /* make sure that image is sufficienly big. i.e al
        least as big as the log kernel support */
    /*if ( log->r > w || log->r > h )
        return -1; */

    /* allocate memory for padded image */
    padded = (float*) malloc ( paddedWidth * paddedHeight * sizeof(float) );
    if ( padded == NULL )
        return -1;

    /* fill top rows of padded image */
    idx1 = padded + log->r + ( log->r - 1 ) * paddedWidth;
    idx2 = src + w;
    for ( i = 0 ; i < log->r ; ++i , idx1 -= paddedWidth , idx2 += w )
    {
        memcpy ( idx1 , idx2 , w * sizeof(float) );

        /* padd begining of each line */
        for ( j = 1 ; j <= log->r ; ++j )
            idx1[-j] = idx2[j];

        /* padd end of each line */
        for ( j = 1 ; j <= log->r ; ++j )
            idx1[w+j-1] = idx2[w-j-1];
    }

    /* fill central part of padded image with
        data from original image */
    idx1 = padded + log->r + log->r * paddedWidth;
    idx2 = src;
    for ( i = 0 ; i < h ; ++i , idx1 += paddedWidth , idx2 += w )
    {
        /* copy one line */
        memcpy ( idx1 , idx2 , w * sizeof(float) );

        /* padd begining of each line */
        for ( j = 1 ; j <= log->r ; ++j )
            idx1[-j] = idx2[j];

        /* padd end of each line */
        for ( j = 1 ; j <= log->r ; ++j )
            idx1[w+j-1] = idx2[w-j-1];
    }

    /* fill bottom rows of padded image */
    idx1 = padded + log->r +  ( log->r + h ) * paddedWidth;
    idx2 = src + ( h - 2 ) * w;
    for ( i = 0 ; i < log->r ; ++i , idx1 += paddedWidth , idx2 -= w )
    {
        memcpy ( idx1 , idx2 , w * sizeof(float) );

        /* padd begining of each line */
        for ( j = 1 ; j <= log->r ; ++j )
            idx1[-j] = idx2[j];

        /* padd end of each line */
        for ( j = 1 ; j <= log->r ; ++j )
            idx1[w+j-1] = idx2[w-j-1];
    }

    /* do convolution */
    kernel = log->data + log->r + log->r * log->d;
    idx1 = padded + log->r + log->r * paddedWidth;
    idx2 = dst;
    for ( j = 0 ; j < h ; ++j )
    {
        for ( i = 0 ; i < w ; ++i )
        {
            srcpix = idx1 + i + j * paddedWidth;
            dstpix = idx2 + i + j * w;
            *dstpix = 0.0f;

            for ( k = -log->r ; k <= log->r ; ++k )
                for ( l = -log->r ; l <= log->r ; ++l )
                    *dstpix += srcpix[l+k*paddedWidth] * kernel[l+k*log->d];
        }
    }

    free ( padded );
    return 0;
}


int
mig_im_log_3d_center ( float *src , float *dst ,
                    int w , int h , int z ,
                     mig_kernel_t *log )
{
    int i , j , k, rc;
 
	int center_x, center_y, center_z;
	
	float sum = 0;

	float* pker = log->data;

	center_x = w/2;
	center_y = h/2;
	center_z = z/2;

    if ( ( center_x < log->r ) || ( center_y < log->r ) || ( center_z < log->r ) )
        return -1;
    

	sum = 0;
	for ( k = center_z - log->r ; k <= center_z + log->r ; ++k )
	{
		for ( j = center_y - log->r ; j <= center_y + log->r ; ++j )
		{
			for ( i = center_x - log->r ; i <= center_x + log->r ; ++i , ++pker )
			{
				sum += ( *(src + w * h * k + w * j + i) * *pker );
			}
		}		
	}
	
	*dst = sum;

    return 0;
}