/*
******************************************************************************
*
* Author      : Todor Petkov
* Filename    : mig_conv.h
* Created     : 2007/06/21
* Description : 1d convolution
*
******************************************************************************
*/

#ifndef __MIG_IM_CONV_H__
#define __MIG_IM_CONV_H__

#include "mig_config.h"
#include "mig_defs.h"
#include "mig_data_types.h"
#include "mig_error_codes.h"
#include "mig_im_kernel.h"

MIG_C_LINKAGE_START

/*
******************************************************************************
*                               PROTOTYPES
******************************************************************************
*/

/*
******************************************************************************
*               1D CONVOLUTION IN HORIZONTAL DIRECTION
*
* Description : This function performs a 1d convolution in the horizontal
*               direction on a 3d input signal
*
* Arguments   : src    - input 3D signal
*               dst    - preallocated. Result of horizontal convolution
*               w      - input signal width
*               kernel - preallocated. 1D kernel
*
* Returns     : 0 on success
*               -1 on error
*
* Notes       : convolution is not performed and error is returned if
*               kernel->r > w !
*
******************************************************************************
*/

int
mig_im_conv_1d_x ( float *src , float *dst , int w , mig_kernel_t *kernel );

/*
******************************************************************************
*               1D CONVOLUTION IN VERTICAL DIRECTION
*
* Description : This function performs a 1d convolution in the vertical
*               direction on a 3d input signal
*
* Arguments   : src    - input 3D signal
*               dst    - preallocated. Result of vertical convolution
*               w      - input signal width
*               h      - input signal height
*               kernel - preallocated. 1D kernel
*
* Returns     : 0 on success
*               -1 on error
*
* Notes       : convolution is not performed and error is returned if
*               kernel->r > h !
*
******************************************************************************
*/

int
mig_im_conv_1d_y ( float *src , float *dst , int w , int h , mig_kernel_t *kernel );

/*
******************************************************************************
*               1D CONVOLUTION IN Z DIRECTION
*
* Description : This function performs a 1d convolution in the z
*               direction on a 3d input signal
*
* Arguments   : src    - input 3D signal
*               dst    - preallocated. Result of z convolution
*               w      - input signal width
*               h      - input signal height
*               z      - input signal height
*               kernel - preallocated. 1D kernel
*
* Returns     : 0 on success
*               -1 on error
*
* Notes       : convolution is not performed and error is returned if
*               kernel->r > z !
*
******************************************************************************
*/

int
mig_im_conv_1d_z ( float *src , float *dst , int w , int h , int z , mig_kernel_t *kernel );

MIG_C_LINKAGE_END

#endif /* __MIG_IM_CONV_H__ */
