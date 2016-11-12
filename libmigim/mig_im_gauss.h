/*
******************************************************************************
*
* Author      : Todor Petkov
* Filename    : mig_im_gauss.h
* Created     : 2007/06/21
* Description : 2D - 3D Gaussian IIR filter (Van Vliet)
*
******************************************************************************
*/

#ifndef __MIG_IM_GAUSS_H__
#define __MIG_IM_GAUSS_H__

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
*               3D GAUSSIAN IIR FILTERING
*
* Description : This function performs a separable 3D gaussian iir filtering
*               as described in :
*
*                       Recursive Gabor filtering
*                       Young, I.T.; van Vliet, L.J.; van Ginkel, M.
*                       Signal Processing, IEEE Transactions on
*                       Volume 50, Issue 11, Nov 2002 Page(s): 2798 - 2805
*
* Arguments   : in    - input signal
*               out   - output filtered signal
*               w     - input signal width
*               h     - input signal height
*               z     - input signal z
*               sigma - gaussian sigma
*
* Returns     : 
*
******************************************************************************
*/

void
mig_im_gauss_iir_3d ( float *in , float *out , int w , int h , int z , float sigma );

/*
******************************************************************************
*               2D GAUSSIAN IIR FILTERING
*
* Description : This function performs a separable 2D gaussian iir filtering
*               as described in :
*
*                       Recursive Gabor filtering
*                       Young, I.T.; van Vliet, L.J.; van Ginkel, M.
*                       Signal Processing, IEEE Transactions on
*                       Volume 50, Issue 11, Nov 2002 Page(s): 2798 - 2805
*
* Arguments   : in    - input signal
*               out   - output filtered signal
*               w     - input signal width
*               h     - input signal height
*               sigma - gaussian sigma
*
* Returns     :
*
******************************************************************************
*/

void
mig_im_gauss_iir_2d ( float *in , float *out , int w , int h , float sigma );

MIG_C_LINKAGE_END

#endif /* __MIG_IM_GAUSS_H__ */
