#ifndef __MIG_IM_POLAR_H__
#define __MIG_IM_POLAR_H__


#include "mig_config.h"
#include "mig_defs.h"

#include "mig_data_types.h"

MIG_C_LINKAGE_START

extern void
mig_im_polar (  float *src , float *dst , int w , int h, int nr , int nphy , float min_r, float max_r);

MIG_C_LINKAGE_END

#endif  /* __MIG_IM_TRANSFORM_H__ */
