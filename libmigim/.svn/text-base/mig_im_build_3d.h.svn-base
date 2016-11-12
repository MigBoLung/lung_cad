/*
******************************************************************************
*
* Author      : Todor Petkov
* Filename    : mig_im_build_3d.h
* Created     : 2007/06/21
* Description : Building and Pruning of 3D regions out of 2D regions
*
******************************************************************************
*/

#ifndef __MIG_IM_BUILD_3D_H__
#define __MIG_IM_BUILD_3D_H__

#include "mig_config.h"
#include "mig_defs.h"
#include "mig_data_types.h"
#include "mig_error_codes.h"

#include "libmigim.h"
#include "libmigst.h"

MIG_C_LINKAGE_START

/*
******************************************************************************
*                               DATA TYPES
******************************************************************************
*/

/*
******************************************************************************
*                               BUILD OBJ3D PARAMETERS
******************************************************************************
*/

int
mig_im_build_obj3d ( mig_lst_t *Src , 
                     mig_lst_t *Results ,
                     mig_lst_cmp_f build_f , 
                     mig_lst_sel_f cut_f ,
                     mig_lst_free_f free_f );

void
obj3d_compact ( mig_im_region_t *Region );


MIG_C_LINKAGE_END

#endif /* __MIG_IM_BUILD_3D_H__ */
