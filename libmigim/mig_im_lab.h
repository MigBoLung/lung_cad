#ifndef __MIG_IM_LAB_H__
#define __MIG_IM_LAB_H__

#include "mig_config.h"
#include "mig_defs.h"

#include "mig_data_types.h"

MIG_C_LINKAGE_START

extern void
mig_im_lab_on ( Mig8u *lab ,
                Mig8u *msk ,
                int s ,
                int id );

extern void
mig_im_lab_on_i ( Mig8u *lab ,
                  int s ,
                  int id );

extern void
mig_im_lab_trace_2d ( Mig8u *src ,
                      int w , int h ,
                      int *num_cc );

extern void
mig_im_lab_union_2d ( Mig8u *src ,
                      int w , int h ,
                      int *num_cc );

extern void
mig_im_lab_union_3d ( Mig8u *src ,
                      int w , int h , int z ,
                      int *num_cc );

extern int
mig_im_lab_union_3d_fast ( Mig8u *src ,
                           int w , int h , int z ,
                           int *num_cc );

MIG_C_LINKAGE_END

#endif /* __MIG_IM_LAB_H__ */
