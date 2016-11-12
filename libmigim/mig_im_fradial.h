/*
******************************************************************************
*
* Author      : Todor Petkov
* Filename    : mig_im_fradial.h
* Created     : 2007/06/21
* Description : 2D - 3D Fast Radial Filter (Loy & Zelinski)
*
*
* NOTE 20100712:  3d FR has some of the new improvements made by todor:
*		- works on [0,1] float
*		- has beta thresholding
*		- uses sobel kernels
*		it still uses iir gaussian
*
******************************************************************************
*/

#ifndef __MIG_IM_FRADIAL_H__
#define __MIG_IM_FRADIAL_H__

#include "mig_config.h"
#include "mig_defs.h"
#include "mig_data_types.h"
#include "mig_error_codes.h"

#include "libmigst.h"

MIG_C_LINKAGE_START

/*
******************************************************************************
*                               DATA TYPES
******************************************************************************
*/

typedef enum
{
    FLAT = 0 ,     /* fast radial threshold is flat value */    
    PERCENT = 1    /* fast radial threshold is calculated as percentage of max fast radial response */

} ThresholdType;

/*
******************************************************************************
*                               FAST RADIAL REPRESENTATION
******************************************************************************
*/

typedef struct _mig_fradial_t
{
        int             dump;               /* should we dump intermediate results to disk */
        char            prefix[MAX_PATH];   /* if dump is 1 this serves as file prefix */
        float           *radii;             /* list of radii */
        int             num_radii;          /* total number of radii */
        float           threshold;          /* threshold for fast radial responses */
        ThresholdType   thr_type;           /* threshold type */        
		float			beta_threshold;		/* threshold on gradient magnitude */

} mig_fradial_t;


/*
******************************************************************************
*                               PROTOTYPES
******************************************************************************
*/

/*
******************************************************************************
*                       PREPARE FAST RADIAL STRUCTURE
*
* Description : This function sets up fast radial structure before actual
*               fast radial processing takes place.
*
* Arguments   : radii       - linear array of radii
*               num_radii   - lenght of radii array
*               threshold   - fast radial responses threshold.
*               type        - how to calculate fast radial response thresholding
*				beta_thr	- gradient magnitude threshold
*
*
* Returns     : filled in mig_fradial_t structure on success
*               NULL on error
*
* Notes       : fast radial structure must be freed using fradial_del
*
******************************************************************************
*/

mig_fradial_t*
mig_im_fradial_get ( float *radii , int num_radii , float threshold , ThresholdType type, float beta_thr );

/*
******************************************************************************
*                       PERFORM FAST RADIAL FILTERING ON 3D STACK
*
* Description : This function performs tha actual fast radial processing.
*               This involves the following
*               steps :
*                       1. Filter input signal once using fast radial filter
*                       2. Binarize responses using relative threshold
*                       3. Build a list of 3D connected components' centroids
*
* Arguments   : Input      - input signal on which to perform fast radial filtering
*               w          - input signal width
*               h          - input signal height
*               z          - input signal z
*               FastRadial - prepared fast radial structure
*               Regions    - results of fast radial filtering - list of coordinates
*
* Returns     : 0 on success
*               -1 on error
*
* Notes       : Only one fast radial pass is performed !
*
******************************************************************************
*/

int
mig_im_fradial_3d ( unsigned short *Input ,
                    int w , int h , int z ,
                    mig_fradial_t *FastRadial ,
                    mig_lst_t *Regions );

/*
******************************************************************************
*                       PERFORM FAST RADIAL FILTERING ON 2D IMAGE
*
* Description : This function performs tha actual fast radial processing.
*               This involves the following
*               steps :
*                       1. Filter input signal once using fast radial filter
*                       2. Filter input signal once using fast radial filter
*                       3. Binarize responses using relative threshold
*                       4. Build a list of 2D connected components' centroids
*
* Arguments   : Input      - input signal on which to perform fast radial filtering
*               w          - input signal width
*               h          - input signal height
*               FastRadial - prepared fast radial structure
*               Regions    - results of fast radial filtering - list of coordinates
*
* Returns     : 0 on success
*               -1 on error
*
* Notes       : Two fast radial passes are performed ! 
*               Input buffer is overwritten !
*
******************************************************************************
*/

int
mig_im_fradial_2d ( float *Input ,
                    int w , int h ,
                    mig_fradial_t *FastRadial ,
                    mig_lst_t *Regions );

/*
******************************************************************************
*                       DELETE FAST RADIAL STRUCTURE
*
* Description : This function frees memory taken up by fast radial structure

* Arguments   : FastRadial - structure to be freed
*
* Returns     :
*
* Notes       : FastRadial pointer is also freed but not initalized to NULL!
*
******************************************************************************
*/

void
mig_im_fradial_del ( mig_fradial_t *FastRadial );

MIG_C_LINKAGE_END

#endif /* __MIG_IM_FRADIAL_H__ */
