#ifndef __MIG_SEG_CLOSE_H__
#define __MIG_SEG_CLOSE_H__

#include "mig_config.h"
#include "mig_defs.h"
#include "mig_data_types.h"
#include "mig_data_image.h"
#include "mig_error_codes.h"

MIG_C_LINKAGE_START

extern int
mig_seg_close ( Mig8u *src , mig_size_t *s , int r );

MIG_C_LINKAGE_END

#endif /* __MIG_SEG_CLOSE_H__ */

