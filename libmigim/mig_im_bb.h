#ifndef __MIG_IM_BB_H__
#define __MIG_IM_BB_H__

#include "mig_config.h"
#include "mig_defs.h"

#include "mig_data_types.h"
#include "mig_data_image.h"

MIG_C_LINKAGE_START

void
mig_im_bb_cut_8u ( Mig8u *src , 
                   Mig8u *dst , 
                   mig_size_t *s ,
                   mig_roi_t *bb );

void
mig_im_bb_cut_16u ( Mig16u *src , 
                    Mig16u *dst , 
                    mig_size_t *s ,
                    mig_roi_t *bb );


void
mig_im_bb_cut_2d ( Mig16u *src ,
                   int w , int h , 
                   int cx , int cy ,
                   float *dst ,
                   int r );

void
mig_im_bb_cut_2d_32f ( float *src , int w , int h , int cx , int cy , float *dst , int r );

void
mig_im_bb_cut_3d ( Mig16u *src ,
                   int w , int h , int z ,
                   int cx , int cy , int cz ,
                   float *dst ,
                   int r );

MIG_C_LINKAGE_END

#endif /* __MIG_IM_BB_H__ */


