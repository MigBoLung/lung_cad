/*
******************************************************************************
*
* Author      : Todor Petkov
* Filename    : mig_kernel.h
* Created     : 2007/06/21
* Description : Convolution kernel definition and building
*
******************************************************************************
*/

#ifndef __MIG_IM_KERNEL_H__
#define __MIG_IM_KERNEL_H__

#include "mig_config.h"
#include "mig_defs.h"
#include "mig_data_types.h"
#include "mig_error_codes.h"

MIG_C_LINKAGE_START

/*
******************************************************************************
*                               DATA TYPES
******************************************************************************
*/

/*
******************************************************************************
*                               CONVOLUTION BORDER TREATMENT
******************************************************************************
*/

typedef enum
{
        BORDER_REPEAT  = 1 ,    /* repeat closest valid pixel */
        BORDER_REFLECT = 2 ,    /* mirro closest valid pixels */
        BORDER_CLIP    = 3      /* zero fill */

} BorderTreatment;

/*
******************************************************************************
*                               CONVOLUTION KERNEL
******************************************************************************
*/

typedef struct
{
        int r;                  /* kernel radius */
        int d;                  /* kernel diameter */
        int n;                  /* total nuber of pixels in kernel */

        float *data;            /* kernel values */
        float *center;          /* kernel center */

        BorderTreatment border; /* border treatment for given kernel */

        float scale;            /* sigma * SQRT(2) */
        float sigma;            /* sigma for gaussian like kernels */

} mig_kernel_t;

/*
******************************************************************************
*                               PROTOTYPES
******************************************************************************
*/

/*
******************************************************************************
*               BUILD 1D LAPLACIAN OF GAUSSIAN KERNEL
*
* Description : This function builds a 1D Laplacian of Gaussian fir kernel
*               by direct sampling of the second order derivative of a gaussian
*               function
*
* Arguments   : sigma - gaussian sigma
*
* Returns     : filled mig_kernel_t structure on success
*               NULL on error
*
* Notes       : kernel radius is 3 * sigma
*               kernel diameter is 2 * r + 1
*               kernel border treamtment is BORDER_REFLECT
*
******************************************************************************
*/

mig_kernel_t*
mig_im_kernel_get_log_1d ( float sigma );

/*
******************************************************************************
*               BUILD 2D LAPLACIAN OF GAUSSIAN KERNEL
*
* Description : This function builds a 2D Laplacian of Gaussian fir kernel
*               by direct sampling of the second order derivative of a gaussian
*               function
*
* Arguments   : sigma - gaussian sigma
*
* Returns     : filled mig_kernel_t structure on success
*               NULL on error
*
* Notes       : kernel radius is 3 * sigma
*               kernel diameter is 2 * r + 1
*               kernel border treamtment is BORDER_REFLECT
*
******************************************************************************
*/

mig_kernel_t*
mig_im_kernel_get_log_2d ( float sigma );


/*
******************************************************************************
*               BUILD 3D LAPLACIAN OF GAUSSIAN KERNEL
*
* Description : This function builds a 3D Laplacian of Gaussian fir kernel
*               by direct sampling of the second order derivative of a gaussian
*               function
*
* Arguments   : sigma - gaussian sigma
*
* Returns     : filled mig_kernel_t structure on success
*               NULL on error
*
* Notes       : kernel radius is 3 * sigma
*               kernel diameter is 2 * r + 1
*               kernel border treamtment is BORDER_REFLECT
*
******************************************************************************
*/
mig_kernel_t*
mig_im_kernel_get_log_3d ( float sigma );

/*
******************************************************************************
*               BUILD 1D MEAN KERNEL
*
* Description : This function builds a 1D Mean normalized fir kernel
*               
*
* Arguments   : radius pixels of radius
*
* Returns     : filled mig_kernel_t structure on success
*               NULL on error
*
* Notes       : kernel border treamtment is BORDER_REFLECT
*
******************************************************************************
*/

mig_kernel_t*
mig_im_kernel_get_mean_1d ( int radius );

/*
******************************************************************************
*               FREE KERNEL DATA
*
* Description : This function frees kernel data
*
* Arguments   : kernel
*
* Notes       : Only data internal to mig_kernel_t is freed.
*
******************************************************************************
*/

void
mig_im_kernel_delete ( mig_kernel_t *kernel );

MIG_C_LINKAGE_END

#endif /* __MIG_IM_KERNEL_H__ */
