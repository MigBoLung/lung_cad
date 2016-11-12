#ifndef __MIG_IM_THR_H__
#define __MIG_IM_THR_H__

#include "mig_config.h"
#include "mig_defs.h"
#include "mig_data_types.h"

MIG_C_LINKAGE_START

/* thresholding function */
typedef int (*mig_thr_f) ( const Mig16u* , int , int , int , int* );

int
mig_im_thr ( Mig16u *im , int s , int g0 , int g1 , int *thr );

void
mig_im_thr_8u_i ( Mig8u *im , int len , int thr );

void
mig_im_thr_16u_inv ( Mig16u *im , Mig8u *msk , int s , int thr );

void
mig_im_thr_32f_i_val ( Mig32f *im , int s , float thr );

int
mig_im_thr_32f_3d_local_mean ( Mig32f *in , int w , int h , int z , int radius );

MIG_C_LINKAGE_END

#endif /* __MIG_IM_THR_H__ */
