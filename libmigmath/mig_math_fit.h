#ifndef __MIG_MATH_FIT_H__
#define __MIG_MATH_FIT_H__

#include "mig_config.h"
#include "mig_defs.h"
#include "mig_data_types.h"

MIG_C_LINKAGE_START

void
mig_math_polyfit_linear ( float *x , float *y , int len , float *a , float *b );

void
mig_math_polyval_linear ( float *x , float *y , int len , float a , float b );

MIG_C_LINKAGE_END

#endif /* __MIG_IM_ALG_H__ */
