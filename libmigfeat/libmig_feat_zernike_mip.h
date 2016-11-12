#ifndef __FEATURE_EXTRACTION_H__
#define __FEATURE_EXTRACTION_H__

#include "mig_config.h"
#include "mig_defs.h"

#include "mig_data_types.h"
#include "mig_data_image.h"
#include "mig_data_dicom.h"
#include "mig_data_cad.h"

#include "mig_error_codes.h"

#include "libmigim.h"




/*****************************************************************
 *	Public elements of fpr2, that will be used by the training
 *  part to extract the features in the same way of the testing
 *  part
 *****************************************************************
 */


MIG_C_LINKAGE_START

/******************************************************************
 *  FEATURE EXTRACTION DATA
 ****************************************************************
 */


typedef struct _fpr2_params_feat_t
{
	/* crop side length after resizing */
	int resized_len;

	/* how much of voi is used to get mip image (float [0,1]) */
	float mip_ratio;

	/* sizes vector for moment mask calculation */
	int *crop_sizes;

	/* length of sizes */
	int crop_sizes_len;

	/* orders indexes vector */
	int *mom_orders;

	/* length of orders */
	int mom_orders_len;

	/* masks */
	mig_im_mom_t *mom_masks;

} fpr2_params_feat_t;


/*
 ***********************************************************
 *  FEATURE STRUCTURE
 **********************************************************
 */
typedef struct _feat_t
{
	int label;
	int feat_len;
	int ndir;
	float **feats;
} feat_t;


feat_t*
feat_t_alloc ( int ndir, int nfeats );

void
feat_t_free ( feat_t* featstruct );

int
extract_features ( mig_im_region_t *obj3d, Mig16u* src, int w, int h, int z,
						fpr2_params_feat_t *featparams, feat_t *featstruct );

 MIG_C_LINKAGE_END
#endif /*__FEATURE_EXTRACTION_H__*/

