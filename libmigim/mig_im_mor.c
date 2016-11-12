#include "mig_im_mor.h"
#include "mig_ut_mem.h"

#define PIXOFF          0x00
#define PIXON           0xFF
#define PIXMARK         0x01

/****************************************************************************/
/* private function prototypes */
/****************************************************************************/
static void     _erode_dsk_hex   ( Mig8u *msk , int w , int h , int r );
static void     _erode_dsk_squ   ( Mig8u *msk , int w , int h , int r );
static void     _erode_dsk_dia   ( Mig8u *msk , int w , int h , int r );
static void     _erode_dsk_oct   ( Mig8u *msk , int w , int h , int r );
static void     _erode_dsk_full  ( Mig8u *msk , int w , int h , int r );

static void     _dilate_dsk_hex   ( Mig8u *msk , int w , int h , int r );
static void     _dilate_dsk_squ   ( Mig8u *msk , int w , int h , int r );
static void     _dilate_dsk_dia   ( Mig8u *msk , int w , int h , int r );
static void     _dilate_dsk_oct   ( Mig8u *msk , int w , int h , int r );
static void     _dilate_dsk_full  ( Mig8u *msk , int w , int h , int r );

static Mig8u*   _get_dsk_full ( int r );
static void     _put_dsk_full ( Mig8u *dsk , int r );

static void     _erode_line_h  ( Mig8u *msk , int w , int h , int len );
static void     _erode_line_v  ( Mig8u *msk , int w , int h , int len );
static void     _erode_line_d1 ( Mig8u *msk , int w , int h , int len );
static void     _erode_line_d2 ( Mig8u *msk , int w , int h , int len );

static void     _dilate_line_h  ( Mig8u *msk , int w , int h , int len );
static void     _dilate_line_v  ( Mig8u *msk , int w , int h , int len );
static void     _dilate_line_d1 ( Mig8u *msk , int w , int h , int len );
static void     _dilate_line_d2 ( Mig8u *msk , int w , int h , int len );

/*****************************************************************************/
/* main disk driver functions */
/*****************************************************************************/
void
mig_im_mor_erode_disk ( Mig8u *msk , int w , int h , int r , MigDiskType t )
{
        switch ( t )
        {
                case MIG_DISK_HEX :
                        _erode_dsk_hex ( msk , w , h , r );
                        break;
                case MIG_DISK_SQUARE :
                        _erode_dsk_squ ( msk , w , h , r );
                        break;
                case MIG_DISK_DIAMOND :
                        _erode_dsk_dia ( msk , w , h , r );
                        break;
                case MIG_DISK_OCTAGON :
                        _erode_dsk_oct ( msk , w , h , r );
                        break;
                case MIG_DISK_FULL :
                        _erode_dsk_full ( msk , w , h , r );
                        break;
        }
}

/*****************************************************************************/
void
mig_im_mor_dilate_disk ( Mig8u *msk , int w , int h , int r , MigDiskType t  )
{
        switch ( t )
        {
                case MIG_DISK_HEX :
                        _dilate_dsk_hex ( msk , w , h , r );
                        break;
                case MIG_DISK_SQUARE :
                        _dilate_dsk_squ ( msk , w , h , r );
                        break;
                case MIG_DISK_DIAMOND :
                        _dilate_dsk_dia ( msk , w , h , r );
                        break;
                case MIG_DISK_OCTAGON :
                        _dilate_dsk_oct ( msk , w , h , r );
                        break;
                case MIG_DISK_FULL :
                        _dilate_dsk_full ( msk , w , h , r );
                        break;
        }
}

/*****************************************************************************/
/* elementary 3x3 cross erosion */
void
mig_im_mor_erode_cross ( Mig8u *msk , int w , int h )
{
        int i, j, k;
        int n[4] = { -w , -1 , +w , +1 };
        Mig8u *idx;

        for ( j = 1 ; j < h - 1 ; ++ j )
        {
                idx = msk + j * w + 1;
                for ( i = 1 ; i < w - 1 ; ++ i , ++idx )
                {
                        if ( *idx  == PIXOFF )
                                continue;

                        for ( k = 0 ; k < 4 ; ++ k )
                        {
                                if ( *( idx + n[k] ) == PIXOFF )
                                {
                                        *idx = PIXMARK;
                                        break;
                                }
                        }
                }
        }

        for ( i = 0 ; i < w * h ; ++ i , ++msk )
                if ( *msk == PIXMARK )
                        *msk = PIXOFF;
}

/*****************************************************************************/
/* elementary 3x3 cross dilation */
void
mig_im_mor_dilate_cross ( Mig8u *msk , int w , int h )
{
        int i, j, k;
        int n[4] = { -w , -1 , +w , +1 };
        Mig8u *idx;

        for ( j = 1 ; j < h - 1 ; ++ j )
        {
                idx = msk + j * w + 1;
                for ( i = 1 ; i < w - 1 ; ++ i , ++idx )
                {
                        if ( ( *idx  == PIXOFF ) || ( *idx == PIXMARK ) )
                                continue;

                        for ( k = 0 ; k < 4 ; ++ k )
                                if ( *( idx + n[k] ) == PIXOFF )
                                        *( idx + n[k] ) = PIXMARK;
                }
        }

        for ( i = 0 ; i < w * h ; ++ i , ++msk )
                if ( *msk == PIXMARK )
                        *msk = PIXON;
}

/*****************************************************************************/
/* elementary 1 x 3 horizontal line erosion */
void
mig_im_mor_erode_line_h ( Mig8u *msk , int w , int h )
{
        int i, j, k;
        int n[2] = { -1 , +1 };
        Mig8u *idx;

        for ( j = 1 ; j < h - 1 ; ++ j )
        {
                idx = msk + j * w + 1;
                for ( i = 1 ; i < w - 1 ; ++ i , ++idx )
                {
                        if ( *idx  == PIXOFF )
                                continue;

                        for ( k = 0 ; k < 2 ; ++ k )
                        {
                                if ( *( idx + n[k] ) == PIXOFF )
                                {
                                        *idx = PIXMARK;
                                        break;
                                }
                        }
                }
        }

        for ( i = 0 ; i < w * h ; ++ i , ++msk )
                if ( *msk == PIXMARK )
                        *msk = PIXOFF;
}

/*****************************************************************************/
/* elementary 1 x 3 horizontal line dilation */
void
mig_im_mor_dilate_line_h ( Mig8u *msk , int w , int h )
{
        int i, j, k;
        int n[2] = { -1 , +1 };
        Mig8u *idx;

        for ( j = 1 ; j < h - 1 ; ++ j )
        {
                idx = msk + j * w + 1;

                for ( i = 1 ; i < w - 1 ; ++ i , ++idx )
                {
                        if ( ( *idx  == PIXOFF ) || ( *idx == PIXMARK ) )
                                continue;

                        for ( k = 0 ; k < 2 ; ++ k )
                                if ( *( idx + n[k] ) == PIXOFF )
                                        *( idx + n[k] ) = PIXMARK;
                }
        }

        for ( i = 0 ; i < w * h ; ++ i , ++msk )
                if ( *msk == PIXMARK )
                        *msk = PIXON;
}

/*****************************************************************************/
/* elementary  3 x 1 vertical line erosion */
void
mig_im_mor_erode_line_v ( Mig8u *msk , int w , int h )
{
        int i, j, k;
        int n[2] = { -w , +w };
        Mig8u *idx;

        for ( j = 1 ; j < h - 1 ; ++ j )
        {
                idx = msk + j * w + 1;
                for ( i = 1 ; i < w - 1 ; ++ i , ++idx )
                {
                        if ( *idx  == PIXOFF )
                                continue;

                        for ( k = 0 ; k < 2 ; ++ k )
                        {
                                if ( *( idx + n[k] ) == PIXOFF )
                                {
                                        *idx = PIXMARK;
                                        break;
                                }
                        }
                }
        }

        for ( i = 0 ; i < w * h ; ++ i , ++msk )
                if ( *msk == PIXMARK )
                        *msk = PIXOFF;
}

/*****************************************************************************/
/* elementary  3 x 1 vertical line dilation */
void
mig_im_mor_dilate_line_v ( Mig8u *msk , int w , int h )
{
        int i, j, k;
        int n[2] = { -w , +w };
        Mig8u *idx;

        for ( j = 1 ; j < h - 1 ; ++ j )
        {
                idx = msk + j * w + 1;
                for ( i = 1 ; i < w - 1 ; ++ i , ++idx )
                {
                        if ( ( *idx  == PIXOFF ) || ( *idx == PIXMARK ) )
                                continue;

                        for ( k = 0 ; k < 2 ; ++ k )
                                if ( *( idx + n[k] ) == PIXOFF )
                                        *( idx + n[k] ) = PIXMARK;
                }
        }

        for ( i = 0 ; i < w * h ; ++ i , ++msk )
                if ( *msk == PIXMARK )
                        *msk = PIXON;

}

/*****************************************************************************/
/* elementary +45deg line erosion */
void
mig_im_mor_erode_line_d1 ( Mig8u *msk , int w , int h )
{
        int i, j, k;
        int n[2] = { -w + 1 , +w - 1 };
        Mig8u *idx;

        for ( j = 1 ; j < h - 1 ; ++ j )
        {
                idx = msk + j * w + 1;
                for ( i = 1 ; i < w - 1 ; ++ i , ++idx )
                {
                        if ( *idx  == PIXOFF )
                                continue;

                        for ( k = 0 ; k < 2 ; ++ k )
                        {
                                if ( *( idx + n[k] ) == PIXOFF )
                                {
                                        *idx = PIXMARK;
                                        break;
                                }
                        }
                }
        }

        for ( i = 0 ; i < w * h ; ++ i , ++msk )
                if ( *msk == PIXMARK )
                        *msk = PIXOFF;
}

/*****************************************************************************/
/* elementary +45deg line dilation */
void
mig_im_mor_dilate_line_d1 ( Mig8u *msk , int w , int h )
{
        int i, j, k;
        int n[2] = { -w + 1 , +w - 1 };
        Mig8u *idx;

        for ( j = 1 ; j < h - 1 ; ++ j )
        {
                idx = msk + j * w + 1;
                for ( i = 1 ; i < w - 1 ; ++ i , ++idx )
                {
                        if ( ( *idx  == PIXOFF ) || ( *idx == PIXMARK ) )
                                continue;

                        for ( k = 0 ; k < 2 ; ++ k )
                                if ( *( idx + n[k] ) == PIXOFF )
                                       *( idx + n[k] ) = PIXMARK;
                }
        }

        for ( i = 0 ; i < w * h ; ++ i , ++msk )
                if ( *msk == PIXMARK )
                        *msk = PIXON;
}

/*****************************************************************************/
/* elementary -45deg line erosion */
void
mig_im_mor_erode_line_d2 ( Mig8u *msk , int w , int h )
{
        int i, j, k;
        int n[2] = { -w - 1 , +w + 1 };
        Mig8u *idx;

        for ( j = 1 ; j < h - 1 ; ++ j )
        {
                idx = msk + j * w + 1;
                for ( i = 1 ; i < w - 1 ; ++ i , ++idx )
                {
                        if ( *idx  == PIXOFF )
                                continue;

                        for ( k = 0 ; k < 2 ; ++ k )
                        {
                                if ( *( idx + n[k] ) == PIXOFF )
                                {
                                        *idx = PIXMARK;
                                        break;
                                }
                        }
                }
        }

        for ( i = 0 ; i < w * h ; ++ i , ++msk )
                if ( *msk == PIXMARK )
                        *msk = PIXOFF;
}

/*****************************************************************************/
/* elementary -45deg line dilation */
void
mig_im_mor_dilate_line_d2 ( Mig8u *msk , int w , int h )
{
        int i, j, k;
        int n[2] = { -w - 1 , +w + 1 };
        Mig8u *idx;

        for ( j = 1 ; j < h - 1 ; ++ j )
        {
                idx = msk + j * w + 1;
                for ( i = 1 ; i < w - 1 ; ++ i , ++idx )
                {
                        if ( ( *idx  == PIXOFF ) || ( *idx == PIXMARK ) )
                                continue;

                        for ( k = 0 ; k < 2 ; ++ k )
                                if ( *( idx + n[k] ) == PIXOFF )
                                        *( idx + n[k] ) = PIXMARK;
                }
        }

        for ( i = 0 ; i < w * h ; ++ i , ++msk )
                if ( *msk == PIXMARK )
                        *msk = PIXON;
}

/*****************************************************************************/
/* elementary 3x3 square erosion */
void
mig_im_mor_erode_square ( Mig8u *msk , int w , int h )
{
        int i, j, k;
        int n[8] = { -w + 1 , -w , -w - 1 ,  -1 , +w - 1 , +w , +w + 1 , +1 };
        Mig8u *idx;

        for ( j = 1 ; j < h - 1 ; ++ j )
        {
                idx = msk + j * w + 1;
                for ( i = 1 ; i < w - 1 ; ++ i , ++idx )
                {
                        if ( *idx  == PIXOFF )
                                continue;

                        for ( k = 0 ; k < 8 ; ++ k )
                        {
                                if ( *( idx + n[k] ) == PIXOFF )
                                {
                                        *idx = PIXMARK;
                                        break;
                                }
                        }
                }
        }

        for ( i = 0 ; i < w * h ; ++ i , ++msk )
                if ( *msk == PIXMARK )
                        *msk = PIXOFF;
}

/*****************************************************************************/
/* elementary 3x3 square dilation */
void
mig_im_mor_dilate_square ( Mig8u *msk , int w , int h )
{
        int i, j, k;
        int n[8] = { -w + 1 , -w , -w - 1 ,  -1 , +w - 1 , +w , +w + 1 , +1 };
        Mig8u *idx;

        for ( j = 1 ; j < h - 1 ; ++ j )
        {
                idx = msk + j * w + 1;
                for ( i = 1 ; i < w - 1 ; ++ i , ++idx )
                {
                        if ( ( *idx  == PIXOFF ) || ( *idx == PIXMARK ) )
                                continue;

                        for ( k = 0 ; k < 8 ; ++ k )
                                if ( *( idx + n[k] ) == PIXOFF )
                                        *( idx + n[k] ) = PIXMARK;
                }
        }

        for ( i = 0 ; i < w * h ; ++ i , ++msk )
                if ( *msk == PIXMARK )
                        *msk = PIXON;
}

/*****************************************************************************/
/* private functions */
/*****************************************************************************/
static void
_erode_dsk_hex ( Mig8u *msk , int w , int h , int r )
{
        r = 2 * r - 1;

        if ( r <= 1 )
        {
                mig_im_mor_erode_cross ( msk , w , h );
                return;
        }

        _erode_line_h  ( msk , w , h , r );
        _erode_line_d2 ( msk , w , h , r );
        _erode_line_d1 ( msk , w , h , r );
}

/*****************************************************************************/
static void
_erode_dsk_squ ( Mig8u *msk , int w , int h , int r )
{
        r = 2 * r - 1;

        if ( r <= 1 )
        {
                mig_im_mor_erode_square ( msk , w , h );
                return;
        }

        _erode_line_h  ( msk , w , h , r );
        _erode_line_v  ( msk , w , h , r );
}

/*****************************************************************************/
static void
_erode_dsk_dia ( Mig8u *msk , int w , int h , int r )
{
        r = 2 * r - 1;

        if ( r <= 1 )
        {
                mig_im_mor_erode_cross ( msk , w , h );
                return;
        }

        _erode_line_d1 ( msk , w , h , r );
        _erode_line_d2 ( msk , w , h , r );
        mig_im_mor_erode_cross ( msk , w , h );
}

/*****************************************************************************/
static void
_erode_dsk_oct ( Mig8u *msk , int w , int h , int r )
{
        r = 2 * r - 1;

        if ( r <= 1 )
        {
                mig_im_mor_erode_cross ( msk , w , h );
                return;
        }

        _erode_line_d1 ( msk , w , h , r );
        _erode_line_d2 ( msk , w , h , r );
        _erode_line_h  ( msk , w , h , r );
        _erode_line_v  ( msk , w , h , r );
}

/*****************************************************************************/
static void
_erode_dsk_full ( Mig8u *msk , int w , int h , int r )
{
        int i , j , k , l;
        int d = 2 * r + 1;

        Mig8u *dsk = NULL;
        Mig8u *idx = msk;
        Mig8u tmp_1 , tmp_2;

        if ( r <= 1 )
        {
                mig_im_mor_erode_cross ( msk , w , h );
                return;
        }

        dsk = _get_dsk_full( r );
        if ( !dsk )
                return;

        dsk += ( r + d * r );

        for ( j = r ; j < h - r ; ++ j )
                for ( i = r ; i < w - r ; ++ i )
                {
                        idx = msk + i + j * w;
                        if ( *idx == PIXOFF )
                                continue;

                        for ( k = -r ; k <= r ; ++ k )
                                for ( l = -r ; l <= r ; ++ l )
                                {
                                        tmp_1 = *( idx + l + k * w );
                                        tmp_2 = *( dsk + l + k * d );

                                        if (    ( tmp_2 == PIXON  ) &&
                                                ( tmp_1 == PIXOFF ) )
                                        {
                                                *idx = PIXMARK;
                                                break;
                                        }
                                }
                }

        for ( i = 0 ; i < w * h ; ++ i , ++msk )
                if ( *msk == PIXMARK )
                        *msk = PIXOFF;

        dsk -= ( r + d * r );
        free ( dsk );
}

/*****************************************************************************/
static void
_dilate_dsk_hex ( Mig8u *msk , int w , int h , int r )
{
        r = 2 * r - 1;

        if ( r <= 1 )
        {
                mig_im_mor_dilate_cross ( msk , w , h );
                return;
        }

        _dilate_line_h  ( msk , w , h , r );
        _dilate_line_d2 ( msk , w , h , r );
        _dilate_line_d1 ( msk , w , h , r );
}

/*****************************************************************************/
static void
_dilate_dsk_squ ( Mig8u *msk , int w , int h , int r )
{
        r = 2 * r - 1;

        if ( r <= 1 )
        {
                mig_im_mor_dilate_square( msk , w , h );
                return;
        }

        _dilate_line_h  ( msk , w , h , r );
        _dilate_line_v  ( msk , w , h , r );
}

/*****************************************************************************/
static void
_dilate_dsk_dia ( Mig8u *msk , int w , int h , int r )
{
        r = 2 * r - 1;

        if ( r <= 1 )
        {
                mig_im_mor_dilate_cross ( msk , w , h );
                return;
        }

        _dilate_line_d1 ( msk , w , h , r );
        _dilate_line_d2 ( msk , w , h , r );
        mig_im_mor_dilate_cross ( msk , w , h );
}

/*****************************************************************************/
static void
_dilate_dsk_oct ( Mig8u *msk , int w , int h , int r )
{
        r = 2 * r - 1;

        if ( r <= 1 )
        {
                mig_im_mor_dilate_cross ( msk , w , h );
                return;
        }

        _dilate_line_d1 ( msk , w , h , r );
        _dilate_line_d2 ( msk , w , h , r );
        _dilate_line_h  ( msk , w , h , r );
        _dilate_line_v  ( msk , w , h , r );
}

/*****************************************************************************/
static void
_dilate_dsk_full ( Mig8u *msk , int w , int h , int r )
{
        int i , j , m , n;
        int d = 2 * r + 1;

        Mig8u *dsk = NULL;
        Mig8u *idx = msk;
        Mig8u *tmp_1 , *tmp_2;

        if ( r <= 1 )
        {
                mig_im_mor_dilate_cross ( msk , w , h );
                return;
        }

        dsk = _get_dsk_full( r );
        if ( !dsk )
                return;

        dsk += ( r + d * r );

        for ( j = r ; j < h - r ; ++ j )
                for ( i = r ; i < w - r ; ++ i )
                {
                        idx = msk + i + j * w;

                        if ( ( *idx  == PIXOFF ) || ( *idx == PIXMARK ) )
                                continue;

                        for ( m = -r ; m <= r ; ++ m )
                                for ( n = -r ; n <= r ; ++ n )
                                {
                                        tmp_1 = idx + n + m * w;
                                        tmp_2 = dsk + n + m * d;

                                        if (    ( *tmp_2 == PIXON ) &&
                                                ( *tmp_1 == PIXOFF ) )
                                                *tmp_1 = PIXMARK;
                                }
                }

        for ( i = 0 ; i < w * h ; ++i , ++msk )
                if ( *msk == PIXMARK )
                        *msk = PIXON;

        dsk -= ( r + d * r );
        free ( dsk );
}

/*****************************************************************************/
static Mig8u*
_get_dsk_full ( int r )
{
        Mig8u *dsk = NULL;
        int d = 2 * r + 1;      /* diameter */
        int i , j;


        dsk = ( Mig8u* ) malloc ( MIG_POW2( d ) * sizeof( Mig8u ) );
        if ( !dsk )
                return NULL;

        dsk += ( r + d * r );
        for ( j = -r ; j <= r ; ++j )
                for ( i = -r ; i <= r ; ++i )
                {
                        *( dsk + i + j * d ) =
                                ( sqrt( (float) ( MIG_POW2(i) + MIG_POW2(j) ) ) > r ) ? PIXOFF : PIXON;
                }
        dsk -= ( r + d * r );

        return dsk;
}

/*****************************************************************************/
static void
_put_dsk_full ( Mig8u *dsk , int r )
{
        int i , j;
        int d = 2 * r + 1;

        printf( "\n" );

        for ( j = 0 ; j < d ; ++ j )
        {
                for ( i = 0 ; i < d ; ++ i , ++dsk )
                        printf( " %03d " , *dsk );

                printf( " \n " );
        }

        printf( "\n" );
}

/*****************************************************************************/
static void
_erode_line_h ( Mig8u *msk , int w , int h , int len )
{
        int i, j, k, r;
        Mig8u *idx;

        r = ( len >> 1 );

        for ( j = r ; j < ( h - r ) ; ++ j )
                for ( i = r ; i < ( w - r ) ; ++ i )
                {
                        idx = msk + j * w + i;

                        if ( *idx  == PIXOFF )
                                continue;

                        for ( k = -r ; k <= r ; ++ k )
                        {
                                if ( *( idx + k ) == PIXOFF )
                                {
                                        *idx = PIXMARK;
                                        break;
                                }
                        }
                }

        for ( i = 0 ; i < ( w * h ) ; ++ i , ++msk )
                if ( *msk == PIXMARK )
                        *msk = PIXOFF;
}

/*****************************************************************************/
static void
_erode_line_v ( Mig8u *msk , int w , int h , int len )
{
        int i, j, k, r;
        Mig8u *idx;

        r = ( len >> 1 );

        for ( j = r ; j < ( h - r ) ; ++ j )
                for ( i = r ; i < ( w - r ) ; ++ i )
                {
                        idx = msk + j * w + i;

                        if ( *idx  == PIXOFF )
                                continue;

                        for ( k = -r ; k <= r ; ++ k )
                        {
                                if ( *( idx + ( k * w ) ) == PIXOFF )
                                {
                                        *idx = PIXMARK;
                                        break;
                                }
                        }
                }

        for ( i = 0 ; i < ( w * h ) ; ++ i , ++msk )
                if ( *msk == PIXMARK )
                        *msk = PIXOFF;
}

/*****************************************************************************/
static void
_erode_line_d1 ( Mig8u *msk , int w , int h , int len )
{
        int i, j, k , r;
        Mig8u *idx;

        r = ( len >> 1 );

        for ( j = r ; j < ( h - r ) ; ++ j )
                for ( i = r ; i < ( w - r ) ; ++ i )
                {
                        idx = msk + j * w + i;

                        if ( *idx  == PIXOFF )
                                continue;

                        for ( k = -r ; k <= r ; ++ k )
                        {
                                if ( *( idx + ( k * w ) - k ) == PIXOFF )
                                {
                                        *idx = PIXMARK;
                                        break;
                                }
                        }
                }

        for ( j = r ; j < ( h - r ) ; ++ j )
                for ( i = r ; i < ( w - r ) ; ++ i )
                        if ( *( msk + i + j * w ) == PIXMARK )
                                *( msk + i + j * w ) = PIXOFF;

}

/*****************************************************************************/
static void
_erode_line_d2 ( Mig8u *msk , int w , int h , int len )
{
        int i, j, k, r;
        Mig8u *idx;

        r = ( len >> 1 );

        for ( j = r ; j < ( h - r ) ; ++ j )
                for ( i = r ; i < ( w - r ) ; ++ i )
                {
                        idx = msk + j * w + i;

                        if ( *idx  == PIXOFF )
                                continue;

                        for ( k = -r ; k <= r ; ++ k )
                        {
                                if ( *( idx + ( k * w ) + k ) == PIXOFF )
                                {
                                        *idx = PIXMARK;
                                        break;
                                }
                        }
                }

        for ( j = r ; j < ( h - r ) ; ++ j )
                for ( i = r ; i < ( w - r ) ; ++ i )
                        if ( *( msk + i + j * w ) == PIXMARK )
                                *( msk + i + j * w ) = PIXOFF;

}

/*****************************************************************************/
static void
_dilate_line_h ( Mig8u *msk , int w , int h , int len )
{
        int i , j , k , r;
        Mig8u *idx;

        r = ( len >> 1 );

        for ( j = r ; j < ( h - r ) ; ++ j )
                for ( i = r ; i < ( w - r ) ; ++ i )
                {
                        idx = msk + j * w + i;

                        if ( ( *idx  == PIXOFF ) || ( *idx == PIXMARK ) )
                                continue;

                        for ( k = -r ; k <= r ; ++ k )
                                if ( *( idx + k ) == PIXOFF )
                                        *( idx + k ) = PIXMARK;
                }

        for ( j = r ; j < ( h - r ) ; ++ j )
                for ( i = r ; i < ( w - r ) ; ++ i )
                        if ( *( msk + i + j * w ) == PIXMARK )
                                *( msk + i + j * w ) = PIXON;
}

/*****************************************************************************/
static void
_dilate_line_v ( Mig8u *msk , int w , int h , int len )
{
        int i , j , k , r;
        Mig8u *idx;

        r = ( len >> 1 );

        for ( j = r ; j < ( h - r ) ; ++ j )
                for ( i = r ; i < ( w - r ) ; ++ i )
                {
                        idx = msk + j * w + i;

                        if ( ( *idx  == PIXOFF ) || ( *idx == PIXMARK ) )
                                continue;

                        for ( k = -r ; k <= r ; ++ k )
                                if ( *( idx + k * w ) == PIXOFF )
                                        *( idx + k * w ) = PIXMARK;
                }

        for ( j = r ; j < ( h - r ) ; ++ j )
                for ( i = r ; i < ( w - r ) ; ++ i )
                        if ( *( msk + i + j * w ) == PIXMARK )
                                *( msk + i + j * w ) = PIXON;
}

/*****************************************************************************/
static void
_dilate_line_d1 ( Mig8u *msk , int w , int h , int len )
{
        int i , j , k , r;
        Mig8u *idx;

        r = len >> 1;

        for ( j = r ; j < ( h - r ) ; ++ j )
                for ( i = r ; i < ( w - r ) ; ++ i )
                {
                        idx = msk + j * w + i;

                        if ( ( *idx  == PIXOFF ) || ( *idx == PIXMARK ) )
                                continue;

                        for ( k = -r ; k <= r ; ++ k )
                                if ( *( idx + ( k * w ) - k ) == PIXOFF )
                                        *( idx + ( k * w ) - k ) = PIXMARK;
                }

        for ( j = r ; j < ( h - r ) ; ++ j )
                for ( i = r ; i < ( w - r ) ; ++ i )
                        if ( *( msk + i + j * w )  == PIXMARK )
                                *( msk + i + j * w ) = PIXON;

}

/*****************************************************************************/
static void
_dilate_line_d2 ( Mig8u *msk , int w , int h , int len )
{
        int i , j , k , r;
        Mig8u *idx;

        r = ( len >> 1 );

        for ( j = r ; j < ( h - r ) ; ++ j )
                for ( i = r ; i < ( w - r ) ; ++ i )
                {
                        idx = msk + j * w + i;

                        if ( ( *idx  == PIXOFF ) || ( *idx == PIXMARK ) )
                                continue;

                        for ( k = -r ; k <= r ; ++ k )
                                if ( *( idx + ( k * w ) + k ) == PIXOFF )
                                        *( idx + ( k * w ) + k ) = PIXMARK;
                }

        for ( j = r ; j < ( h - r ) ; ++ j )
                for ( i = r ; i < ( w - r ) ; ++ i )
                        if ( *( msk + i + j * w )  == PIXMARK )
                                *( msk + i + j * w ) = PIXON;
}


