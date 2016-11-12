#ifndef __MIG_IM_GEOM_H__
#define __MIG_IM_GEOM_H__

#include "mig_config.h"
#include "mig_defs.h"
#include "mig_data_types.h"
#include "mig_data_image.h"
#include "mig_error_codes.h"

typedef enum
{
        NEAREST  = 0 ,
        BILINEAR = 1 ,
        BICUBIC  = 2

} InterpType;

MIG_C_LINKAGE_START

/**
        Perform image rotation by angle Theta using
        interpolation type Type.

        Rotation is performed around image central
        pixel.

        Only central portion of rotation is valid!

        Theta is in degrees. Positive
        Theta gives clockwise rotation.

        Type should be one of
        NEAREST, BILINEAR, BICUBIC.

        Src and Dst shuold have the same dimensions.
        One should only use valid central part of Dst.

        To test under MATLAB (mex file) compile using
        mex -DMATLAB_ROT.
*/

void
mig_im_geom_rotate ( float *Src , float *Dst ,
                     int Width , int Height ,
                     float Theta ,
                     InterpType Type );

/**
        Perform image scaling using
        interpolation type Type.

        Scaling is performed taking input
        image central pixel as center of
        coordinate system.

        Type should be one of
        NEAREST, BILINEAR, BICUBIC.

        To test under MATLAB (mex file) compile using
        mex -DMATLAB_SCALE.
*/

void
mig_im_geom_resize ( float *Src ,
                     int SrcWidth ,
                     int SrcHeight ,
                     float *Dst ,
                     int DstWidth ,
                     int DstHeight ,
                     InterpType Type );


/** 
        Perform 3D image resampling in
        z linearly.
*/
int
mig_im_geom_resize_z ( Mig16u *Src ,
                       mig_size_t *SrcSize ,
                       Mig16u **Dst ,
                       mig_size_t *DstSize );

MIG_C_LINKAGE_END

#endif /* __MIG_IM_GEOM_H__ */

