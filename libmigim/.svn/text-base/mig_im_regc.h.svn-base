/*
******************************************************************************
*
* Author      : Todor Petkov
* Filename    : mig_im_regc.h
* Created     : 2007/06/21
* Description : 2D and 3D Region labeling and centroid
*
******************************************************************************
*/

#ifndef __MIG_REGC_H__
#define __MIG_REGC_H__

#include "mig_config.h"
#include "mig_defs.h"
#include "mig_data_types.h"
#include "mig_error_codes.h"

#include "libmigim.h"
#include "libmigst.h"

MIG_C_LINKAGE_START

/*
******************************************************************************
*                               PROTOTYPES
******************************************************************************
*/

/*
******************************************************************************
*               CALCULATE CENTROIDS OF 3D CONNECTED COMPONENTS
*
* Description : This function calculates the centroids of all 3D connected components
*
* Arguments   : Binary  - Imput volume ( should contain only the values 0.0 and 1.0 )
*               Width   - width of input volume
*               Height  - height of input volume
*               Depth   - z of input volume
*               Regions - Preallocated but empty list. Resulting centroids are
                          going to be store here.
*
* Returns     : 0 on success
*               -1 or error
*
* Notes       : Binary array is zeroed in the process of labeling !
*
******************************************************************************
*/

int
mig_im_regc_3d ( float *Binary ,
                 int Width , int Height , int Depth ,
                 mig_lst_t *Regions );


/*
******************************************************************************
*               CALCULATE CENTROIDS OF 2D CONNECTED COMPONENTS
*
* Description : This function calculates the centroids of all 3D connected components
*
* Arguments   : Binary  - Imput image ( should contain only the values 0.0 and 1.0 )
*               Width   - width of input image
*               Height  - height of input image
*               Regions - Preallocated but empty list. Resulting centroids are
                          going to be store here.
*
* Returns     : 0 on success
*               -1 or error
*
* Notes       : Binary array is zeroed in the process of labeling !
*
******************************************************************************
*/

int
mig_im_regc_2d ( float *Binary ,
                 int Width , int Height ,
                 mig_lst_t *Regions );



/*
******************************************************************************
*               SIMPLE VOLUME TO REGS CONVERSION
*
* Description : This function creates a new region for each ON pixel encountered
*
* Arguments   : Binary  - Imput image ( should contain only the values 0.0 and 1.0 )
*               Width   - width of input image
*               Height  - height of input image
*               Regions - Preallocated but empty list. Resulting centroids are
                          going to be store here.
*
* Returns     : 0 on success
*               -1 or error
*
* Notes       : Binary array is zeroed in the process of labeling !
*
******************************************************************************
*/
int
mig_im_regc_3d_odd ( float *Binary ,
                 int Width , int Height , int Depth ,
                 mig_lst_t *Regions );

MIG_C_LINKAGE_END

#endif /* __MIG_REGC_H__ */

