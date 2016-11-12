#ifndef __MIG_IM_BIN_H__
#define __MIG_IM_BIN_H__

#include "mig_config.h"
#include "mig_defs.h"

#include "mig_data_types.h"

MIG_C_LINKAGE_START

extern void
mig_im_bin_neg_i ( Mig8u *src , int s );

extern void
mig_im_bin_and ( const Mig8u *src , Mig8u *dst , int s );

void
mig_im_bin_msk_16u_i ( Mig16u *im , const Mig8u *msk , int s );

void
mig_im_bin_clb_8u_i ( Mig8u *src , int w , int h , int conn );

void
mig_im_bin_fill_8u_i ( Mig8u *src , int w , int h , int conn );

MIG_C_LINKAGE_END

#endif /* __MIG_IM_BIN_H__ */
