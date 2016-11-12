#ifndef __MIG_IO_MAT_H__
#define __MIG_IO_MAT_H__

#include "mig_config.h"
#include "mig_defs.h"
#include "mig_data_types.h"
#include "mig_data_image.h"
#include "mig_data_dicom.h"

/* We have MATLAB installed on this machine */
#if defined(MATLAB)

#include "mat.h"
#include "mex.h"


MIG_C_LINKAGE_START

int
mig_io_mat_w_ct ( char *mat_name ,
                  Mig16u *src ,
                  mig_dcm_data_t *info ,
                  mig_size_t *s ,
                  mig_size_t *orig ,
                  mig_roi_t *bb );

int
mig_io_mat_w_masks ( char *mat_name ,
                     Mig8u *src ,
                     mig_dcm_data_t *info ,
                     mig_size_t *s ,
                     mig_size_t *orig ,
                     mig_roi_t *bb );

int
mig_io_mat_w_float ( char *mat_name ,
                     float *src ,
                     mig_dcm_data_t *info ,
                     mig_size_t *s ,
                     mig_size_t *orig ,
                     mig_roi_t *bb );

MIG_C_LINKAGE_END

/* We do not have MATLAB installed on this machine */
#else

#define mig_io_mat_w_ct(a,b,c,d,e,f)        MIG_ERROR_UNSUPPORTED;
#define mig_io_mat_w_masks(a,b,c,d,e,f)     MIG_ERROR_UNSUPPORTED;
#define mig_io_mat_w_float(a,b,c,d,e,f)     MIG_ERROR_UNSUPPORTED;

#endif  /* MATLAB */

#endif /* __MIG_IO_MAT_H__ */
