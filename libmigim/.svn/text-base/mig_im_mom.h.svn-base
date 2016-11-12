/*
******************************************************************************
*
* Author      : Gianluca Ferri
* Filename    : mig_im_mom.h
* Created     : 2010/06/08
* Description : Image moments extraction, for grayscale image.
*
* NOTE:
* In Lung CAD as of june 2010, pseudo zernike moments are used, without
* normalization, because scale and translation are given by the way
* classification is done
*
******************************************************************************
*/

#ifndef __MIG_IM_MOM_H__
#define __MIG_IM_MOM_H__

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

/*
******************************************************************************
*                      POLYNOMIALS REAL AND IMAGINARY PARAMS
******************************************************************************
*/



/*
******************************************************************************
*                               POLYNOMIALS MASK REPRESENTATION
******************************************************************************
*/

typedef struct _mig_im_mom_t
 {
		int				size;					/* size of this mask in px */
		int				num_orders;				/* list of orders to use */
		int				*orders;				/* ids of orders (insted m n notation we assign increasing ids) */
		int				elemsperorder;			/* at the same radius we have all the same lengths */
		float			**values_re;		    /* for each order we have an array of discretized poly real values */
		float			**values_im;			/* for each order we have an array of discretized poly imaginary values */
} mig_im_mom_t;



/*
******************************************************************************
*                               PROTOTYPES
******************************************************************************
*/

/*
******************************************************************************
*                       PREPARE MOMENTS STRUCTURE
*
* Description : This function sets up moments structure before actual
*               moment calculation processing takes place.
				Here, masks are filled
*
* Arguments   : radii       - linear array of radii
*               num_radii   - lenght of radii array
*				orders		- ids of orders (insted m n notation we assign increasing ids)
*				num_orders	- length of moments array 
*
* Returns     : filled in mig_im_mom_t structure on success
*               NULL on error
*
* Notes       : moment structure must be freed using mom_del
*
******************************************************************************
*/

mig_im_mom_t*
mig_im_mom_get ( int size, int *orders , int num_orders );

/*
******************************************************************************
*                       GET MOMENTS OF A 2D IMAGE
*
* Description : This function extracts moments from a passed crop.
*
* Arguments   : crop       - input crop
*               w          - input signal width
*               h          - input signal height
*               Masks	   - prepared moment masks structure
*               Moments    - extracted moments at various scales
*
* Returns     : 0 on success
*               -1 on error
*
* Notes       : FOR LUNG CAD, as june 2010: MIP calculation is done for each crop, because
*				we want 70% of the 3D crop mipped. TODO: evaluate different strategies
*
******************************************************************************
*/

int
mig_im_mom_crop_2D ( float *crop,
					int size ,
                    mig_im_mom_t *Masks , 
                    float *moments );

/*
******************************************************************************
*                       DELETE MOMENT MASKS STRUCTURE
*
* Description : This function frees memory taken up by moments mask structure

* Arguments   : Masks - structure to be freed
*
* Returns     :
*
* Notes       : Masks pointer is also freed but not initalized to NULL!
*
******************************************************************************
*/

void
mig_im_mom_del ( mig_im_mom_t *Masks );


MIG_C_LINKAGE_END

#endif /* __MIG_IM_MOM_H__ */
