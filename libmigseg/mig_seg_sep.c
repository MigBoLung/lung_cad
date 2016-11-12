#include "mig_seg_sep.h"

#include "libmigut.h"
#include "libmigst.h"
#include "libmigim.h"

/****************************************/
/* private functions */
/****************************************/

MIG_C_LINKAGE_START

static int
_extrema ( float *profile ,
           int s ,
           float thr ,
           int range ,
           int idx[3] ,
           float val[3] );

static void
_flex_points ( float *profile ,
               int s ,
               int idx[2] ,
               float val[2] );

static int
_dt ( Mig8u *im ,
      int w , int h ,
      int idx[5] ,
      Mig16u *dt );

static void
_path ( Mig16u *dt ,
        int w , int h ,
        int idx[3] ,
        Mig8u *msk );

static int
_reg_cmp ( const void *a ,
           const void *b );

MIG_C_LINKAGE_END

/****************************************/
/* exported functions */
/****************************************/
int
mig_seg_sep ( Mig8u *accum ,
              int w , int h ,
              Mig8u *msk_l ,
              Mig8u *msk_r )
{
        int rc , dim;

        float extrema_val[3] = { 0.0 , 0.0 , 0.0 };
        int   extrema_idx[3] = { 0 , 0 , 0 };
        float flexpts_val[2] = { 0.0 , 0.0 };
        int   flexpts_idx[2] = { 0 , 0 };

        float *h_profile = NULL;
        float *v_profile = NULL;

        Mig16u *dt = NULL;

        int dtidx[5] = { 0 , 0 , 0 , 0 , 0 };
        int mskidx[5] = { 0 , 0 , 0 };

        mig_lst_t *props = NULL;
        mig_reg_props_t *prop_1 , *prop_2;
        int cc = 0 , lab_l , lab_r;

        if ( ( accum == NULL ) ||
             ( msk_l == NULL ) ||
             ( msk_r == NULL ) ||
             ( w <= 0 ) ||
             ( h <= 0 ) )
                return MIG_ERROR_PARAM;

        h_profile = (float*)
                calloc ( w , sizeof(float) );
        if ( !h_profile )
        {
                rc = MIG_ERROR_MEMORY;
                goto error;
        }

        v_profile = (float*)
                calloc ( h , sizeof(float) );
        if ( !v_profile )
        {
                rc = MIG_ERROR_MEMORY;
                goto error;
        }

        /* generate horizontal and vertical
           profiles */
        mig_im_alg_profile_h_8u ( accum , w , h , h_profile );
        mig_im_alg_profile_v_8u ( accum , w , h , v_profile );

        _extrema ( h_profile , w , 5 , 90 ,
                   extrema_idx , extrema_val );

        _flex_points ( v_profile , h ,
                       flexpts_idx , flexpts_val );

        dim = w * h;

        dt = (Mig16u*)
                mig_calloc ( dim , sizeof(Mig16u) );
        if ( dt == NULL )
        {
                rc = MIG_ERROR_MEMORY;
                goto error;
        }

        dtidx[0] = extrema_idx[2];
        dtidx[1] = extrema_idx[0];
        dtidx[2] = extrema_idx[1];
        dtidx[3] = flexpts_idx[0];
        dtidx[4] = flexpts_idx[1];

        rc = _dt ( accum , w , h , dtidx , dt );
        if ( rc != MIG_OK )
                goto error;

        mskidx[0] = extrema_idx[2];
        mskidx[1] = flexpts_idx[0];
        mskidx[2] = flexpts_idx[1];

        /* produce a binary image from accumulator */
        mig_im_thr_8u_i ( accum , dim  , 1 );

        /* use left mask for temporary storage trace path */
        _path ( dt , w , h , mskidx , msk_l );

        /* invert mask */
        mig_im_bin_neg_i ( msk_l , dim );

        /* and the masks -> result is in accum */
        mig_im_bin_and ( msk_l , accum , dim );

        /* zero left mask */
        mig_memz_fast ( msk_l , dim * sizeof(Mig8u) );

        /* label accumulator */
        mig_im_lab_trace_2d ( accum , w , h , &cc );

        /* calculate region properties -> centroids */
        rc = mig_reg_prop_sep ( accum , w , h , &props , cc );

        /* sort regions based on size in decreasing order */
        mig_lst_sort ( props , &_reg_cmp );

        /* there should be two regions so
           the one that has the smaller
           centroid horizontal coordinate
           is the right lung, the other is
           the left lung. */

        prop_1 = (mig_reg_props_t*)
                        mig_lst_get_head ( props );
        if ( prop_1 == NULL )
        {
                rc = MIG_ERROR_MEMORY;
                goto error;
        }

        prop_2 = (mig_reg_props_t*)
                        mig_lst_get_head ( props );
        if ( prop_2 == NULL )
        {
                rc = MIG_ERROR_MEMORY;
                goto error;
        }

        if ( prop_1->centroid[0] < prop_2->centroid[0] )
        {
                lab_r = prop_1->id;
                lab_l = prop_2->id;
        }
        else
        {
                lab_r = prop_2->id;
                lab_l = prop_1->id;
        }

        free ( prop_1 );
        free ( prop_2 );

        /* switch on right lung label */
        mig_im_lab_on ( accum , msk_r , dim , lab_r );

        /* switch on left lung label */
        mig_im_lab_on ( accum , msk_l , dim , lab_l );

        free ( h_profile );
        free ( v_profile );
        mig_free ( dt );
        mig_lst_free ( props );

        return MIG_OK;

error :
        if ( h_profile )
                free ( h_profile );

        if ( v_profile )
                free ( v_profile );

        if ( dt )
                mig_free ( dt );

        if ( props )
                mig_lst_free ( props );

        return rc;
}

/****************************************/
/* private functions */
/****************************************/
static int
_extrema ( float *profile ,
           int s ,
           float thr ,
           int range ,
           int idx[3] ,
           float val[3] )
{
        int i , cnt , tmp;
        unsigned char *diff = NULL;

        /* global maximum */
        for ( i = 0 ; i < s ; ++i )
        {
                if ( profile[i] > val[0] )
                {
                        val[0] = profile[i];
                        idx[0] = i;
                }
        }

        diff = (unsigned char*)
		malloc ( ( s - 1 ) * sizeof(unsigned char) );
        if ( diff == NULL )
                return MIG_ERROR_MEMORY;

        /* difference between adjacent pairs */
        for ( i = 0 ; i < s - 1 ; ++i )
                diff[i] =
                  ( fabs(profile[i]-profile[i+1])>thr)?0x01:0x00;

        /* perform forward search */
        i = MIG_CLIP( idx[0] + range , 0 , s - 1 );
        cnt = 0;

        while ( ( ( diff[i] != 0x01 ) ||
                  ( cnt <= 10 ) ) &&
                  ( i < s - 1 ) )
        {
                ++i ;
                ++cnt ;
        }

        /* second maximum - 1 possibility */
        for ( ; i < s ; ++i )
        {
                if ( profile[i] > val[1] )
                {
                        val[1] = profile[i];
                        idx[1] = i;
                }
        }

        /* perform backward search */
        i = MIG_CLIP( idx[0] - range , 0 , s - 1 );
        cnt = 0;

        while ( ( ( diff[i] != 0x01 ) ||
                  ( cnt <= 10  ) ) &&
                  ( i > 0 ) )
        {
                -- i ;
                ++ cnt;
        }

        /* second maximum - 2 possibility */
        for ( ; i >= 0 ; --i )
        {
                if ( profile[i] > val[2] )
                {
                        val[2] = profile[i];
                        idx[2] = i;
                }
        }

        /* choose backward or forward maximum */
        if ( val[2] > val[1] )
        {
                val[1] = val[2];
                idx[1] = idx[2];
        }

        /* put maxima so that
           the first has the lower
           index */
        if ( idx[0] > idx[1] )
        {
                tmp = idx[0];
                idx[0] = idx[1];
                idx[1] = tmp;

                tmp = val[0];
                val[0] = val[1];
                val[1] = tmp;

        }

        /* minimum between the two maxima */
        val[2] = MIG_MAX_32F;
        for ( i = idx[0] + 1 ; i < idx[1] ; ++i )
        {
                if ( profile[i] < val[2] )
                {
                        val[2] = profile[i];
                        idx[2] = i;
                }
        }

	/*
        idx[2] = ( idx[0] + idx[1] ) >> 1;
        val[2] = profile[idx[2]];
        */

        free ( diff );
        return MIG_OK;
}

/****************************************/
static void
_flex_points ( float *profile , int s ,
                  int idx[2] ,
                  float val[2] )
{
        /* forward search */
        int i = 0;
        while ( profile[i] <= 1.0 )
                ++ i;
        idx[0] = i;
        val[0] = profile[i];

        /* backward search */
        i = s - 1;
        while ( profile[i] <= 1.0 )
                -- i;
        idx[1] = i;
        val[1] = profile[i];

}

/****************************************/
#define MAX_PATH_VAL    MIG_MAX_16U

static int
_dt ( Mig8u *im ,
         int w , int h ,
         int idx[5] ,
         Mig16u *dt )
{

        int i , j;
        int x0 , x1;
        Mig16u *d = NULL;
        int val;

        d = (Mig16u*)
                mig_malloc ( w * h * sizeof(Mig16u) );
        if ( d == NULL )
                return MIG_ERROR_MEMORY;

        /* set all distances to Inf */
        mig_memset ( d  , 0xFF , w * h * sizeof(Mig16u) );
        mig_memset ( dt , 0xFF , w * h * sizeof(Mig16u) );

        /* set starting pixels to 0 */
        d[  idx[0] + idx[3] * w ] = 0x00;
        dt[ idx[0] + idx[4] * w ] = 0x00;

        /* downward distance transform */
        x0 = idx[0] - 1;
        x1 = idx[0] + 1;

        for ( j = idx[3] + 1 ; j <= idx[4] ; ++j )
        {
                for ( i = x0 ; i <= x1 ; ++i )
                {
                        val = ((int)im[i+j*w]) + ((int)
                                MIG_MIN3( d[i-1+(j-1)*w] ,
                                          d[i  +(j-1)*w] ,
                                          d[i+1+(j-1)*w]));
                        d[i+j*w] = (val > MAX_PATH_VAL ) ?
                                MAX_PATH_VAL : (Mig16u)val;
                }

                if ( x0 > idx[1] )
                        -- x0;

                if ( x1 < idx[2] )
                        ++ x1;
        }

        /* upward distance transform */
        x0 = idx[0] - 1;
        x1 = idx[0] + 1;

        for ( j = idx[4] - 1 ; j >= idx[3] ; --j )
        {
                for ( i = x0 ; i <= x1 ; ++i )
                {
                        val =((int)im[i+j*w]) + ((int)
                                MIG_MIN3( dt[i-1+(j+1)*w] ,
                                          dt[i  +(j+1)*w] ,
                                          dt[i+1+(j+1)*w]));
                        dt[i+j*w] = (val > MAX_PATH_VAL ) ?
                                MAX_PATH_VAL : (Mig16u)val;
                }

                if ( x0 > idx[1] )
                        -- x0;

                if ( x1 < idx[2] )
                        ++ x1;
        }

        /* result is sum of downward
           and upward transform */
        for ( i = 0 ; i < w * h ; ++i )
        {
                val = ((int)dt[i]) + ((int)d[i]);
                dt[i] = ( val > MAX_PATH_VAL ) ?
                        MAX_PATH_VAL : (Mig16u)val;
        }

        mig_free ( d );
        return MIG_OK;
}

/****************************************/
static void
_path ( Mig16u *dt , int w , int h , int idx[3] , Mig8u *msk )
{
        int i , curr_v , curr_h;
        Mig16u min;

        curr_h = idx[0];
        curr_v = idx[1];

        while ( curr_v <= idx[2] )
        {
                msk[curr_v*w+curr_h] =
                msk[curr_v*w+curr_h-1] =
                msk[curr_v*w+curr_h+1] = 0XFF;

                min = MIG_MIN3( dt[(curr_v+1)*w+curr_h-1] ,
                                dt[(curr_v+1)*w+curr_h+0] ,
                                dt[(curr_v+1)*w+curr_h+1]);

                if ( min == dt[(curr_v+1)*w+curr_h] )
                        i = 0;
                else
                {
                        if ( min == dt[(curr_v+1)*w+curr_h-1] )
                                i = -1;
                        else
                                i = +1;
                }

                curr_h += i;
                curr_v ++;
        }
}

/***********************************************************/
static int
_reg_cmp ( const void *a , const void *b )
{
        mig_lst_node *node1 = *(( mig_lst_node** ) a );
        mig_lst_node *node2 = *(( mig_lst_node** ) b );
        mig_reg_props_t *tmp1 = ( mig_reg_props_t* ) node1->data;
        mig_reg_props_t *tmp2 = ( mig_reg_props_t* ) node2->data;

        if ( ( tmp1->area ) == ( tmp2->area ) )
                return 0;

        if ( ( tmp1->area ) > ( tmp2->area ) )
                return -1;

        return 1;
}

