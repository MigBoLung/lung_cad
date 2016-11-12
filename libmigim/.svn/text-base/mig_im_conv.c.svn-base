/*
******************************************************************************
*
* Author      : Todor Petkov
* Filename    : mig_conv.c
* Created     : 2007/06/21
* Description : 1d convolution
*
******************************************************************************
*/

#include "mig_im_conv.h"

/*
******************************************************************************
*               GLOBAL PROTOTYPES IMPLEMENTATION
******************************************************************************
*/

int
mig_im_conv_1d_x ( float *src , float *dst , int w , mig_kernel_t *kernel )
{
        float *buffer;
        int i , j;

        if ( kernel->r > w )
                return -1;

        /* allocate local buffer */
        buffer = (float*)
                calloc ( ( w + 2 * kernel->r ) , sizeof( float ) );
        if ( buffer == NULL )
                return -1;

        /* shift buffer */
        buffer += kernel->r;

        /* copy input line into buffer */
        memcpy ( buffer , src , w * sizeof( float ) );

        /* setup border conditions */
        if ( kernel->border == BORDER_REPEAT )
        {
                for ( i = 1 ; i <= kernel->r ; ++i )
                {
                        buffer[-i] = buffer[0];
                        buffer[w+i-1] = buffer[w-1];
                }
        }

        if ( kernel->border == BORDER_REFLECT )
        {
                for ( i = 1 ; i <= kernel->r ; ++i )
                {
                        buffer[-i] = buffer[i];
                        buffer[w+i-1] = buffer[w-i-1];
                }
        }

        /* zero output buffer */
        memset ( dst , 0x00 , w * sizeof( float ) );

        /* perform convolution */
        for ( i = 0 ; i < w ; ++i , ++dst )
        {

                for ( j = -kernel->r ; j <= kernel->r ; ++j )
                {
                        *dst += buffer[i+j] * kernel->center[j];
                }
        }

        /* shift buffer back */
        buffer -= kernel->r;

        free ( buffer );
        return 0;
}

/*****************************************************************************/

int
mig_im_conv_1d_y ( float *src , float *dst , int w , int h , mig_kernel_t *kernel )
{
        float *buffer;
        int i , j;

        if ( kernel->r > h )
                return -1;

        /* allocate local buffer */
        buffer = (float*)
                calloc ( ( h + 2 * kernel->r ) , sizeof( float ) );
        if ( buffer == NULL )
                return -1;

        /* shift buffer */
        buffer += kernel->r;

        /* copy input line into buffer */
        for ( i = 0 ; i < h ; ++i , src += w )
                buffer[i] = *src;

        /* setup border conditions */
        if ( kernel->border == BORDER_REPEAT )
        {
                for ( i = 1 ; i <= kernel->r ; ++i )
                {
                        buffer[-i] = buffer[0];
                        buffer[h+i-1] = buffer[h-1];
                }
        }

        if ( kernel->border == BORDER_REFLECT )
        {
                for ( i = 1 ; i <= kernel->r ; ++i )
                {
                        buffer[-i] = buffer[i];
                        buffer[h+i-1] = buffer[h-i-1];
                }
        }

        /* perform convolution */
        for ( i = 0 ; i < h ; ++i , dst += w )
        {
                *dst = 0.0f;

                for ( j = -kernel->r ; j <= kernel->r ; ++j )
                {
                        *dst += buffer[i+j] * kernel->center[j];
                }
        }

        /* shift buffer back */
        buffer -= kernel->r;

        free ( buffer );
        return 0;
}

/*****************************************************************************/

int
mig_im_conv_1d_z ( float *src , float *dst , int w , int h , int z , mig_kernel_t *kernel )
{
        float *buffer;
        int i , j;

        if ( kernel->r > z )
                return -1;

        /* allocate local buffer */
        buffer = (float*)
                calloc ( ( z + 2 * kernel->r ) , sizeof( float ) );
        if ( buffer == NULL )
                return -1;

        /* shift buffer */
        buffer += kernel->r;

        /* copy input line into buffer */
        for ( i = 0 ; i < z ; ++i , src += w * h )
                buffer[i] = *src;


        /* setup border conditions */
        if ( kernel->border == BORDER_REPEAT )
        {
                for ( i = 1 ; i <= kernel->r ; ++i )
                {
                        buffer[-i] = buffer[0];
                        buffer[z+i-1] = buffer[z-1];
                }
        }

        if ( kernel->border == BORDER_REFLECT )
        {
                for ( i = 1 ; i <= kernel->r ; ++i )
                {
                        buffer[-i] = buffer[i];
                        buffer[z+i-1] = buffer[z-i-1];
                }
        }

        /* perform convolution */
        for ( i = 0 ; i < z ; ++i , dst+= w * h )
        {
                *dst = 0.0f;

                for ( j = -kernel->r ; j <= kernel->r ; ++j )
                {
                        *dst += buffer[i+j] * kernel->center[j];
                }
        }

        /* shift buffer back */
        buffer -= kernel->r;

        free ( buffer );
        return 0;
}
