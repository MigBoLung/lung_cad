/*
******************************************************************************
*
* Author      : Todor Petkov
* Filename    : mig_im_drv.h
* Created     : 2007/06/21
* Description : 3d first order derivative operators
*
******************************************************************************
*/

#ifndef __MIG_IM_DRV_H__
#define __MIG_IM_DRV_H__

#include "mig_config.h"
#include "mig_defs.h"
#include "mig_data_types.h"
#include "mig_error_codes.h"

MIG_C_LINKAGE_START

/*
******************************************************************************
*                               PROTOTYPES
******************************************************************************
*/


/*
******************************************************************************
*               3D FIRST ORDER DERIVATIVE USING SOBEL'S 3D MASK
*
* Description : This function performs a 1st order derivation on a 3d signal
*               by method of central differences
*
*
* Arguments   : data - input 3D signal
*               w    - input signal width
*               h    - input signal height
*               z    - input signal z
*               dx   - preallocated. Output dx
*               dy   - preallocated. Output dy
*               dz   - preallocated. Output dz
*               dmag - preallocated. Output gradient magnitude
*               thr  - threshold for gradient magnitude.
*
* Returns     :
*
* Notes       : Gradient magnitude is calculated as abs(dx) + abs(dy)
*
******************************************************************************
*/

extern void
mig_im_sobel_3d ( float *data, int w, int h, int z, float *dx, float *dy, float *dz,
				 float *dmag, float thr );







/*
******************************************************************************
*               3D FIRST ORDER DERIVATIVE USING CENTRAL DIFFERENCES
*
* Description : This function performs a 1st order derivation on a 3d signal
*               by method of central differences
*
*
* Arguments   : data - input 3D signal
*               w    - input signal width
*               h    - input signal height
*               z    - input signal z
*               dx   - preallocated. Output dx
*               dy   - preallocated. Output dy
*               dz   - preallocated. Output dz
*               dmag - preallocated. Output gradient magnitude
*
* Returns     :
*
* Notes       : Gradient magnitude is calculated as abs(dx) + abs(dy) + abs(dz) !
*
******************************************************************************
*/

void
mig_im_drv_3d_central_diffs ( unsigned short *data ,
                              int w , int h , int z ,
                              float *dx , float *dy , float *dz ,
                              float *dmag );

/*
******************************************************************************
*               2D FIRST ORDER DERIVATIVE USING CENTRAL DIFFERENCES
*
* Description : This function performs a 1st order derivation on a 2d signal
*               by method of central differences
*
*
* Arguments   : data - input 2D signal
*               w    - input signal width
*               h    - input signal height
*               dx   - preallocated. Output dx
*               dy   - preallocated. Output dy
*               dmag - preallocated. Output gradient magnitude
*
* Returns     :
*
* Notes       : Gradient magnitude is calculated as abs(dx) + abs(dy) !
*
******************************************************************************
*/

void
mig_im_drv_2d_central_diffs ( float *data , int w , int h , float *dx , float *dy , float *dmag );

MIG_C_LINKAGE_END

#endif /* __MIG_IM_DRV_H__ */
