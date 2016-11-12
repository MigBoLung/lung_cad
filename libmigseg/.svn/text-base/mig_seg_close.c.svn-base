#include "mig_seg_close.h"

#include "libmigut.h"
#include "libmigim.h"

/*******************************************/
/* PRIVATE FUNCTIONS */
/*******************************************/
static void
_copy_to_buff ( Mig8u *src ,
                Mig8u *dst ,
                int coord[2] ,
                int dim[3] );

static void
_copy_from_buff ( Mig8u *src ,
                  Mig8u *dst ,
                  int coord[2] ,
                  int dim[3] );

/*******************************************/
/* EXPORTED */
/*******************************************/
int
mig_seg_close ( Mig8u *src , mig_size_t *s , int r )
{
        int i , w , h , size;
        int coord[2] , dim[3];
        Mig8u *buff = NULL;
        sel_t sel;

        if ( encode_disk ( r , &sel ) )
                return MIG_ERROR_MEMORY;        
       
        /* start coordinates inside
           destination buffer */
        coord[0] = coord[1] = 2 * r + 2;
        
        /* temporary buffer dimensions */
        w = s->w + 2 * ( coord[0] + 1 );
        h = s->h + 2 * ( coord[1] + 1 );

        dim[0] = s->w;  /* original width */
        dim[1] = s->h;  /* original height */
        dim[2] = w;     /* new, bigger buffer width */

        size = w * h * sizeof( Mig8u ); /* size in bytes of new buffer */
        buff = (Mig8u*)
                mig_malloc ( size );
        if ( !buff )
                goto error;

        for ( i = 0 ; i < s->slices ; ++ i , src += s->dim )
        {
                /* zero temporary buffer as it is reused during each
                   iteration */
                mig_memz_fast ( buff , size );
                
                /* copy input data to bigger buffer */
                _copy_to_buff ( src , buff , coord , dim );

                /* perform operations on bigger buffer */
                //mig_im_mor_dilate_disk ( buff , w , h , r , MIG_DISK_FULL );
                //mig_im_mor_erode_disk  ( buff , w , h , r , MIG_DISK_FULL );

                /* perform closing */
                if ( mclose ( buff , w , h , &sel ) )
                        goto error;

                /* copy data back to output buffer */
                _copy_from_buff ( buff , src , coord , dim );
        }

        mig_free ( buff );
        free_sel ( &sel );

        return MIG_OK;

error :

        if ( buff )
                mig_free ( buff );
        
        free_sel ( &sel );
        
        return MIG_ERROR_MEMORY;
}

/*******************************************/
/* PRIVATE FUNCTIONS */
/*******************************************/
static void
_copy_to_buff ( Mig8u *src , Mig8u *dst ,
                int coord[2] , int dim[3] )
{
        int j;
        int size = dim[0] * sizeof(Mig8u);
        
        /* position dst at starting coordinates */
        dst += coord[0] + coord[1] * dim[2];
        
        /* copy on line at a time */
        for ( j = 0 ; j < dim[1] ; ++ j ,
              src += dim[0] , dst += dim[2] )
        {
                mig_memcpy ( src , dst , size );
        }
}

/*******************************************/
static void
_copy_from_buff ( Mig8u *src , Mig8u *dst ,
                  int coord[2] , int dim[3] )
{
        int j;
        int size = dim[0] * sizeof(Mig8u);

        /* position src at starting coordinates */
        src += coord[0] + coord[1] * dim[2];
        
        /* copy on line at a time */
        for ( j = 0 ; j < dim[1] ; ++ j ,
              src += dim[2] , dst += dim[0] )
        {
                mig_memcpy ( src , dst , size );
        }
}

