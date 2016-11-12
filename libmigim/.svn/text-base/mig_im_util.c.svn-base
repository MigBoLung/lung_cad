#include "mig_im_util.h"
#include "libmigut.h"

/***************************************************************************/
double
mig_im_util_h2g ( double h ,
                  double min , double max ,
                  double wc , double ww )
{
        if ( h <= wc - 0.5 - ( ww - 1 ) / 2.0 )
                return min;

        if ( h > wc - 0.5 + ( ww - 1 ) / 2.0 )
                return max;

        return ( ( ( h - ( wc - 0.5 ) ) / ( ww - 1 ) + 0.5 ) *
                   ( max - min ) + min );
}

/***************************************************************************/
int
mig_im_util_isempty_32f ( Mig32f *src , int s )
{
        int isempty = 1;
        int i;

        for ( i = 0 ; i < s ; ++i , ++src )
        {
                if ( MIG_ABS(*src) > MIG_EPS_32F )
                {
                        isempty = 0;
                        break;
                }
        }

        return isempty;
}

/***************************************************************************/
void
mig_im_util_max_32f ( Mig32f *src ,
                      int s , 
                      Mig32f *max )
{
	int i;

	*max = MIG_MIN_32F;

	for ( i = 0 ; i < s ; ++i , ++src )
	{
		if ( *src > *max )
			*max = *src;
	}        
}

/***************************************************************************/
void
mig_im_util_min_max_32f ( Mig32f *src , int s , Mig32f *min , Mig32f *max )
{
	int i;

	*min = MIG_MAX_32F;
	*max = MIG_MIN_32F;

	for ( i = 0 ; i < s ; ++i , ++src )
	{
		if ( *src > *max )
			*max = *src;

		if ( *src < *min )
			*min = *src;
	}
}

/***************************************************************************/
void
mig_im_util_min_max_16u ( Mig16u *src , int s , Mig32f *min , Mig32f *max )
{
	int i;

	*min = MIG_MAX_32F;
	*max = MIG_MIN_32F;

	for ( i = 0 ; i < s ; ++i , ++src )
	{
		if ( *src > *max )
			*max = (float) *src;

		if ( *src < *min )
			*min = (float) *src;
	}
}

/***************************************************************************/
void
mig_im_util_mat2gray_32f ( Mig32f *src , int s , Mig32f min , Mig32f max )
{
	int i;

	for ( i = 0 ; i < s ; ++i , ++src )
		*src = ( *src - min ) / ( max - min );
}


/***************************************************************************/
void
mig_im_util_conv_16u_32f ( Mig16u *src ,
                           Mig32f *dst ,
                           int s )
{
	int i;

	for ( i = 0 ; i < s ; ++i , ++src , ++dst )
		*dst = (Mig32f) *src;
}

/***************************************************************************/
void
mig_im_util_conv_32f_16u ( Mig32f *src , 
                           Mig16u *dst ,
                           int s )
{
        int i;
        for ( i = 0 ; i < s ; ++i , ++src , ++dst )
                *dst = (Mig16u) *src;
}

/***************************************************************************/
void
mig_im_util_scale_32f_16u ( Mig32f *src , 
                           Mig16u *dst ,
                           int s )
{
        int i;
        float min , max;

        mig_im_util_min_max_32f ( src , s , &min , &max );

        if ( max == min )
        {
                for ( i = 0 ; i < s ; ++i , ++src , ++dst )
                {
                        *dst = (Mig16u) *src;
                }
        }
        else
        {
                for ( i = 0 ; i < s ; ++i , ++src , ++dst )
                {
                        *dst = (Mig16u) 
                                (  ( ( *src - min ) / ( max - min ) ) * 65535.0f );
                }
        }
}

/***************************************************************************/
void
mig_im_util_conv_32f_1u ( Mig32f *src ,
                          Mig8u *dst ,
                          int s )
{
        int i;
        for ( i = 0 ; i < s ; ++i , ++src , ++dst )
                *dst = (Mig8u) *src;        
}

/***************************************************************************/
void
mig_im_util_scale_32f_rgb ( Mig32f *src , 
                            unsigned char *rgb ,
                            int w , int h )
{
        int i;
        float min , max;
        unsigned char *R , *G , *B;

        R = rgb;
        G = rgb + w * h;
        B = rgb + 2 * w * h;

        mig_im_util_min_max_32f ( src , w * h , &min , &max );

        /* copy R plane */
        if ( max == min )
        {
                for ( i = 0 ; i < w * h ; ++i , ++src  )
                {
                        *( R + i ) = (Mig8u) *src;
                }
        }
        else
        {
                for ( i = 0 ; i < w * h ; ++i , ++src )
                {
                        *( R + i ) = (Mig8u) 
                                (  ( ( *src - min ) / ( max - min ) ) * 255.0f );
                }
        }

        /* copy G plane */
        memcpy ( G , R , w * h * sizeof(unsigned char) );

        /* copy B plane */
        memcpy ( B , R , w * h * sizeof(unsigned char) );
}

/***************************************************************************/
void
mig_im_util_scale_16u_rgb ( Mig16u *src , 
                            unsigned char *rgb ,
                            int w , int h )
{
        int i;
        float min , max;
        unsigned char *R , *G , *B;

        R = rgb;
        G = rgb + w * h;
        B = rgb + 2 * w * h;

        mig_im_util_min_max_16u ( src , w * h , &min , &max );

        /* copy R plane */
        if ( max == min )
        {
                for ( i = 0 ; i < w * h ; ++i , ++src  )
                {
                        *( R + i ) = (Mig8u) *src;
                }
        }
        else
        {
                for ( i = 0 ; i < w * h ; ++i , ++src )
                {
                        *( R + i ) = (Mig8u) 
                                (  ( ( *src - min ) / ( max - min ) ) * 255.0f );
                }
        }

        /* copy G plane */
        memcpy ( G , R , w * h * sizeof(unsigned char) );

        /* copy B plane */
        memcpy ( B , R , w * h * sizeof(unsigned char) );
}
