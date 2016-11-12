#ifndef __MIG_IM_REG_H__
#define __MIG_IM_REG_H__

#include "mig_config.h"
#include "mig_defs.h"

#include "mig_data_types.h"
#include "mig_data_image.h"

#include "libmigut.h"
#include "libmigst.h"

typedef struct
{
        /* region label */
        int id;

        /* binary */
        double  area;

        /* coordinates */
        double  centroid[3];        /* x , y , z */
        int     min_coord[3];       /* x , y , z */
        int     max_coord[3];       /* x , y , z */

} mig_reg_props_t;

MIG_C_LINKAGE_START

extern int
mig_reg_prop_sep ( Mig8u *labs , 
                   int w , int h , 
                   mig_lst_t **props , 
                   int num_cc );

extern int
mig_reg_prop_vol ( Mig8u *labs , 
                   int w , int h , int z , 
                   mig_lst_t **props , 
                   int num_cc );

MIG_C_LINKAGE_END

#endif /* __MIG_IM_REG_H__ */

