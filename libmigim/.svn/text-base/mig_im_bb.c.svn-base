#include "mig_im_bb.h"
#include "mig_ut_mem.h"

/***************************************/
/* PRIVATE FUNCTIONS */
/***************************************/
static void
_cut_8u_2d ( Mig8u *src , 
             Mig8u *dst , 
             mig_size_t *s ,
             mig_roi_t *bb );

static void
_cut_8u_3d ( Mig8u *src , 
             Mig8u *dst , 
             mig_size_t *s ,
             mig_roi_t *bb );

static void
_cut_16u_2d ( Mig16u *src , 
              Mig16u *dst , 
              mig_size_t *s ,
              mig_roi_t *bb );

static void
_cut_16u_3d ( Mig16u *src , 
              Mig16u *dst , 
              mig_size_t *s ,
              mig_roi_t *bb );

/***************************************/
/* EXPORTS */
/***************************************/
void
mig_im_bb_cut_8u ( Mig8u *src , 
                   Mig8u *dst , 
                   mig_size_t *s ,
                   mig_roi_t *bb )
{
        if ( s->slices == 0 )
                _cut_8u_2d ( src , dst , 
                             s , bb );
        else
                _cut_8u_3d ( src , dst , 
                             s , bb );
}

/***************************************/
void
mig_im_bb_cut_16u ( Mig16u *src , 
                    Mig16u *dst , 
                    mig_size_t *s ,
                    mig_roi_t *bb )
{
        if ( s->slices == 0 )
                _cut_16u_2d ( src , dst , 
                              s , bb );
        else
                _cut_16u_3d ( src , dst , 
                              s , bb );
}

/**************************************************************************************************/

void
mig_im_bb_cut_3d ( Mig16u *src ,
                   int w , int h , int z ,
                   int cx , int cy , int cz ,
                   float *dst ,
                   int r )
{
    int x0 , x1;
    int y0 , y1;
    int z0 , z1;
    int d = 2 * r + 1;
    int i , j , k;

    /* zero output */
    memset ( dst , 0x00 , d * d * d * sizeof(float) );

    /* shift src to current pixel */
    src += cx + cy * w + cz * w * h;

    /* shift dst to its central pixel */
    dst += r + r * d + r * d * d;

    /* take care of boundary conditions */
    x0 = ( ( cx - r ) < 0 )? cx : r;
    x1 = ( ( cx + r ) >= w )? (w - cx) - 1 : r;

    y0 = ( ( cy - r ) < 0 )? cy : r;
    y1 = ( ( cy + r ) >= h )? (h - cy) - 1 : r;

    z0 = ( ( cz - r ) < 0 )? cz : r;
    z1 = ( ( cz + r ) >= z )? (z - cz) - 1 : r;

    /* cut region from src */
    for ( k = -z0 ; k <= z1 ; ++k )
    {
        for ( j = -y0 ; j <= y1 ; ++j )
        {
            for ( i = -x0 ; i <= x1 ; ++i )
            {
                *( dst + i + j * d + k * d * d ) = (float)
                    *( src + i + j * w + k * w * h );
            }
        }
    }
}

/**************************************************************************************************/

void
mig_im_bb_cut_2d ( Mig16u *src ,
                   int w , int h ,
                   int cx , int cy ,
                   float *dst ,
                   int r )
{
    int x0 , x1;
    int y0 , y1;
    int d = 2 * r + 1;
    int i , j;

    /* zero output */
    memset ( dst , 0x00 , d * d * sizeof(float) );

    /* shift src to current pixel */
    src += cx + cy * w;

    /* shift dst to its central pixel */
    dst += r + r * d;

    /* take care of boundary conditions */
    x0 = ( ( cx - r ) < 0 )? cx : r;
    x1 = ( ( cx + r ) >= w )? w - cx : r;

    y0 = ( ( cy - r ) < 0 )? cy : r;
    y1 = ( ( cy + r ) >= h )? h - cy : r;

    /* cut region form src */
    for ( j = -y0 ; j < y1 ; ++j )
    {
        for ( i = -x0 ; i < x1 ; ++i )
        {
            *( dst + i + j * d ) = (float) *( src + i + j * w );
        }
    }
}

/**************************************************************************************************/

void
mig_im_bb_cut_2d_32f ( float *src , int w , int h , int cx , int cy , float *dst , int r )
{
    int x0 , x1;
    int y0 , y1;
    int d = 2 * r + 1;
    int i , j;

    /* zero output */
    memset ( dst , 0x00 , d * d * sizeof(float) );

    /* shift src to current pixel */
    src += cx + cy * w;

    /* shift dst to its central pixel */
    dst += r + r * d;

    /* take care of boundary conditions */
    x0 = ( ( cx - r ) < 0 )? cx : r;
    x1 = ( ( cx + r ) >= w )? w - cx : r;

    y0 = ( ( cy - r ) < 0 )? cy : r;
    y1 = ( ( cy + r ) >= h )? h - cy : r;

    /* cut region form src */
    for ( j = -y0 ; j < y1 ; ++j )
    {
        for ( i = -x0 ; i < x1 ; ++i )
        {
            *( dst + i + j * d ) = *( src + i + j * w );
        }
    }
}

/***************************************/
/* PRIVATE FUNCTIONS */
/***************************************/
static void
_cut_8u_2d ( Mig8u *src , 
             Mig8u *dst , 
             mig_size_t *s ,
             mig_roi_t *bb )
{
        int j;
        int size = bb->w * sizeof(Mig8u);
        
        /* shift source image to first bb coordinate */
        src += ( bb->x0 ) + ( bb->y0 ) * ( s->w );

        for ( j = 0 ; j < bb->h ; ++ j ,
              src += s->w , dst += bb->w )
        {
                mig_memcpy ( src , dst , size );
        }
}

/***************************************/
static void
_cut_8u_3d ( Mig8u *src , 
             Mig8u *dst , 
             mig_size_t *s ,
             mig_roi_t *bb )
{
        int k;

        /* shift source image to first bb coordinate */
        src += ( bb->z0 ) * ( s->dim );
        
        for ( k = 0 ; k < bb->z ; ++ k ,
              src += s->dim , dst += ( bb->w ) * ( bb->h ) )
        {
                _cut_8u_2d ( src , dst , s , bb );
        }
}

/***************************************/
static void
_cut_16u_2d ( Mig16u *src , 
              Mig16u *dst , 
              mig_size_t *s ,
              mig_roi_t *bb )
{
        int j;
        int size = bb->w * sizeof(Mig16u);
        
        /* shift source image to first bb coordinate */
        src += ( bb->x0 ) + ( bb->y0 ) * ( s->w );

        for ( j = 0 ; j < bb->h ; ++ j ,
              src += s->w , dst += bb->w )
        {
                mig_memcpy ( src , dst , size );
        }
}

/***************************************/
static void
_cut_16u_3d ( Mig16u *src , 
              Mig16u *dst , 
              mig_size_t *s ,
              mig_roi_t *bb )
{
        int k;

        /* shift source image to first bb coordinate */
        src += ( bb->z0 ) * ( s->dim );
        
        for ( k = 0 ; k < bb->z ; ++ k ,
              src += s->dim , dst += ( bb->w ) * ( bb->h ) )
        {
                _cut_16u_2d ( src , dst , s , bb );
        }
}
