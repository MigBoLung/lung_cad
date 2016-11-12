#include "mig_ut_bit.h"

/*********************************************************************/
/* Buffer packing/unpacking -  32bit */

/*********************************************************************/
/* Pack 1 bit value buffer into 32 bit buffer */
void
mig_pack_32_1 ( Mig8u *src ,
                Mig32u *dst ,
                int s )
{
        unsigned int i;

        for ( i = 0 ; i < s ; ++i , ++src )
        {
                if ( *src == 0x00 )
                        mig_clear_32_1( dst , i );
                else
                        mig_set_32_1( dst , i );
        }
}

/*********************************************************************/
/* Unpack 1 bit value buffer from 32 bit buffer */
void
mig_upack_32_1 ( Mig32u *src ,
                 Mig8u *dst ,
                 int s )
{
        unsigned int i;

        for ( i = 0 ; i < s ; ++i ,++dst )
                *dst = mig_get_32_1( src , i );
}

/*********************************************************************/
/* Pack 8 bit buffer into 32 bit buffer */
void
mig_pack_32_8 ( Mig8u *src ,
	        Mig32u *dst ,
	        int s )
{
        int i;

        for ( i = 0 ; i < s ; ++i ,++src )
                mig_set_32_8( dst , i , *src );
}

/*********************************************************************/
/* Unpack 8 bit buffer from 32 bit buffer */
void
mig_upack_32_8 ( Mig32u *src ,
                 Mig8u *dst ,
                 int s )
{
        int i;

        for ( i = 0 ; i < s; ++i ,++dst )
                *dst = mig_get_32_8( src , i );
}

/*********************************************************************/
/* Pack 16 bit buffer into 32 bit buffer */
void
mig_pack_32_16 ( Mig16u *src ,
                 Mig32u *dst ,
                 int s )
{
        int i;

        for ( i = 0 ; i < s ; ++i , ++src )
                mig_set_32_16( dst , i , *src );
}

/*********************************************************************/
/* Unpack 16 bit buffer into 32 bit buffer */
void
mig_upack_32_16 ( Mig32u *src ,
                  Mig16u *dst ,
                  int s )
{
        int i;

        for ( i = 0 ; i < s ; ++i ,++dst )
                *dst = mig_get_32_16( src , i );
}

/*********************************************************************/
/* Buffer packing/unpacking -  8bit */

/*********************************************************************/
/* Pack 1 bit values into 8 bit buffer */
void
mig_pack_8_1 ( Mig8u *src ,
               Mig8u *dst ,
               int s )
{
        unsigned int i;

        for ( i = 0 ; i < s ; ++i ,++src )
        {
                if (*src == 0x00)
                        mig_clear_8_1( dst , i );
                else
                        mig_set_8_1( dst , i );
        }
}

/*********************************************************************/
/* Unpack 1 bit values from 8 bit buffer */
void
mig_upack_8_1 ( Mig8u *src ,
                Mig8u *dst ,
                int s )
{
        unsigned int i;

        for ( i = 0 ; i < s ; ++i ,++dst )
                *dst = mig_get_8_1( src , i );
}

/****************************************************************************/
/* swap all two consecutive bytes in buffer */
void
mig_swp_byte ( Mig8u *src ,
               unsigned int size )
{
        int i;
        Mig8u tmp;

        for ( i = 0 ; i < size ; i += 2 , src += 2 )
        {
                tmp = *src;
                *src = *(src+1);
                *(src+1) = tmp;
        }
}

/****************************************************************************/
/* dump buffer contents in hex to stdout */

#define FMT_HEX "%02x"
#define ROW_WIDTH       79

void
mig_bit_dump ( Mig8u *src ,
               unsigned int size )
{
        int i;

        for ( i = 0 ; i < size ; ++i )
        {
                printf ( FMT_HEX , *src++ );

                if ( (i+1) % (ROW_WIDTH) == 0 )
                        printf ( "\n" );
                else
                        printf ( " " );
        }

        printf ( "\n" );
        fflush ( stdout );
}

