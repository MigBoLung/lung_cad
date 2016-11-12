#include "mig_im_bin.h"

/*************************************************************************/
#define MIG_BORDER_LABEL 0x02

/*
        clear image border -> clear on pixels
        connected to image border.
        First all image border pixels are tagged.
        Than we perform successive sweeps over
        data in from top-left to bottom-right and
        than from bottom-right to top-left marking all
        pixels adjacent to a tagged pixel as tagged.
        Sweeps are performed until no further change
        in tags occurs.
        Finally all tagged pixels are switched off.
*/
void
mig_im_bin_clb_8u_i ( Mig8u *src , int w , int h , int conn )
{
        int i, j, k, changed;
        Mig8u *idx1, *idx2;
        int n_d[4], n_u[4]; /* neighbours up, neighbours down */

        /* take care of first and last row ->
           all on pixels are set to 0x02, i.e.
           MIG_BORDER_LABEL */
        idx1 = src;
        idx2 = src + w * ( h - 1 );
        for ( i = 0 ; i < w ; ++i , ++idx1 , ++idx2 )
        {
                *idx1 &= MIG_BORDER_LABEL;
                *idx2 &= MIG_BORDER_LABEL;
        }

        /* take care of first and last column ->
            all on pixels are set to 0x02, i.e.
            MIG_BORDER_LABEL */
        idx1 = src;
        idx2 = src + ( w - 1 );
        for ( i = 0 ; i < h ; ++i , idx1 += w , idx2 += w )
        {
		*idx1 &= MIG_BORDER_LABEL;
		*idx2 &= MIG_BORDER_LABEL;
        }

        /* setup neighbourhood */

        /* top left to bottom right 8 neighbours */
        n_d[0] = -w;
        n_d[1] = -1;
        n_d[2] = -w - 1;
        n_d[3] = -w + 1;

        /* bottom right to top left neighbours */
        n_u[0] = w;
        n_u[1] = 1;
        n_u[2] = w + 1;
        n_u[3] = w - 1;

        /* prcess image until no further changes occur */
        do
        {
                changed = 0;

                /* top left to bottom right */
                for ( j = 1 ; j < h - 1 ; ++j )
                {
                        for ( i = 1 ; i < w - 1 ; ++i )
                        {
                                idx1 = src + i + j * w;

                                        /* pixel is off -> continue */
                                if ( (*idx1 == MIG_BORDER_LABEL) ||
                                     (*idx1 == 0x00) )
                                        continue;
                                /* look at all neighbours
                                    given by connectivity */
                                for ( k = 0 ; k < ( conn >> 1 ) ; ++k )
                                {
                                        /* if at least one neighbouring pixel
                                            is labeled as connected to border
                                            label current pixel as connected
                                            to border too. */
                                        if ( *( idx1 + n_d[k] ) ==
                                        	MIG_BORDER_LABEL )
                                        {
                                                *idx1 = MIG_BORDER_LABEL;
                                                changed = 1;
                                                break;
                                        }
                                } /* for connected pixels */
                        } /* for image width */
                } /* for image height */

                /* bottom right to top left */
                for ( j = h - 2 ; j > 0 ; --j )
                {
                        for ( i = w - 2 ; i > 0 ; --i )
                        {
                                idx1 = src + i + j * w;

                                if ( (*idx1 == MIG_BORDER_LABEL) ||
                                     (*idx1 == 0x00) )
                                        continue;

                                for ( k = 0 ; k < ( conn >> 1 ) ; ++k )
                                {
                                        if ( *( idx1 + n_u[k] ) ==
                                        	MIG_BORDER_LABEL )
                                        {
                                                *idx1 = MIG_BORDER_LABEL;
                                                changed = 1;
                                                break;
                                        }
                                } /* for neighbours */
                        } /* for width */
                } /* for height */

        } while ( changed ); /* do ... while */

        /* final run over data ->
            turn pixels off if connected
            to image border */
        for ( i = 0 ; i < w * h ; ++i , ++src )
                if ( *src == MIG_BORDER_LABEL ) *src = 0x00;
}

/*************************************************************************/
/*
        fill holes in binary image -> holes are
        background pixels not connected to
        image border.
        First all image border pixels are tagged.
        Than we perform successive sweeps over
        data in from top-left to bottom-right and
        than from bottom-right to top-left marking all
        pixels adjacent to a tagged pixel as tagged.
        Sweeps are performed until no further change
        in tags occurs.
        Finally all off pixels which have not been
        tagged are switched on.
*/
void
mig_im_bin_fill_8u_i ( Mig8u *src , int w , int h , int conn )
{
        int i, j, k, changed;
        Mig8u *idx1, *idx2;
        int n_d[4], n_u[4];     /* neighbours up, neighbours down */

        /* take care of first and last row */
        idx1 = src;
        idx2 = src + w * ( h - 1 );
        for ( i = 0 ; i < w ; ++i , ++idx1 , ++idx2 )
        {
                if ( *idx1 == 0x00 )
                        *idx1 = MIG_BORDER_LABEL;
                if ( *idx2 == 0x00 )
                        *idx2 = MIG_BORDER_LABEL;
        }

        /* take care of first and last column */
        idx1 = src;
        idx2 = src + ( w - 1 );
        for ( i = 0 ; i < h ; ++i , idx1 += w , idx2 += w )
        {
                if ( *idx1 == 0x00 )
                        *idx1 = MIG_BORDER_LABEL;

                if ( *idx2 == 0x00 )
                        *idx2 = MIG_BORDER_LABEL;
        }

        /* setup neighbourhood */

        /* top left to bottom right 8 neighbours */
        n_d[0] = -w;
        n_d[1] = -1;
        n_d[2] = -w - 1;
        n_d[3] = -w + 1;

        /* bottom right to top left neighbours */
        n_u[0] = w;
        n_u[1] = 1;
        n_u[2] = w + 1;
        n_u[3] = w - 1;

        /* prcess image until no further changes occur */
        do
        {
                changed = 0;

                /* top left to bottom right */
                for ( j = 1 ; j < h - 1 ; ++j )
                {
                        for ( i = 1 ; i < w - 1 ; ++i )
                        {
                                idx1 = src + i + j * w;

                                if ( (*idx1 == MIG_BORDER_LABEL) ||
                                     (*idx1 == 0xFF) )
                                        continue;

                                for ( k = 0 ; k < ( conn >> 1 ) ; ++k )
                                {
                                        if ( *( idx1 + n_d[k] ) ==
                                        	MIG_BORDER_LABEL )
                                                                {
                                                *idx1 = MIG_BORDER_LABEL;
                                                changed = 1;
                                                break;
                                        }
                                } /* k */
                        } /* i */
                } /* j */

                /* bottom right to top left */
                for ( j = h - 2 ; j > 0 ; --j )
                {
                        for ( i = w - 2 ; i > 0 ; --i )
                                        {
                                idx1 = src + i + j * w;

                                if ( ( *idx1 == MIG_BORDER_LABEL ) ||
                                     ( *idx1 == 0xFF ) )
                                        continue;

                                for ( k = 0 ; k < ( conn >> 1 ) ; ++k )
                                {
                                        if ( *( idx1 + n_u[k] ) ==
                                        	MIG_BORDER_LABEL )
                                        {
                                                *idx1 = MIG_BORDER_LABEL;
                                                changed = 1;
                                                break;
                                        }
                                } /* k */
                        } /* i */
                } /* j */
        } while ( changed );

        /* final sweep over data - mask */
        for ( i = 0 ; i < w * h ; ++i , ++src )
        {
                if ( *src == MIG_BORDER_LABEL )
                        *src = 0x00;
                else
                        if ( *src == 0x00 )
                                *src = 0xFF;
        }
}

/*************************************************************************/
void
mig_im_bin_neg_i ( Mig8u *src , int s )
{
    int i;
    
    for ( i = 0 ; i < s ; ++i , ++src )
        *src = ~(*src);
}

/*************************************************************************/
void
mig_im_bin_and ( const Mig8u *src , Mig8u *dst , int s )
{
    int i;
    
    for ( i = 0 ; i < s ; ++ i , ++src , ++dst )
        *dst &= (*src);
}

/*************************************************************************/
void
mig_im_bin_msk_16u_i ( Mig16u *im , const Mig8u *msk , int s )
{
    int i;
    Mig8u *idx = (Mig8u*)im;

    for ( i = 0 ; i < s ; ++i , ++msk , idx+= 2 )
    {
        *idx &= *msk;
        *(idx+1) &= *msk;
    }
}
