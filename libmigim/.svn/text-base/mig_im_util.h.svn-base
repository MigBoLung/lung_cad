#ifndef __MIG_IM_UTIL_H__
#define __MIG_IM_UTIL_H__

#include "mig_config.h"
#include "mig_defs.h"
#include "mig_data_types.h"

MIG_C_LINKAGE_START

extern double
mig_im_util_h2g ( double h ,
                  double min , double max ,
                  double wc , double ww );

extern int
mig_im_util_isempty_32f ( Mig32f *src , int s );

extern void
mig_im_util_max_32f ( Mig32f *src , 
                      int s , 
                      Mig32f *max );

extern void
mig_im_util_min_max_32f ( Mig32f *src , 
                          int s , 
                          Mig32f *min , 
                          Mig32f *max );

extern void
mig_im_util_min_max_16u ( Mig16u *src , 
                          int s , 
                          Mig32f *min , 
                          Mig32f *max );

extern void
mig_im_util_mat2gray_32f ( Mig32f *src , 
                           int s , 
                           Mig32f min , 
                           Mig32f max );

extern void
mig_im_util_conv_16u_32f ( Mig16u *src ,
                           Mig32f *dst ,
                           int s );

extern void
mig_im_util_conv_32f_16u ( Mig32f *src , 
                           Mig16u *dst ,
                           int s );

extern void
mig_im_util_scale_32f_16u ( Mig32f *src , 
                            Mig16u *dst ,
                            int s );

extern void
mig_im_util_conv_32f_1u ( Mig32f *src ,
                          Mig8u *dst ,
                          int s );

extern void
mig_im_util_scale_32f_rgb ( Mig32f *src , 
                            unsigned char *rgb ,
                            int w , int h );

extern void
mig_im_util_scale_16u_rgb ( Mig16u *src , 
                            unsigned char *rgb ,
                            int w , int h );

MIG_C_LINKAGE_END

#endif /* __MIG_IM_UTIL_H__ */
