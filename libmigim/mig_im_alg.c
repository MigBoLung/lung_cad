#include "mig_im_alg.h"

#if !defined(SSE2)		/* not SSE2 */

/*******************************************************************/
void
mig_im_alg_add_bin ( const Mig8u *src , Mig8u *dst , int s )
{
        int i;
        for ( i = 0 ; i < s ; ++ i , ++src , ++dst )
        {
        	if ( *dst == 0xFF ) /* overflow -> jump */
        		continue;
                *dst += (*src) & 0x01;
        }
}

/*******************************************************************/
#else /* SSE2 */
/*******************************************************************/

void
mig_im_alg_add_bin ( const Mig8u *src , Mig8u *dst , int s )
{
        int i;

        __m128i *idx_src = (__m128i*) src;
        __m128i *idx_dst = (__m128i*) dst;
        __m128i xmm0 , xmm1 , xmm2;

        xmm0 = _mm_set1_epi8 ( 0x01 );
        for ( i = 0 ; i < ( s >> 4 )  ; ++ i , ++idx_src , ++idx_dst )
        {
                _mm_prefetch ( (char*) idx_src ,  _MM_HINT_NTA );
                _mm_prefetch ( (char*) idx_dst ,  _MM_HINT_T0  );

                xmm1 = _mm_load_si128 ( idx_src );
                xmm2 = _mm_load_si128 ( idx_dst );

                xmm1 = _mm_and_si128 ( xmm0 , xmm1 );
                xmm1 = _mm_add_epi8  ( xmm1 , xmm2 );

                _mm_store_si128 ( idx_dst , xmm1 );
        }
}

#endif  /* SSE2 */

/*******************************************************************/
void
mig_im_alg_profile_h_8u ( const Mig8u *src , int w , int h , float *profile )
{
        int i , j;

        for ( i = 0 ; i < w ; ++i )
        {
                for ( j = 0 ; j < h ; ++j )
                        *profile += *( src + i + j * w );
                ++ profile;
        }
}

/*******************************************************************/
void
mig_im_alg_profile_v_8u ( const Mig8u *src , int w , int h , float *profile )
{
        int i , j;

        for ( j = 0 ; j < h ; ++j )
        {
                for ( i = 0 ; i < w ; ++i , src++ )
                        *profile += *src;
                ++ profile;
        }
}

/*******************************************************************/
void
mig_im_alg_constmul ( Mig32f *src , int s , Mig32f factor )
{
        int i;

        for ( i = 0 ; i < s ; ++i , ++src )
                *src *= factor;
}
