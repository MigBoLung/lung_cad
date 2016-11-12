#ifndef __MIG_IM_PROJ_H__
#define __MIG_IM_PROJ_H__


#include "mig_config.h"
#include "mig_defs.h"
#include "mig_data_types.h"
#include "mig_error_codes.h"
#include "mig_data_types.h"

/*typedef enum proj_func {MIP,MinIP} proj_func;*/

MIG_C_LINKAGE_START

extern int
mig_im_proj_mip_axes_vol_32f(  float *src , float *dst , int w , int h, int d, int dir_idx, float ratio);

extern int
mig_im_proj_mip_z_stack_single(  float *src , float *dst , int w , int h, int d, int z, int radius);

extern int
mig_im_proj_mip_z_stack_whole( float *src , float *dst , int w , int h, int d, int radius);

MIG_C_LINKAGE_END

#endif  /* __MIG_IM_TRANSFORM_H__ */
