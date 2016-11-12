#ifndef __MIG_IM_ALG_H__
#define __MIG_IM_ALG_H__

#include "mig_config.h"
#include "mig_defs.h"
#include "mig_data_types.h"

MIG_C_LINKAGE_START

void
mig_im_alg_add_bin ( const Mig8u *src , Mig8u *dst , int s );

void
mig_im_alg_profile_h_8u ( const Mig8u *src , int w , int h , float *profile );

void
mig_im_alg_profile_v_8u ( const Mig8u *src , int w , int h , float *profile );

void
mig_im_alg_constmul ( Mig32f *src , int s , Mig32f factor );

MIG_C_LINKAGE_END

#endif /* __MIG_IM_ALG_H__ */
