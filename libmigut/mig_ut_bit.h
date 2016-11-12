#ifndef __MIG_UT_BIT_H__
#define __MIG_UT_BIT_H__

#include "mig_config.h"
#include "mig_defs.h"
#include "mig_data_types.h"

/**************************************************************************/
/* access to 32 bit buffers */
/**************************************************************************/

/**************************************************************************/
/* 1 bit access to 32 bit buffer */
#define  mig_get_32_1(pdata,n) \
        ((*((pdata) + ((n) >> 5) ) >> (31 - ((n) & 31))) & 1)

#define  mig_set_32_1(pdata,n) \
        (*((pdata) + ((n) >> 5)) |= (0x80000000u >> ((n) & 31)))

#define  mig_clear_32_1(pdata,n) \
        (*((pdata) + ((n) >> 5)) &= ~(0x80000000u >> ((n) & 31)))

/**************************************************************************/
/* 8 bit access to 32 bit buffer */
#if defined(MIG_LITTLE_ENDIAN)  /* LITTLE ENDIAN PLATFORM */

#define  mig_get_32_8(pdata,n) \
        (*(Mig8u*)((uintptr_t)((Mig8u*)(pdata) + (n)) ^ 3))

#define  mig_set_32_8(pdata,n,val) \
        (*(Mig8u*)((uintptr_t)((Mig8u*)(pdata) + (n)) ^ 3) = (val))

#else                           /* BIG ENDIAN PLATFORM */

#define  mig_get_32_8(pdata,n) \
        (*((Mig8u*)(pdata) + (n)))

#define  mig_set_32_8(pdata,n,val) \
        (*((Mig8u*)(pdata) + (n)) = (val))
#endif                                  /* END ENDIANNESS */

/**************************************************************************/
/* 16 bit access to 32 bit buffer */
#if defined(MIG_LITTLE_ENDIAN)  /* LITTLE ENDIAN PLATFORM */

#define  mig_get_32_16(pdata,n) \
        (*(Mig16u*)((uintptr_t)((Mig16u*)(pdata) + (n)) ^ 2))

#define  mig_set_32_16(pdata,n,val) \
        (*(Mig16u*)((uintptr_t)((Mig16u*)(pdata) + (n)) ^ 2) = (val))

#else                           /* BIG ENDIAN PLATFORM */

#define  mig_get_32_16(pdata,n) \
        (*((Mig16u*)(pdata) + (n)))

#define  mig_set_32_16(pdata,n,val) \
        (*((Mig16u*)(pdata) + (n)) = (val))

#endif                                  /* END ENDIANNESS */


/**************************************************************************/
/* access to 8 bit buffers */

/* 1 bit access to 8 bit buffer */
#define  mig_get_8_1(pdata,n) \
        ((*((pdata) + ((n) >> 3) ) >> (7 - ((n) & 7))) & 1)

#define  mig_set_8_1(pdata,n) \
        (*((pdata) + ((n) >> 3)) |= (0x80 >> ((n) & 7)))

#define  mig_clear_8_1(pdata,n)  \
        (*((pdata) + ((n) >> 3)) &= ~(0x80 >> ((n) & 7)))

/**************************************************************************/
MIG_C_LINKAGE_START

extern void
mig_pack_32_1 ( Mig8u *src , Mig32u *dst , int s );

extern void
mig_upack_32_1 ( Mig32u *src , Mig8u *dst , int s );

extern void
mig_pack_32_8 ( Mig8u *src , Mig32u *dst , int s );

extern void
mig_upack_32_8 ( Mig32u *src , Mig8u *dst , int s );

extern void
mig_pack_32_16 ( Mig16u *src , Mig32u *dst , int s );

extern void
mig_upack_32_16 ( Mig32u *src , Mig16u *dst , int s );

extern void
mig_pack_8_1 ( Mig8u *src , Mig8u *dst , int s );

extern void
mig_upack_8_1 ( Mig8u *src , Mig8u *dst , int s );

extern void
mig_swp_byte ( Mig8u *src , unsigned int size );

extern void
mig_bit_dump ( Mig8u *src , unsigned int size );

MIG_C_LINKAGE_END

#endif /* __MIG_UT_BIT_H__ */

