#include "mig_im_thr.h"
#include "mig_ut_mem.h"
#include "mig_ut_bit.h"

#include "mig_im_kernel.h"
#include "mig_im_conv.h"

#include "mig_im_bb.h"
#include "mig_error_codes.h"

/**************************************************/
typedef struct
{
        double  *h;     /* histogram            */
        int     g0;     /* start gray level -> h[0]     */
        int     g1;     /* end gray level  -> h[MAX]    */
        double  n;      /* total number of samples      */
        double  s;      /* total sum of samples */
} hst;

/**************************************************/
static int
hist ( const Mig16u *im , int s , hst *h )
{
    int i, val;

    /* allocate data just for interval [g0 , g1] */
    h->h = (double*) calloc ( h->g1 - h->g0 + 1 , sizeof ( double ) );
    if ( !( h->h ) )
        return MIG_ERROR_MEMORY;

    for ( i = 0 ; i < s ; ++i , ++im )
    {
        val = (double) *im;
        if ( ( val >= h->g0 ) && ( val <= h->g1 ) )
        /* we are in the wanted interval */
        {
            /* histogram */
            h->h[val - h->g0] ++ ;

            /* total samples */
            h->n ++ ;

			/* total sum of values normalized to interval [0;g1-g0] */
            h->s += ( val - h->g0 );
        }
    }

    return MIG_OK;
}

/**************************************************/
/* Ridler + Cavalard */
int
mig_im_thr ( Mig16u *im , int s , int g0 , int g1 , int *thr )
{
    hst     h;
    int     i;
    double  t0, t1;
    double  m_0, n_0;
    double  m_1, n_1;
    int rc = MIG_OK;

    h.g0 = g0; 
    h.g1 = g1;
    h.n = h.s = 0.0;

    if ( hist ( im , s , &h ) != MIG_OK )
        return MIG_ERROR_MEMORY;

    if ( h.n == 0 )
        return MIG_ERROR_INTERNAL;

    /* initial threshold */
    t0 = (double) *thr;

    do
    {
        /* calculate first cluster */
        m_0 = n_0 = 0.0;
        for ( i = 0 ; i <= t0 ; ++ i )
        {
            m_0 += i * h.h[i];
            n_0 += h.h[i];
        }

        /* calculate second cluster */
        m_1 = h.s - m_0;
        n_1 = h.n - n_0;

        if ( ( n_0 == 0 ) || ( n_1 == 0 ) )
        {
            rc = MIG_ERROR_INTERNAL;
            goto out;
        }

        m_0 /= n_0;
        m_1 /= n_1;

        t1 = t0;
        t0 = ( ( m_0 + m_1 ) / 2.0 ) + 0.5;

    } while ( ( abs( t0 - t1 ) ) > 1.0 );

    *thr = t0 + g0;

out :

    free ( h.h );
    return rc;
}

/*************************************************************************/
void
mig_im_thr_8u_i ( Mig8u *im , int s , int thr )
{
        int i;

        for ( i = 0 ; i < s ; ++i , ++im )
                *im = ( *im < thr ) ? 0x00:0xFF;
}


/*************************************************************************/
void
mig_im_thr_16u_inv ( Mig16u *im ,
                     Mig8u *msk ,
                     int s ,
                     int thr )
{
        int i;

        for ( i = 0 ; i < s ; ++i , ++msk , ++im )
                *msk = ( *im < thr ) ? 0xFF : 0x00;
}

/*************************************************************************/
void
mig_im_thr_32f_i_val ( Mig32f *im ,
                       int s ,
                       float thr )
{
	int i;

        for ( i = 0 ; i < s ; ++i , ++im )
                *im = ( *im < thr ) ? 0.0f : *im;
}




/*TODO UNFINISHED, FIND FAST IMPLEMENTATION! */

int
mig_im_thr_32f_3d_local_mean ( Mig32f *in , int w , int h , int z , int radius )
{

	/*TODO: Si può fare facendo 3 convoluzioni 1-d 1 1 1 1 1 normalizzate, e poi sottraendo membro a membro */
	
	
	/* AAA: 2 buffers are needed! */

	int i,j,k;
	Mig32f *pIm = in;
	Mig32f *buf1;
	Mig32f *buf2;
	Mig32f *pBuf = NULL;

	int rc = MIG_OK;

	mig_kernel_t *kernel = mig_im_kernel_get_mean_1d( radius );


	buf1 = (Mig32f*) calloc ( w * h * z, sizeof(Mig32f) );
	if ( buf1 == NULL )
	{
		mig_im_kernel_delete ( kernel );
		return MIG_ERROR_MEMORY;
	}

	buf2 = (Mig32f*) calloc ( w * h * z, sizeof(Mig32f) );
	if ( buf2 == NULL )
	{
		mig_im_kernel_delete ( kernel );
		free ( buf1 );
		return MIG_ERROR_MEMORY;
	}
	/* convolution in x */
    for ( j = 0 ; j < z ; ++j )
    {
        for ( i = 0 ; i < h ; ++i )
        {
            rc = mig_im_conv_1d_x ( in + i * w + j * w * h , 
                    buf2 + i * w + j * w * h , w , kernel );
        }
    }

    /* convolution in y */
    for ( j = 0 ; j < z ; ++j )
    {
        for ( i = 0 ; i < w ; ++i )
        {
            rc = mig_im_conv_1d_y ( buf2 + i + j * w * h ,
                    buf1 + i + j * w * h , w , h , kernel );
        }
    }

    /* convolution in z */
    for ( j = 0 ; j < h ; ++j )
    {
        for ( i = 0 ; i < w ; ++i )
        {
            rc = mig_im_conv_1d_z ( buf1 + i + j * w ,
                    buf2 + i + j * w , w , h , z , kernel );
        }
    }

	/*  threshold using mean: retain if value > mean*/

	pBuf = buf2;

	for ( k = 0; k < z; ++k )
	{
		for ( j = 0; j < h; ++j )
		{
			for ( i = 0; i < w; ++i )
			{
				if ( *pIm < ( *pBuf ) )
				{
					
					*pIm = 0.0f;
				}
			
				++pIm;
				++pBuf;
			}

		}

	}
	
	free (buf1);
	free (buf2);
	mig_im_kernel_delete ( kernel );
	return rc;

}