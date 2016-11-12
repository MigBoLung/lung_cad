#ifndef __MIG_CAD_DATA_H__
#define __MIG_CAD_DATA_H__

#include "mig_config.h"
#include "mig_defs.h"

#include "mig_data_types.h"
#include "mig_data_image.h"
#include "mig_data_dicom.h"

#include "libmigut.h"
#include "libmigst.h"

/***********************************************************/
/* CAD LOGGER NAME */
/***********************************************************/
#define CAD_LOGGER_NAME "CadLogger"

/***********************************************************/
/* PROCESSING FUNCTIONS PROTOTYPES */
/***********************************************************/
#define VERSION_LEN     11
#define BUILDDATE_LEN   9
#define DESCRIPTION_LEN 21
#define COMMENTS_LEN    21

typedef struct _mig_dll_info_t
{
    char Version[VERSION_LEN];
    char BuildDate[BUILDDATE_LEN];
    char Description[DESCRIPTION_LEN];
    char Comments[COMMENTS_LEN];

} mig_dll_info_t;

/* the function signatures that follow are used
   to load functions dynamically from dlls
   so that different segmentation and detection
   libraries can be used.
*/

#define MIG_INIT_F_NAME         "mig_init"
#define MIG_RUN_F_NAME          "mig_run"
#define MIG_CLEANUP_F_NAME      "mig_cleanup"
#define MIG_INFO_F_NAME         "mig_info"

/* forward definitions */
struct _mig_cad_data_t;

typedef int  (*mig_init_f)( mig_dic_t* , struct _mig_cad_data_t* );
typedef int  (*mig_run_f)();
typedef void (*mig_cleanup_f)(void*);
typedef void (*mig_info_f)( mig_dll_info_t* );

/***********************************************************/
/* CAD PROCESSING DATA STRUCTURE */
/***********************************************************/
typedef struct _mig_cad_data_t
{
    /* cpu information */
    cpuinfo_t cpu;

    /*****************************************/
    /* FILLED IN BY LOADER ROUTINE */
    
    /* dicom stack : original or resampled */
    Mig16u *stack;
    
    /* dicom stack size : original or resampled */
    mig_size_t stack_s;

    /* has the dicom stack been resampled */
    int resampled;

    /* size of stack before resampling took place */
    mig_size_t raw_s;

    /* dicom specific data */
    mig_dcm_data_t dicom_data;

    /* loader routine cleanup */
    mig_cleanup_f load_cleanup;

    /***********************************************/
    /* FILLED IN BY SEGMENTATION ROUTINE */
    
    /* segmented left dicom stack */
    Mig16u *stack_l;
    
    /* segmented left stack size */
    mig_size_t stack_l_s;

    /* segmented right dicom stack */
    Mig16u *stack_r;
    
    /* segmented right stack size */
    mig_size_t stack_r_s;

    /* bounding boxes for right [0] and left [1] stacks */
    mig_roi_t bb[2];

    /* segmentation cleanup */
    mig_cleanup_f seg_cleanup;

    /********************************************/
    /* FILLED IN BY DETECTION ROUTINE */
    
    /* lists of interest points after detection has
       been performed on the segmented left and right
       stacks -> contain pointers to mig_im_region_t's */
    
    mig_lst_t det_r;    /* detected mig_im_region_t list for right lung */
    mig_lst_t det_l;    /* detected mig_im_region_t list for left lung */

    mig_cleanup_f det_cleanup;
	mig_cleanup_f fpr1_cleanup;
    
    /********************************************/
    /* FINAL RESULTS */
    mig_lst_t results;

} mig_cad_data_t;

#endif /* __MIG_CAD_DATA_H__ */
