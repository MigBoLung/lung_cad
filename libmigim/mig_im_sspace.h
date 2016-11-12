/*
******************************************************************************
*
* Author      : Todor Petkov
* Filename    : mig_im_sspace.h
* Created     : 2007/06/21
* Description : 2D - 3D Scale Space
*
******************************************************************************
*/

#ifndef __MIG_IM_SSPACE_H__
#define __MIG_IM_SSPACE_H__

#include "mig_config.h"
#include "mig_defs.h"
#include "mig_data_types.h"
#include "mig_error_codes.h"

#include "libmigim.h"

MIG_C_LINKAGE_START

/*
******************************************************************************
*                               DATA TYPES
******************************************************************************
*/

/*
******************************************************************************
*                               TYPE OF SCALE SPACE SIGMAS SPACING
******************************************************************************
*/

typedef enum
{
        GEOMETRIC  = 0 ,        /* space adjacent sigmas geometricaly */
        ARITHMETIC = 1          /* space adjacent sigmas arithmeticaly */

} SigmaSpacing;

/*
******************************************************************************
*                               SCALE SPACE REPRESENTATION
******************************************************************************
*/

typedef struct _mig_sspace_t
{
        int type;                       /* scale spacew type -> 0 is 3D, 1 is 2D */
        int window_radius;              /* scale space buffer radius given by  floor( 2 * sigma_end + 0.5 ) */ /*AAAA MODIFIED TO 3*floor( 2 * sigma_end + 0.5 )*/
        int window_len;                 /* scale space buffer diameter */
        int window_voxels;              /* scale space buffer total length */

        SigmaSpacing spacing;           /* scale space method for calculating the list of sigmas */
                                        /* If GEOMETRIC sigmas are calculated as geometric progression of
                                           common ration SS_K ( see macros.h ), starting from
                                           sigma_start. If ARITHMETIC sigmas are calculated as arithmetic
                                           progression of common difference sigma_inc, starting
                                           from sigma_start */

        float sigma_start;              /* scale space first sigma */
        float sigma_end;                /* scale space last sigma  */
        float sigma_inc;                /* scale space spacing between adjacent sigmas */

        int   num_sigmas;               /* scale space total number of sigmas */

        float threshold;                /* scale space responses threshold */

        mig_kernel_t **kernels;         /* scale space LoG kernels */
        float        **data;            /* buffers for scale space representation of input signal */
        float        **extrema;         /* buffers for scale space extrema */



} mig_sspace_t;

/*
******************************************************************************
*                               PROTOTYPES
******************************************************************************
*/

/*
******************************************************************************
*                       PREPARE SCALE SPACE STRUCTURE
*
* Description : This function sets up scale space structure before actual
*               scale space processing takes place.  This involves the following
*               steps :
*                       1. Build scale space LoG kernels
*                       2. Allocate scale space data buffer
*                       3. Allocate scale space etrema buffer
*
* Arguments   : 
*               SSpaceType    - 0 for 3D scale space , 1 for 2D scale space
*               spacing       - sigmas spacing ( GEOMETRIC or ARITHMETIC )
*               sigma_start   - starting sigma ( should be > 0 )
*               sigma_end     - end sigma ( should be > sigma_start )
*               sigma_inc     - distance between adjacent sigmas
*               threshold     - scale space responses threshold
*
* Returns     : filled in mig_sspace_t structure on success
*               NULL on error
*
* Notes       : scale space structure must be freed using sspace_del
*
******************************************************************************
*/

mig_sspace_t*
mig_im_sspace_get ( int SSpaceType , 
                    SigmaSpacing spacing ,
                    float sigma_start , float sigma_end , float sigma_inc ,
                    float threshold );

/*
******************************************************************************
*                       PERFORM SCALE SPACE PROCESSING
*
* Description : This function performs tha actual scale space processing.
*               This involves the following
*               steps :
*                       1. Filter input signal with all available LoG filters
*                       2. Scan LoG filtered buffers looking for local extrema
*                          in a 3x3x3 neighbourhood
*                       3. Threshold local extrema responses
*                       4. Find maximum local extrema response and associated
*                          sigma as well as 3D voxel coordinates
*
* Arguments   : Input        - input signal on which to perform scale space filtering
*               ScaleSpace   - scale space structure
*
* Returns     : filled in region_t structure on success
*               NULL if no apropriate scale space response was found or an error
*               occurd
*
* Notes       : Returned region coordinates are expressed with respect to
*               the centroid of the input signal
*
******************************************************************************
*/

mig_im_region_t*
mig_im_sspace ( float *Input , mig_sspace_t *ScaleSpace );

/*
******************************************************************************
*                       DELETE SCALE SPACE STRUCTURE
*
* Description : This function frees memory taken up by scale space structure

* Arguments   : ScaleSpace - structure to be freed
*
* Returns     :
*
* Notes       : ScaleSpace pointer is also freed but not initalized to NULL!
*
******************************************************************************
*/

void
mig_im_sspace_del ( mig_sspace_t *ScaleSpace );



/*
******************************************************************************
*                       FAST BEST RADIUS COMPUTATION
*
* Description : This function obtains a radius estimation by computing sspace values
*				in the center of the given region. No centroid estimation is done
*               This involves the following
*               steps :
*                       1. Apply LoG filters centered on the center of the region
*                       2. Get the radius of maximum response
*
* Arguments   : Input        - input signal on which to perform scale space filtering
*               ScaleSpace   - scale space structure
*
* Returns     : filled in region_t structure on success
*               NULL if no apropriate scale space response was found or an error
*               occurd
*
* Notes       : Returned region coordinates are expressed with respect to
*               the centroid of the input signal
*
******************************************************************************
*/

mig_im_region_t*
mig_im_sspace_radius ( float *Input , mig_sspace_t *ScaleSpace );

MIG_C_LINKAGE_END

#endif /* __MIG_IM_SSPACE_H__ */
