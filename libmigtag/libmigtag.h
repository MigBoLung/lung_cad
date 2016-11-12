#ifndef __LIBMIGTAG_H__
#define __LIBMIGTAG_H__

#include "mig_config.h"
#include "mig_defs.h"
#include "mig_data_types.h"
#include "mig_data_image.h"
#include "mig_data_dicom.h"
#include "mig_data_cad.h"
#include "mig_error_codes.h"

#include "libmigut.h"
#include "libmigst.h"
#include "libmigim.h"

MIG_C_LINKAGE_START

int
mig_tag_write ( char *name ,
                mig_lst_t *results );

int
mig_tag_read ( char *name ,
               mig_lst_t *results );

void
mig_tag_free ( mig_lst_t *results );

/*
 *************************************************************************
 * change z positions of elements in list from list_z_res to target_z_res
 *************************************************************************
 */
int
mig_tag_resize ( mig_lst_t *results, float list_z_res, float target_z_res);

MIG_C_LINKAGE_END

#endif /* __LIBMIGTAG_H__ */


