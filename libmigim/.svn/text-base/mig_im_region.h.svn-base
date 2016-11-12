/*
******************************************************************************
*
* Author      : Todor Petkov
* Filename    : mig_region.h
* Created     : 2007/06/21
* Description : 2D - 3D Region definition
*
******************************************************************************
*/

#ifndef __MIG_IM_REGION_H__
#define __MIG_IM_REGION_H__

#include "mig_config.h"
#include "mig_defs.h"

/*
******************************************************************************
*                               DATA TYPES
******************************************************************************
*/

/*
******************************************************************************
*                               2D - 3D REGION OF INTEREST
******************************************************************************
*/

typedef struct _mig_im_region_t
{
        float       centroid[3];    /* x , y , z centroid */
        float       radius;         /* used for circular and spherical regions : in pixels */
        int         size;           /* size in voxels */
        mig_lst_t   objs;           /* mig_im_region_t 2D objects making up 3D object */  

} mig_im_region_t;


/*
******************************************************************************
*   FUNCTIONS TO SAVE AND LOAD 2D - 3D REGION OF INTEREST FROM TEXT FILE
******************************************************************************
*/

//MIG_C_LINKAGE_START
//
//void
//mig_im_roi_save ( void *roi , void *file );
//
//void
//mig_im_roi_load ( void **roi , void *file );
//
//void
//mig_im_roi_free ( void **roi );

//MIG_C_LINKAGE_END

#endif /* __MIG_IM_REGION_H__ */
