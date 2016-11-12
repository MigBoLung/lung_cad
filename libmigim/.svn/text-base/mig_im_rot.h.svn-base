#ifndef __MIG_IM_ROT_H__
#define __MIG_IM_ROT_H__

#include "mig_config.h"
#include "mig_defs.h"

#include "mig_data_types.h"

MIG_C_LINKAGE_START

typedef enum
{
        MIG_NEAREST_NEIGHBOR = 0,
        MIG_BILINEAR = 1,
        MIG_BICUBIC = 2

} MigInterpType;


extern void
mig_im_rot_8u ( Mig8u *src , Mig8u *dst ,
                int w , int h ,
                Mig64f theta ,
                Mig64f rx , Mig64f ry ,
                MigInterpType ip );

MIG_C_LINKAGE_END

#endif /* __MIG_IM_ROT_H__ */
