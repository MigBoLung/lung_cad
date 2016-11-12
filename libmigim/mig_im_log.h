/*
******************************************************************************
*
* Author      : Todor Petkov
* Filename    : mig_im_log.h
* Created     : 2007/06/21
* Description : 3D Laplacian of Gaussian
*
******************************************************************************
*/

#ifndef __MIG_IM_LOG_H__
#define __MIG_IM_LOG_H__

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
*               3D LAPLACIAN OF GAUSSIAN SEPARABLE CONVOLUTION
*
* Description : This function performs a 3D laplacian of gaussian convolution
*
* Arguments   : src - input signal
*               dst - output signal
*               w   - input signal width
*               h   - input signal height
*               z   - input signal z
*               log - 1d LoG kernel as returned by get_log_1d
*
* Returns     : 0 on success
*               -1 on error
*
* Notes       : convolution is not performed and error is returned if
*               kernel->r > w or kernel->r > h or kernel->r > z !
*
******************************************************************************
*/

int
mig_im_log_3d_sep ( float *src , float *dst ,
                    int w , int h , int z ,
                    mig_kernel_t *log );

/*
******************************************************************************
*               2D LAPLACIAN OF GAUSSIAN SEPARABLE CONVOLUTION
*
* Description : This function performs a 2D laplacian of gaussian convolution
*
* Arguments   : src - input signal
*               dst - output signal
*               w   - input signal width
*               h   - input signal height
*               log - 1d LoG kernel as returned by get_log_1d
*
* Returns     : 0 on success
*               -1 on error
*
* Notes       : convolution is not performed and error is returned if
*               kernel->r > w or kernel->r > h !
*
******************************************************************************
*/

int
mig_im_log_2d_sep ( float *src , float *dst ,
                    int w , int h ,
                    mig_kernel_t *log );


/*
******************************************************************************
*               2D LAPLACIAN OF GAUSSIAN FULL CONVOLUTION
*
* Description : This function performs a 2D laplacian of gaussian full convolution
*
* Arguments   : src - input signal
*               dst - output signal
*               w   - input signal width
*               h   - input signal height
*               log - 1d LoG kernel as returned by get_log_2d
*
* Returns     : 0 on success
*               -1 on error
*
* Notes       : convolution is not performed and error is returned if
*               kernel->r > w or kernel->r > h !
*
******************************************************************************
*/

int
mig_im_log_2d_full ( float *src , float *dst ,
                     int w , int h ,
                     mig_kernel_t *log );





/*
******************************************************************************
*               3D LAPLACIAN OF GAUSSIAN EVALUATED ON CENTER
*
* Description : This function performs a 3D laplacian of gaussian convolution
*               only on the center voxel
*
* Arguments   : src - input signal
*               dst - output signal (only one value returned )
*               w   - input signal width
*               h   - input signal height
*               z   - input signal z
*               log - 3d LoG kernel as returned by get_log_1d
*
* Returns     : 0 on success
*               -1 on error
*
* Notes       : convolution is not performed and error is returned if
*               kernel->r > w or kernel->r > h or kernel->r > z !
*
******************************************************************************
*/


int
mig_im_log_3d_center ( float *src , float *dst ,
                    int w , int h , int z ,
                     mig_kernel_t *log );


MIG_C_LINKAGE_END

#endif /* __MIG_IM_LOG_H__ */
