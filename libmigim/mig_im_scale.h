#ifndef __MIG_IM_SCALE_H__
#define __MIG_IM_SCALE_H__

#include "mig_config.h"
#include "mig_defs.h"
#include "mig_data_types.h"
#include "mig_data_image.h"
#include "mig_error_codes.h"



MIG_C_LINKAGE_START


/**
        Perform "whitening" scaling subtracting mean
		and dividing by (std * nsigma) for each dimension

*/

void
mig_im_scale_whitening ( float *Src , float *Dst , float nsigma,
                     int len , float* mean , float* std );

void
mig_im_scale_whitening_inplace ( float *Src , float nsigma , 
						int len , float *mean , float *std );
MIG_C_LINKAGE_END

#endif /* __MIG_IM_SCALE_H__ */

