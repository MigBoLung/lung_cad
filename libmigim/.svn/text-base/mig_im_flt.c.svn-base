#include "mig_im_flt.h"
#include "mig_ut_mem.h"

/******************************************************************************/
/* local defines */
/******************************************************************************/
#ifndef MAX
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

#define PIX_SWAP( a , b ) { Mig16u temp=( a ) ; ( a ) = ( b ) ; ( b ) = temp; }
#define PIX_SORT( a , b ) { if (( a ) > ( b ) ) PIX_SWAP( ( a ) , ( b ) ); }

#define MAX_USHORT 65535

/******************************************************************************/
/* private function declarations */
/******************************************************************************/
static Mig16u  opt_med3        ( Mig16u p[] );
static Mig16u  opt_med5        ( Mig16u p[] );
static Mig16u  opt_med7        ( Mig16u p[] );
static Mig16u  opt_med9        ( Mig16u p[] );
static Mig16u  opt_med25       ( Mig16u p[] );

static void    _hist (  Mig16u *src ,
                        int w , int ww ,
		        int hst[] , 
			int *mdn , int *ltmdn );

static void    _mu_std ( double vect[] , int s ,
                                                float *mu , float *std );

static void    _tom3_msk ( Mig16u *src , int w ,
                                                float *mu , float *std );

static void    _tom5_msk ( Mig16u *src , int w ,
                                                float *mu , float *std );

static void    _nag_msk_1 ( Mig16u *idx , int w ,
                                                float *mu , float *std );

static void    _nag_msk_2 ( Mig16u *idx , int w ,
                                                float *mu , float *std );

static void    _nag_msk_3 ( Mig16u *idx , int w ,
                                                float *mu , float *std );

static void    _nag_msk_4 ( Mig16u *idx , int w ,
                                                float *mu , float *std );

static void    _nag_msk_5 ( Mig16u *idx , int w ,
                                                float *mu , float *std );

static void    _nag_msk_6 ( Mig16u *idx , int w ,
                                                float *mu , float *std );

static void    _nag_msk_7 ( Mig16u *idx , int w ,
                                                float *mu , float *std );

static void    _nag_msk_8 ( Mig16u *idx , int w ,
                                                float *mu , float *std );

static void    _nag_msk_9 ( Mig16u *idx , int w ,
                                                float *mu , float *std );

static void    _nag5_msk_1 ( Mig16u *idx , int w ,
                                                float *mu , float *std );

static void    _nag5_msk_2 ( Mig16u *idx , int w ,
                                                float *mu , float *std );

static void    _nag5_msk_3 ( Mig16u *idx , int w ,
                                                float *mu , float *std );

static void    _nag5_msk_4 ( Mig16u *idx , int w ,
                                                float *mu , float *std );

static void    _nag5_msk_5 ( Mig16u *idx , int w ,
                                                float *mu , float *std );

static void    _nag5_msk_6 ( Mig16u *idx , int w ,
                                                float *mu , float *std );

static void    _nag5_msk_7 ( Mig16u *idx , int w ,
                                                float *mu , float *std );

static void    _nag5_msk_8 ( Mig16u *idx , int w ,
                                                float *mu , float *std );

static void    _nag5_msk_9 ( Mig16u *idx , int w ,
                                                float *mu , float *std );
/******************************/
/* exported functions */
/******************************/

void
mig_im_flt_copy (  Mig16u *src , Mig16u *dst , int w , int h )
{
    memcpy ( dst , src , w * h * sizeof(Mig16u) );
}

void
mig_im_flt_med_cross_3 (  Mig16u *src , Mig16u *dst , int w , int h )
{
        Mig16u tmp[5];
        int i , j;

        for ( j = 1 ; j < h - 1 ; ++j )
                for ( i = 1 ; i < w - 1 ; ++i )
                {
                        tmp[0] = *( src + i - 1  + j * w );
                        tmp[1] = *( src + i      + j * w );
                        tmp[2] = *( src + i + 1  + j * w );
                        tmp[3] = *( src + i      + ( j - 1 ) * w );
                        tmp[4] = *( src + i      + ( j + 1 ) * w );

                        *( dst + i + j * w ) = opt_med5( tmp );
                }
}

/*******************************************************************/
void
mig_im_flt_med_cross_5 (  Mig16u *src , Mig16u *dst , int w , int h )
{
        Mig16u tmp[9];
        int i , j;

        for ( j = 2 ; j < ( h - 2 ) ; ++j )
                for ( i = 2 ; i < ( w - 2 ) ; ++i )
                {
                        tmp[0] = *( src + i - 2  + j * w );
                        tmp[1] = *( src + i - 1  + j * w );
                        tmp[2] = *( src + i      + j * w );
                        tmp[3] = *( src + i + 1  + j * w );
                        tmp[4] = *( src + i + 2  + j * w );
                        tmp[5] = *( src + i      + ( j - 2 ) * w );
                        tmp[6] = *( src + i      + ( j - 1 ) * w );
                        tmp[7] = *( src + i      + ( j + 1 ) * w );
                        tmp[8] = *( src + i      + ( j + 2 ) * w );

                        *( dst + i + j * w ) = opt_med9( tmp );
                }
}

/*******************************************************************/
void
mig_im_flt_med_box_3_3d (  Mig16u *src , Mig16u *dst ,
                                int w , int h , int z )
{
        Mig16u tmp[7];
        int i , j , k , s;

        s = w * h;
        src += ( s << 1 );
        dst += ( s << 1 );

        for ( k = 2 ; k < ( z - 2 ) ; ++k , src += s , dst += s )
                for ( j = 2 ; j < ( h - 2 ) ; ++j )
                        for ( i = 2 ; i < ( w - 2 ) ; ++i )
                        {
                                tmp[0] = *( src + i - 1  + j * w );
                                tmp[1] = *( src + i      + j * w );
                                tmp[2] = *( src + i + 1  + j * w );

                                tmp[3] = *( src + i      + ( j - 1 ) * w );
                                tmp[4] = *( src + i      + ( j + 1 ) * w );

                                tmp[5] = *( src + i      + j * w - s );
                                tmp[6] = *( src + i      + j * w + s );

                                *( dst + i + j * w ) = opt_med7( tmp );
                        }
}

/*******************************************************************/
void
mig_im_flt_med_box_3 (  Mig16u *src , Mig16u *dst , int w , int h )
{
        Mig16u tmp[9];
        int i , j;

        for ( j = 1 ; j < ( h - 1 ) ; ++j )
                for ( i = 1 ; i < ( w - 1 ) ; ++i )
                {
                        tmp[0] = *( src + i - 1 + ( j - 1 ) * w );
                        tmp[1] = *( src + i     + ( j - 1 ) * w );
                        tmp[2] = *( src + i + 1 + ( j + 1 ) * w );

                        tmp[3] = *( src + i - 1  + j * w );
                        tmp[4] = *( src + i      + j * w );
                        tmp[5] = *( src + i + 1  + j * w );

                        tmp[6] = *( src + i - 1  + ( j + 1 ) * w );
                        tmp[7] = *( src + i      + ( j + 1 ) * w );
                        tmp[8] = *( src + i + 1  + ( j + 1 ) * w );

                        *( dst + i + j * w ) = opt_med9( tmp );
                }
}

/*******************************************************************/
void
mig_im_flt_med_box_5 (  Mig16u *src , Mig16u *dst , int w , int h )
{
        Mig16u tmp[25];
        int i , j;

        for ( j = 2 ; j < h - 2 ; ++j )
                for ( i = 2 ; i < w - 2 ; ++i )
                {
                        tmp[0] = *( src + i - 2 + j * w );
                        tmp[1] = *( src + i - 1 + j * w );
                        tmp[2] = *( src + i     + j * w );
                        tmp[3] = *( src + i + 1 + j * w );
                        tmp[4] = *( src + i + 2 + j * w );

                        tmp[5] = *( src + i - 2 + ( j - 1 ) * w );
                        tmp[6] = *( src + i - 1 + ( j - 1 ) * w );
                        tmp[7] = *( src + i     + ( j - 1 ) * w );
                        tmp[8] = *( src + i + 1 + ( j - 1 ) * w );
                        tmp[9] = *( src + i + 2 + ( j - 1 ) * w );

                        tmp[10] = *( src + i - 2 + ( j + 1 ) * w );
                        tmp[11] = *( src + i - 1 + ( j + 1 ) * w );
                        tmp[12] = *( src + i     + ( j + 1 ) * w );
                        tmp[13] = *( src + i + 1 + ( j + 1 ) * w );
                        tmp[14] = *( src + i + 2 + ( j + 1 ) * w );

                        tmp[15] = *( src + i - 2 + ( j - 2 ) * w );
                        tmp[16] = *( src + i - 1 + ( j - 2 ) * w );
                        tmp[17] = *( src + i     + ( j - 2 ) * w );
                        tmp[18] = *( src + i + 1 + ( j - 2 ) * w );
                        tmp[19] = *( src + i + 2 + ( j - 2 ) * w );

                        tmp[20] = *( src + i - 2 + ( j + 2 ) * w );
                        tmp[21] = *( src + i - 1 + ( j + 2 ) * w );
                        tmp[22] = *( src + i     + ( j + 2 ) * w );
                        tmp[23] = *( src + i + 1 + ( j + 2 ) * w );
                        tmp[24] = *( src + i + 2 + ( j + 2 ) * w );

                        *( dst + i + j * w ) = opt_med25( tmp );
                }
}

/*******************************************************************/
void
mig_im_flt_med (  Mig16u *src , Mig16u *dst , int w , int h , int ww )
{
        int i , j , k , d;
        int th , mdn , ltmdn;
        Mig16u *left , *right;
        int hst[ MAX_USHORT + 1 ];

        d  = ( ww >> 1 );
        th = ( ww * ww ) >> 1;

        dst += d + ( d * w );
        for ( j = 0 ; j <= ( h - ww ) ; ++j , src += w , dst += w )
        {
                /* process first window of each line separately */
                mig_memz ( &hst , ( MAX_USHORT + 1 ) * sizeof( int ) );
                _hist ( src , w , ww , hst , &mdn , &ltmdn );
                *dst = mdn;

                for ( i = 1 ; i <= ( w - ww ) ; ++i )
                {
                        left  = src  + i - 1;
                        right = left + ww;

                        for ( k = 0 ; k < ww ; ++k , left += w , right += w )
                        {
                                hst[*left] -- ;
                                if ( *left < mdn )
                                        ltmdn -- ;

                                hst[*right] ++ ;
                                if ( *right < mdn )
                                        ltmdn ++ ;
                        }

                        if ( ltmdn > th )
                                while ( ltmdn > th )
                                {
                                        mdn --;
                                        ltmdn -= hst[mdn];
                                }
                        else
                                while ( ( ltmdn + hst[mdn] ) <= th )
                                {
                                        ltmdn += hst[mdn];
                                        mdn ++ ;
                                }

                        *( dst + i ) = ( Mig16u ) mdn;
                }
        }
}

/*******************************************************************/
void
mig_im_flt_tomita3 (  Mig16u *src , Mig16u *dst , int w , int h )
{
        Mig16u *idx_src , *idx_dst;
        int i , j , k , idx = 0;
        float tmp , std[5] , mu[5];

        for ( j = 2 ; j < ( h - 2 ); ++j )
        {
                for ( i = 2 ; i < ( w - 2 ) ; ++i )
                {
                        idx_src = src + i + j * w;
                        idx_dst = dst + i + j * w;

                        _tom3_msk( idx_src         , w , &mu[0] , &std[0] );
                        _tom3_msk( idx_src - w - 1 , w , &mu[1] , &std[1] );
                        _tom3_msk( idx_src - w + 1 , w , &mu[2] , &std[2] );
                        _tom3_msk( idx_src + w - 1 , w , &mu[3] , &std[3] );
                        _tom3_msk( idx_src + w + 1 , w , &mu[4] , &std[4] );

                        tmp = 65535.0;
                        for ( k = 0 ; k < 5 ; ++k )
                        {
                                if ( std[k] < tmp )
                                {
                                        tmp = std[k];
                                        idx = k;
                                }
                        }

                        *idx_dst = ( Mig16u ) mu[idx];
                }
        }
}

/*******************************************************************/
void
mig_im_flt_tomita5 (  Mig16u *src , Mig16u *dst , int w , int h )
{
        Mig16u *idx_src , *idx_dst;
        int i , j , k , idx = 0;
        float tmp , std[5] , mu[5];

        for ( j = 5 ; j < ( h - 5 ); ++j )
        {
                for ( i = 5 ; i < ( w - 5 ) ; ++i )
                {
                        idx_src = src + i + j * w;
                        idx_dst = dst + i + j * w;

                        _tom5_msk( idx_src , w , &mu[0] , &std[0] );
                        _tom5_msk( idx_src - ( w << 1 ) - 2 , w ,
                                        &mu[1] , &std[1] );
                        _tom5_msk( idx_src - ( w << 1 ) + 2 , w ,
                                        &mu[2] , &std[2] );
                        _tom5_msk( idx_src + ( w << 1 ) - 2 , w ,
                                        &mu[3] , &std[3] );
                        _tom5_msk( idx_src + ( w << 1 ) + 2 , w ,
                                        &mu[4] , &std[4] );

                        tmp = 65535.0;
                        for ( k = 0 ; k < 5 ; ++k )
                        {
                                if ( std[k] < tmp )
                                {
                                        tmp = std[k];
                                        idx = k;
                                }
                        }

                        *idx_dst = ( Mig16u ) mu[idx];
                }
        }
}

/*******************************************************************/
void
mig_im_flt_nagao7 (  Mig16u *src , Mig16u *dst , int w , int h )
{
        Mig16u *idx_src , *idx_dst;
        int i , j , k , idx;
        float tmp , std[9] , mu[9];

        for ( j = 3 ; j < ( h - 3 ) ; ++j )
        {
                for ( i = 3 ; i < ( w - 3 ) ; ++i )
                {
                        idx_src = src + i + j * w;
                        idx_dst = dst + i + j * w;

                        _nag_msk_1( idx_src , w , &mu[0] , &std[0] );
                        _nag_msk_2( idx_src , w , &mu[1] , &std[1] );
                        _nag_msk_3( idx_src , w , &mu[2] , &std[2] );
                        _nag_msk_4( idx_src , w , &mu[3] , &std[3] );
                        _nag_msk_5( idx_src , w , &mu[4] , &std[4] );
                        _nag_msk_6( idx_src , w , &mu[5] , &std[5] );
                        _nag_msk_7( idx_src , w , &mu[6] , &std[6] );
                        _nag_msk_8( idx_src , w , &mu[7] , &std[7] );
                        _nag_msk_9( idx_src , w , &mu[8] , &std[8] );

                        tmp = 65535.0;
                        for ( k = 0 ; k < 9 ; ++k )
                        {
                                if ( std[k] < tmp )
                                {
                                        tmp = std[k];
                                        idx = k;
                                }
                        }

                        *idx_dst = ( Mig16u ) mu[idx];
                }
        }
}

/*******************************************************************/
void
mig_im_flt_nagao5 (  Mig16u *src , Mig16u *dst , int w , int h )
{
        Mig16u *idx_src , *idx_dst;
        int i , j , k , idx;
        float tmp , std[9] , mu[9];

        for ( j = 2 ; j < ( h - 2 ) ; ++j )
        {
                for ( i = 2 ; i < ( w - 2 ) ; ++i )
                {
                        idx_src = src + i + j * w;
                        idx_dst = dst + i + j * w;

                        _nag5_msk_1( idx_src , w , &mu[0] , &std[0] );
                        _nag5_msk_2( idx_src , w , &mu[1] , &std[1] );
                        _nag5_msk_3( idx_src , w , &mu[2] , &std[2] );
                        _nag5_msk_4( idx_src , w , &mu[3] , &std[3] );
                        _nag5_msk_5( idx_src , w , &mu[4] , &std[4] );
                        _nag5_msk_6( idx_src , w , &mu[5] , &std[5] );
                        _nag5_msk_7( idx_src , w , &mu[6] , &std[6] );
                        _nag5_msk_8( idx_src , w , &mu[7] , &std[7] );
                        _nag5_msk_9( idx_src , w , &mu[8] , &std[8] );

                        tmp = 65535.0;
                        for ( k = 0 ; k < 9 ; ++k )
                        {
                                if ( std[k] < tmp )
                                {
                                        tmp = std[k];
                                        idx = k;
                                }
                        }

                        *idx_dst = ( Mig16u ) mu[idx];
                }
        }
}

/**************************************************************************/
/* private function definitions */
/**************************************************************************/
static Mig16u
opt_med3 ( Mig16u p[] )
{
        PIX_SORT( p[0] , p[1] );
        PIX_SORT( p[1] , p[2] );
        PIX_SORT( p[0] , p[1] );
        return( p[1] ) ;
}

/**************************************************************************/
static Mig16u
opt_med5 ( Mig16u p[] )
{
        PIX_SORT( p[0] , p[1] );
        PIX_SORT( p[3] , p[4] );
        PIX_SORT( p[0] , p[3] );
        PIX_SORT( p[1] , p[4] );
        PIX_SORT( p[1] , p[2] );
        PIX_SORT( p[2] , p[3] );
        PIX_SORT( p[1] , p[2] );
        return( p[2] ) ;
}

/**************************************************************************/
static Mig16u
opt_med7 ( Mig16u p[] )
{
        PIX_SORT( p[0], p[5] );
        PIX_SORT( p[0], p[3] );
        PIX_SORT( p[1], p[6] );
        PIX_SORT( p[2], p[4] );
        PIX_SORT( p[0], p[1] );
        PIX_SORT( p[3], p[5] );
        PIX_SORT( p[2], p[6] );
        PIX_SORT( p[2], p[3] );
        PIX_SORT( p[3], p[6] );
        PIX_SORT( p[4], p[5] );
        PIX_SORT( p[1], p[4] );
        PIX_SORT( p[1], p[3] );
        PIX_SORT( p[3], p[4] );
        return ( p[3] );
}

/**************************************************************************/
static Mig16u
opt_med9 ( Mig16u p[] )
{
        PIX_SORT( p[1] , p[2] );
        PIX_SORT( p[4] , p[5] );
        PIX_SORT( p[7] , p[8] );
        PIX_SORT( p[0] , p[1] );
        PIX_SORT( p[3] , p[4] );
        PIX_SORT( p[6] , p[7] );
        PIX_SORT( p[1] , p[2] );
        PIX_SORT( p[4] , p[5] );
        PIX_SORT( p[7] , p[8] );
        PIX_SORT( p[0] , p[3] );
        PIX_SORT( p[5] , p[8] );
        PIX_SORT( p[4] , p[7] );
        PIX_SORT( p[3] , p[6] );
        PIX_SORT( p[1] , p[4] );
        PIX_SORT( p[2] , p[5] );
        PIX_SORT( p[4] , p[7] );
        PIX_SORT( p[4] , p[2] );
        PIX_SORT( p[6] , p[4] );
        PIX_SORT( p[4] , p[2] );
        return( p[4] );
}

/**************************************************************************/
static Mig16u
opt_med25 ( Mig16u p[] )
{
        PIX_SORT( p[0]  , p[1]  );      PIX_SORT( p[3]  , p[4]  );      PIX_SORT( p[2]  , p[4]  );
        PIX_SORT( p[2]  , p[3]  );      PIX_SORT( p[6]  , p[7]  );      PIX_SORT( p[5]  , p[7]  );
        PIX_SORT( p[5]  , p[6]  );      PIX_SORT( p[9]  , p[10] );      PIX_SORT( p[8]  , p[10] );
        PIX_SORT( p[8]  , p[9]  );      PIX_SORT( p[12] , p[13] );      PIX_SORT( p[11] , p[13] );
        PIX_SORT( p[11] , p[12] );      PIX_SORT( p[15] , p[16] );      PIX_SORT( p[14] , p[16] );
        PIX_SORT( p[14] , p[15] );      PIX_SORT( p[18] , p[19] );      PIX_SORT( p[17] , p[19] );
        PIX_SORT( p[17] , p[18] );      PIX_SORT( p[21] , p[22] );      PIX_SORT( p[20] , p[22] );
        PIX_SORT( p[20] , p[21] );      PIX_SORT( p[23] , p[24] );      PIX_SORT( p[2]  , p[5]  );
        PIX_SORT( p[3]  , p[6]  );      PIX_SORT( p[0]  , p[6]  );      PIX_SORT( p[0]  , p[3]  );
        PIX_SORT( p[4]  , p[7]  );      PIX_SORT( p[1]  , p[7]  );      PIX_SORT( p[1]  , p[4]  );
        PIX_SORT( p[11] , p[14] );      PIX_SORT( p[8]  , p[14] );      PIX_SORT( p[8]  , p[11] );
        PIX_SORT( p[12] , p[15] );      PIX_SORT( p[9]  , p[15] );      PIX_SORT( p[9]  , p[12] );
        PIX_SORT( p[13] , p[16] );      PIX_SORT( p[10] , p[16] );      PIX_SORT( p[10] , p[13] );
        PIX_SORT( p[20] , p[23] );      PIX_SORT( p[17] , p[23] );      PIX_SORT( p[17] , p[20] );
        PIX_SORT( p[21] , p[24] );      PIX_SORT( p[18] , p[24] );      PIX_SORT( p[18] , p[21] );
        PIX_SORT( p[19] , p[22] );      PIX_SORT( p[8]  , p[17] );      PIX_SORT( p[9]  , p[18] );
        PIX_SORT( p[0]  , p[18] );      PIX_SORT( p[0]  , p[9]  );      PIX_SORT( p[10] , p[19] );
        PIX_SORT( p[1]  , p[19] );      PIX_SORT( p[1]  , p[10] );      PIX_SORT( p[11] , p[20] );
        PIX_SORT( p[2]  , p[20] );      PIX_SORT( p[2]  , p[11] );      PIX_SORT( p[12] , p[21] );
        PIX_SORT( p[3]  , p[21] );      PIX_SORT( p[3]  , p[12] );      PIX_SORT( p[13] , p[22] );
        PIX_SORT( p[4]  , p[22] );      PIX_SORT( p[4]  , p[13] );      PIX_SORT( p[14] , p[23] );
        PIX_SORT( p[5]  , p[23] );      PIX_SORT( p[5]  , p[14] );      PIX_SORT( p[15] , p[24] );
        PIX_SORT( p[6]  , p[24] );      PIX_SORT( p[6]  , p[15] );      PIX_SORT( p[7]  , p[16] );
        PIX_SORT( p[7]  , p[19] );      PIX_SORT( p[13] , p[21] );      PIX_SORT( p[15] , p[23] );
        PIX_SORT( p[7]  , p[13] );      PIX_SORT( p[7]  , p[15] );      PIX_SORT( p[1]  , p[9]  );
        PIX_SORT( p[3]  , p[11] );      PIX_SORT( p[5]  , p[17] );      PIX_SORT( p[11] , p[17] );
        PIX_SORT( p[9]  , p[17] );      PIX_SORT( p[4]  , p[10] );      PIX_SORT( p[6]  , p[12] );
        PIX_SORT( p[7]  , p[14] );      PIX_SORT( p[4]  , p[6]  );      PIX_SORT( p[4]  , p[7]  );
        PIX_SORT( p[12] , p[14] );      PIX_SORT( p[10] , p[14] );      PIX_SORT( p[6]  , p[7]  );
        PIX_SORT( p[10] , p[12] );      PIX_SORT( p[6]  , p[10] );      PIX_SORT( p[6]  , p[17] );
        PIX_SORT( p[12] , p[17] );      PIX_SORT( p[7]  , p[17] );      PIX_SORT( p[7]  , p[10] );
        PIX_SORT( p[12] , p[18] );      PIX_SORT( p[7]  , p[12] );      PIX_SORT( p[10] , p[18] );
        PIX_SORT( p[12] , p[20] );      PIX_SORT( p[10] , p[20] );      PIX_SORT( p[10] , p[12] );

        return ( p[12] );
}

/*******************************************************************/
static void
_hist ( Mig16u *src , int w , int ww , int hst[] , int *mdn , int *ltmdn )
{
        int i , j;
        int cnt = 0 , th;
        Mig16u tmp;

        th = ( ( ww * ww ) >> 1 ) + 1;
        for ( j = 0 ; j < ww ; ++j , src += w )
                for ( i = 0 ; i < ww ; ++i )
                {
                        tmp = *( src + i );
                        ++ hst[tmp];
                        ++ cnt;

                        if ( cnt == th )
                                *mdn = tmp;
                }

        *ltmdn = 0;
        for ( i = 0 ; i < ( *mdn ) ; ++i )
                *ltmdn += hst[i];

}

/*******************************************************************/
static void
_mu_std ( double vect[] , int s , float *mu , float *std )
{
        int i;
        double tmp_mu = 0.0, tmp_std = 0.0;

        for ( i = 0; i < s ; ++i )
                tmp_mu += vect[i];
        tmp_mu /= s;

        for ( i = 0; i < s ; ++i )
                tmp_std += ( tmp_mu - vect[i] ) * ( tmp_mu - vect[i] );
        tmp_std /= s;
        tmp_std = sqrt( tmp_std );

        *mu  = ( float )( tmp_mu  );
        *std = ( float )( tmp_std );
}

/*******************************************************************/
static void
_tom3_msk ( Mig16u *src , int w , float *mu , float *std )
{
        double msk[9];

        msk[0] = (double) *( src - 1 );
        msk[1] = (double) *( src );
        msk[2] = (double) *( src + 1 );

        msk[3] = (double) *( src - w - 1 );
        msk[4] = (double) *( src - w );
        msk[5] = (double) *( src - w + 1 );

        msk[6] = (double) *( src + w - 1 );
        msk[7] = (double) *( src + w );
        msk[8] = (double) *( src + w + 1 );

        _mu_std ( msk , 9 , mu , std );
}

/*******************************************************************/
static void
_tom5_msk ( Mig16u *src , int w , float *mu , float *std )
{
        double msk[25];

        msk[0] = (double) *( src - 2 - ( w << 1 ) );
        msk[1] = (double) *( src - 1 - ( w << 1 ) );
        msk[2] = (double) *( src     - ( w << 1 ) );
        msk[3] = (double) *( src + 1 - ( w << 1 ) );
        msk[4] = (double) *( src + 2 - ( w << 1 ) );

        msk[5] = (double) *( src - 2 - w );
        msk[6] = (double) *( src - 1 - w );
        msk[7] = (double) *( src     - w );
        msk[8] = (double) *( src + 1 - w );
        msk[9] = (double) *( src + 2 - w );

        msk[10] = (double) *( src - 2 );
        msk[11] = (double) *( src - 1 );
        msk[12] = (double) *( src     );
        msk[13] = (double) *( src + 1 );
        msk[14] = (double) *( src + 2 );

        msk[15] = (double) *( src - 2 + w );
        msk[16] = (double) *( src - 1 + w );
        msk[17] = (double) *( src     + w );
        msk[18] = (double) *( src + 1 + w );
        msk[19] = (double) *( src + 2 + w );

        msk[20] = (double) *( src - 2 + ( w << 1 ) );
        msk[21] = (double) *( src - 1 + ( w << 1 ) );
        msk[22] = (double) *( src     + ( w << 1 ) );
        msk[23] = (double) *( src + 1 + ( w << 1 ) );
        msk[24] = (double) *( src + 2 + ( w << 1 ) );

        _mu_std ( msk , 25 , mu , std );
}

/*******************************************************************/
static void
_nag_msk_1 ( Mig16u *idx , int w , float *mu , float *std )
{
        double msk[9];

        msk[0] = (double) *( idx - 1 );
        msk[1] = (double) *( idx );
        msk[2] = (double) *( idx + 1 );

        msk[3] = (double) *( idx - w - 1 );
        msk[4] = (double) *( idx - w );
        msk[5] = (double) *( idx - w + 1 );

        msk[6] = (double) *( idx + w - 1 );
        msk[7] = (double) *( idx + w );
        msk[8] = (double) *( idx + w + 1 );

        _mu_std ( msk , 9 , mu , std );
}

/*******************************************************************/
static void
_nag_msk_2 ( Mig16u *idx , int w , float *mu , float *std )
{
        double msk[7];

        msk[0] = (double) *( idx - 1 );
        msk[1] = (double) *( idx );

        msk[2] = (double) *( idx - w - 2 );
        msk[3] = (double) *( idx - w - 1 );
        msk[4] = (double) *( idx - w );

        msk[5] = (double) *( idx - ( w << 1 ) - 2 );
        msk[6] = (double) *( idx - ( w << 1 ) - 1 );

        _mu_std ( msk , 7 , mu , std );
}

/*******************************************************************/
static void
_nag_msk_3 ( Mig16u *idx , int w , float *mu , float *std )
{
        double msk[7];

        msk[0] = (double) *( idx - 1 );
        msk[1] = (double) *( idx );

        msk[2] = (double) *( idx + w - 2 );
        msk[3] = (double) *( idx + w - 1 );
        msk[4] = (double) *( idx + w );

        msk[5] = (double) *( idx + ( w << 1 ) - 2 );
        msk[6] = (double) *( idx + ( w << 1 ) - 1 );

        _mu_std ( msk , 7 , mu , std );
}

/*******************************************************************/
static void
_nag_msk_4 ( Mig16u *idx , int w , float *mu , float *std )
{
        double msk[7];

        msk[0] = (double) *( idx );
        msk[1] = (double) *( idx + 1 );

        msk[2] = (double) *( idx - w );
        msk[3] = (double) *( idx - w + 1 );
        msk[4] = (double) *( idx - w + 2 );

        msk[5] = (double) *( idx - ( w << 1 ) + 1 );
        msk[6] = (double) *( idx - ( w << 1 ) + 2 );

        _mu_std ( msk , 7 , mu , std );
}

/*******************************************************************/
static void
_nag_msk_5 ( Mig16u *idx , int w , float *mu , float *std )
{
        double msk[7];

        msk[0] = (double) *( idx );
        msk[1] = (double) *( idx + 1 );

        msk[2] = (double) *( idx + w );
        msk[3] = (double) *( idx + w + 1 );
        msk[4] = (double) *( idx + w + 2 );

        msk[5] = (double) *( idx + ( w << 1 ) + 1 );
        msk[6] = (double) *( idx + ( w << 1 ) + 2 );

        _mu_std ( msk , 7 , mu , std );
}

/*******************************************************************/
static void
_nag_msk_6 ( Mig16u *idx , int w , float *mu , float *std )
{
        double msk[10];

        msk[0] = (double) *( idx - 3 );
        msk[1] = (double) *( idx - 2 );
        msk[2] = (double) *( idx - 1 );
        msk[3] = (double) *( idx );

        msk[4] = (double) *( idx - w - 3 );
        msk[5] = (double) *( idx - w - 2 );
        msk[6] = (double) *( idx - w - 1 );

        msk[7] = (double) *( idx + w - 3 );
        msk[8] = (double) *( idx + w - 2 );
        msk[9] = (double) *( idx + w - 1 );

        _mu_std ( msk , 10 , mu , std );
}

/*******************************************************************/
static void
_nag_msk_7 ( Mig16u *idx , int w , float *mu , float *std )
{
        double msk[10];

        msk[0] = (double) *( idx );

        msk[1] = (double) *( idx - ( 3 * w ) - 1 );
        msk[2] = (double) *( idx - ( 3 * w ) );
        msk[3] = (double) *( idx - ( 3 * w  ) + 1 );

        msk[4] = (double) *( idx - ( w << 1 ) - 1 );
        msk[5] = (double) *( idx - ( w << 1 ) );
        msk[6] = (double) *( idx - ( w << 1 ) + 1 );

        msk[7] = (double) *( idx - w - 1 );
        msk[8] = (double) *( idx - w );
        msk[9] = (double) *( idx - w + 1 );

        _mu_std ( msk , 10 , mu , std );
}

/*******************************************************************/
static void
_nag_msk_8 ( Mig16u *idx , int w , float *mu , float *std )
{
        double msk[10];

        msk[0] = (double) *( idx );
        msk[1] = (double) *( idx + 1 );
        msk[2] = (double) *( idx + 2 );
        msk[3] = (double) *( idx + 3 );

        msk[4] = (double) *( idx - w + 1 );
        msk[5] = (double) *( idx - w + 2 );
        msk[6] = (double) *( idx - w + 3 );

        msk[7] = (double) *( idx + w + 1 );
        msk[8] = (double) *( idx + w + 2 );
        msk[9] = (double) *( idx + w + 3 );

        _mu_std ( msk , 10 , mu , std );
}

/*******************************************************************/
static void
_nag_msk_9 ( Mig16u *idx , int w , float *mu , float *std )
{
        double msk[10];

        msk[0] = (double) *( idx );

        msk[1] = (double) *( idx + ( 3 * w ) - 1 );
        msk[2] = (double) *( idx + ( 3 * w ) );
        msk[3] = (double) *( idx + ( 3 * w  ) + 1 );

        msk[4] = (double) *( idx + ( w << 1 ) - 1 );
        msk[5] = (double) *( idx + ( w << 1 ) );
        msk[6] = (double) *( idx + ( w << 1 ) + 1 );

        msk[7] = (double) *( idx + w - 1 );
        msk[8] = (double) *( idx + w );
        msk[9] = (double) *( idx + w + 1 );

        _mu_std ( msk , 10 , mu , std );
}

/*******************************************************************/
static void
_nag5_msk_1 ( Mig16u *idx , int w , float *mu , float *std )
{
        double msk[9];

        msk[0] = (double) *( idx - 2 );
        msk[1] = (double) *( idx - 1 );
        msk[2] = (double) *( idx     );
        msk[3] = (double) *( idx - w - 2 );
        msk[4] = (double) *( idx - w - 1 );
        msk[5] = (double) *( idx - w     );
        msk[6] = (double) *( idx - ( w << 1 ) - 2 );
        msk[7] = (double) *( idx - ( w << 1 ) - 1 );
        msk[8] = (double) *( idx - ( w << 1 )     );

        _mu_std ( msk , 9 , mu , std );
}

/*******************************************************************/
static void
_nag5_msk_2 ( Mig16u *idx , int w , float *mu , float *std )
{
        double msk[9];

        msk[0] = (double) *( idx     );
        msk[1] = (double) *( idx + 1 );
        msk[2] = (double) *( idx + 2 );
        msk[3] = (double) *( idx - w     );
        msk[4] = (double) *( idx - w + 1 );
        msk[5] = (double) *( idx - w + 2 );
        msk[6] = (double) *( idx - ( w << 1 )     );
        msk[7] = (double) *( idx - ( w << 1 ) + 1 );
        msk[8] = (double) *( idx - ( w << 1 ) + 2 );

        _mu_std ( msk , 9 , mu , std );
}

/*******************************************************************/
static void
_nag5_msk_3 ( Mig16u *idx , int w , float *mu , float *std )
{
        double msk[9];

        msk[0] = (double) *( idx         );
        msk[1] = (double) *( idx - w - 1 );
        msk[2] = (double) *( idx - w     );
        msk[3] = (double) *( idx - w + 1 );
        msk[4] = (double) *( idx - ( w << 1 ) - 2 );
        msk[5] = (double) *( idx - ( w << 1 ) - 1 );
        msk[6] = (double) *( idx - ( w << 1 )     );
        msk[7] = (double) *( idx - ( w << 1 ) + 1 );
        msk[8] = (double) *( idx - ( w << 1 ) + 2 );

        _mu_std ( msk , 9 , mu , std );
}

/*******************************************************************/
static void
_nag5_msk_4 ( Mig16u *idx , int w , float *mu , float *std )
{
        double msk[9];

        msk[0] = (double) *( idx );
        msk[1] = (double) *( idx + 1 );
        msk[2] = (double) *( idx + 2 );
        msk[3] = (double) *( idx + w );
        msk[4] = (double) *( idx + w + 1 );
        msk[5] = (double) *( idx + w + 2 );
        msk[6] = (double) *( idx + ( w << 1 )     );
        msk[7] = (double) *( idx + ( w << 1 ) + 1 );
        msk[8] = (double) *( idx + ( w << 1 ) + 2 );

        _mu_std ( msk , 9 , mu , std );
}

/*******************************************************************/
static void
_nag5_msk_5 ( Mig16u *idx , int w , float *mu , float *std )
{
        double msk[9];

        msk[0] = (double) *( idx );
        msk[1] = (double) *( idx - w + 1 );
        msk[2] = (double) *( idx     + 1 );
        msk[3] = (double) *( idx + w + 1 );
        msk[4] = (double) *( idx - ( w << 1 ) + 2 );
        msk[5] = (double) *( idx - w + 2 );
        msk[6] = (double) *( idx     + 2 );
        msk[7] = (double) *( idx + w + 2 );
        msk[8] = (double) *( idx + ( w << 1 ) + 2 );

        _mu_std ( msk , 9 , mu , std );
}

/*******************************************************************/
static void
_nag5_msk_6 ( Mig16u *idx , int w , float *mu , float *std )
{
        double msk[9];

        msk[0] = (double) *( idx - 2 );
        msk[1] = (double) *( idx - 1 );
        msk[2] = (double) *( idx );
        msk[3] = (double) *( idx + w - 2 );
        msk[4] = (double) *( idx + w - 1 );
        msk[5] = (double) *( idx + w     );
        msk[6] = (double) *( idx + ( w << 1 ) - 2 );
        msk[7] = (double) *( idx + ( w << 1 ) - 1 );
        msk[8] = (double) *( idx + ( w << 1 )     );

        _mu_std ( msk , 9 , mu , std );
}

/*******************************************************************/
static void
_nag5_msk_7 ( Mig16u *idx , int w , float *mu , float *std )
{
        double msk[9];

        msk[0] = (double) *( idx );
        msk[1] = (double) *( idx + w - 1 );
        msk[2] = (double) *( idx + w     );
        msk[3] = (double) *( idx + w + 1 );
        msk[4] = (double) *( idx + ( w << 1 ) - 2 );
        msk[5] = (double) *( idx + ( w << 1 ) - 1 );
        msk[6] = (double) *( idx + ( w << 1 )     );
        msk[7] = (double) *( idx + ( w << 1 ) + 1 );
        msk[8] = (double) *( idx + ( w << 1 ) + 2 );

        _mu_std ( msk , 9 , mu , std );
}

/*******************************************************************/
static void
_nag5_msk_8 ( Mig16u *idx , int w , float *mu , float *std )
{
        double msk[9];

        msk[0] = (double) *( idx );
        msk[1] = (double) *( idx - w - 1 );
        msk[2] = (double) *( idx     - 1 );
        msk[3] = (double) *( idx + w - 1 );
        msk[4] = (double) *( idx - ( w << 1 ) - 2 );
        msk[5] = (double) *( idx - w - 2 );
        msk[6] = (double) *( idx     - 2 );
        msk[7] = (double) *( idx + w - 2 );
        msk[8] = (double) *( idx + ( w << 1 ) - 2 );

        _mu_std ( msk , 9 , mu , std );
}

/*******************************************************************/
static void
_nag5_msk_9 ( Mig16u *idx , int w , float *mu , float *std )
{
        double msk[9];

        msk[0] = (double) *( idx - w - 1 );
        msk[1] = (double) *( idx - w     );
        msk[2] = (double) *( idx - w + 1 );
        msk[3] = (double) *( idx     - 1 );
        msk[4] = (double) *( idx         );
        msk[5] = (double) *( idx     + 1 );
        msk[6] = (double) *( idx + w - 1 );
        msk[7] = (double) *( idx + w     );
        msk[8] = (double) *( idx + w + 1 );

        _mu_std ( msk , 9 , mu , std );
}

/*******************************************************************/
#undef MIN
#undef MAX
#undef PIX_SORT
#undef PIX_SWAP
#undef MAX_USHORT
/*******************************************************************/


