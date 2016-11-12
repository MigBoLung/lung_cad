#ifndef __MIG_CAD_PARAM_KEYS_H__
#define __MIG_CAD_PARAM_KEYS_H__

/***********************************************************/
/* LOGGING SYSTEM */
/***********************************************************/

/* keys into ini hashtable */
#define PARAM_LOG_INI            "log:logini"

/* default values */
#define DEFAULT_PARAM_LOG_INI    "cadlog.ini"

/***********************************************************/
/* CAD GENERAL */
/***********************************************************/

/* keys into ini hashtable */
#define PARAM_CAD_DIR_OUT               "general:dir_out"
#define PARAM_CAD_DB_FILE               "general:db_file"
#define PARAM_CAD_QUEUE_LEN             "general:queue_len"
#define PARAM_CAD_RETRY_READ            "general:retry_read"
#define PARAM_CAD_RETRY_WRITE           "general:retry_write"

#define PARAM_CAD_LOAD_DLL              "io:dll"
#define PARAM_CAD_SEGMENT               "segmentation:perform_segmentation"
#define PARAM_CAD_SEGMENT_DLL           "segmentation:dll"
#define PARAM_CAD_DETECT                "detection:perform_detection"
#define PARAM_CAD_DETECT_DLL            "detection:dll"

/* default values */
#define DEFAULT_PARAM_CAD_DIR_OUT       "results/"
#define DEFAULT_PARAM_CAD_DB_FILE       "store.db"
#define DEFAULT_PARAM_CAD_QUEUE_LEN     1
#define DEFAULT_PARAM_CAD_RETRY_READ    10
#define DEFAULT_PARAM_CAD_RETRY_WRITE   10

#define DEFAULT_PARAM_CAD_SEGMENT       1
#define DEFAULT_PARAM_CAD_DETECT        1

/***********************************************************/
/* CAD DICOM LOADING */
/***********************************************************/

/* keys into ini hashtable */
#define PARAM_DCM_RESAMPLE              "io:resample"
#define PARAM_DCM_ZRES                  "io:target_z_resolution"
#define PARAM_DCM_DUMP                  "io:dump"

#define PARAM_DCM_SLICE_LIMIT           "io:slice_limit"

#define PARAM_DCM_VOI_LUT_TYPE          "io/voi_lut:type"
#define PARAM_DCM_VOI_LUT_WC            "io/voi_lut:wc"
#define PARAM_DCM_VOI_LUT_WW            "io/voi_lut:ww"
#define PARAM_DCM_VOI_LUT_IGNORE_PERC   "io/voi_lut:ignore_perc"

/* default values */
#define DEFAULT_PARAM_DCM_RESAMPLE              0
#define DEFAULT_PARAM_DCM_ZRES                  0.0
#define DEFAULT_PARAM_DCM_DUMP                  0

#define DEFAULT_PARAM_DCM_SLICE_LIMIT           700

#define DEFAULT_PARAM_DCM_VOI_LUT_TYPE          1
#define DEFAULT_PARAM_DCM_VOI_LUT_WC            -600
#define DEFAULT_PARAM_DCM_VOI_LUT_WW            1200
#define DEFAULT_PARAM_DCM_VOI_LUT_IGNORE_PERC   10

/***********************************************************/
/* CAD SEGMENTATION */
/***********************************************************/

/* SEGMENTATION FILTERING */
#define SEG_FILTER_NONE         0
#define SEG_FILTER_MED_CROSS_3  1
#define SEG_FILTER_MED_CROSS_5  2
#define SEG_FILTER_MED_BOX_3    3
#define SEG_FILTER_MED_BOX_5    4
#define SEG_FILTER_TOMITA_3     5
#define SEG_FILTER_TOMITA_5     6
#define SEG_FILTER_NAGAO_5      7
#define SEG_FILTER_NAGAO_7      8

/* keys into ini hashtable */
#define PARAM_SEG_THR_FILTER_INPLACE    "segmentation/threshold:filter_inplace"
#define PARAM_SEG_THR_FILTER            "segmentation/threshold:filter"
#define PARAM_SEG_THR_G0                "segmentation/threshold:g0"
#define PARAM_SEG_THR_G1                "segmentation/threshold:g1"
#define PARAM_SEG_THR_G2                "segmentation/threshold:g2"

#define PARAM_SEG_SEP_MIN_AREA      "segmentation/lung_separate:sep_min_area"
#define PARAM_SEG_SEP_THR_INI       "segmentation/lung_separate:sep_accum_thr_ini"
#define PARAM_SEG_SEP_THR_MAX_SIZE  "segmentation/lung_separate:sep_accum_thr_size_max"

#define PARAM_SEG_DUMP              "segmentation/debug:dump"
#define PARAM_SEG_DIR_DUMP          "segmentation/debug:dir_dump"

/* default values */
#define DEFAULT_PARAM_SEG_OPT_DUMP      0
#define DEFAULT_PARAM_SEG_OPT_DIR_DUMP  "segmentation/"
#define DEFAULT_PARAM_SEG_INPLACE       0
#define DEFAULT_PARAM_SEG_FILTER        SEG_FILTER_NONE
#define DEFAULT_PARAM_SEG_G0            -1000
#define DEFAULT_PARAM_SEG_G1            0
#define DEFAULT_PARAM_SEG_G2            -500
#define DEFAULT_PARAM_SEG_SEP_MIN_AREA  0
#define DEFAULT_PARAM_SEG_SEP_THR_INI   3
#define DEFAULT_PARAM_SEG_SEP_THR_MAX   0.60

/***********************************************************/
/* CAD DETECTION */
/***********************************************************/

/* keys into ini hashtable */

#define PARAM_DET_FR_RADII          "detection/radial:radii"
#define PARAM_DET_FR_THR            "detection/radial:threshold"
#define PARAM_DET_FR_THR_TYPE       "detection/radial:is_thr_percent_of_max"
#define PARAM_DET_FR_BETA_THR		"detection/radial:beta_threshold"

#define PARAM_DET_SSPACE_SPACING    "detection/sspace:spacing"
#define PARAM_DET_SSPACE_INCREMENT  "detection/sspace:increment"

#define PARAM_DET_SSPACE_MIN_DIAM   "detection/sspace:min_nod_diam"
#define PARAM_DET_SSPACE_MAX_DIAM   "detection/sspace:max_nod_diam"
#define PARAM_DET_SSPACE_THR        "detection/sspace:threshold"

#define PARAM_DET_DUMP              "detection/debug:dump"
#define PARAM_DET_DIR_DUMP          "detection/debug:dir_dump"

/* default values */
#define DEFAULT_PARAM_DET_FR_THR            0.03f
#define DEFAULT_PARAM_DET_FR_THR_TYPE       0
#define DEFAULT_PARAM_DET_FR_BETA_THR       0.1f

#define DEFAULT_PARAM_DET_SSPACE_SPACING    0
#define DEFAULT_PARAM_DET_SSPACE_INCREMENT  1.0f
#define DEFAULT_PARAM_DET_SSPACE_MIN        2.0f
#define DEFAULT_PARAM_DET_SSPACE_MAX        20.0f
#define DEFAULT_PARAM_DET_SSPACE_THR        0.5f
#define DEFAULT_PARAM_DET_DUMP              0
#define DEFAULT_PARAM_DET_DIR_DUMP          "detection/"

/***********************************************************/
/* CAD FPR1 */
/***********************************************************/

/* keys into ini hashtable */

#define PARAM_FPR1                          "fpr1:perform_fpr1"
#define PARAM_FPR1_DLL                      "fpr1:dll"

#define PARAM_FPR1_MAX_SCAN_DISTANCE        "fpr1:delta_tolerance"
#define PARAM_FPR1_DELTA_TOLERANCE          "fpr1:max_scan_distance"

#define PARAM_FPR1_MIN_MAX_RADIUS          "fpr1:min_max_radius"

#define PARAM_FPR1_MAX_OBJ_LENGHT           "fpr1:max_obj_length"

#define PARAM_FPR1_MAX_MEAN_GL				"fpr1:max_mean_GL"

#define PARAM_FPR1_MIN_MEAN_GL				"fpr1:min_mean_GL"

#define PARAM_FPR1_MAX_ANGLE_L2             "fpr1:max_angle_l2"
#define PARAM_FPR1_MAX_ANGLE_L3             "fpr1:max_angle_l3"
#define PARAM_FPR1_MAX_ANGLE_L4             "fpr1:max_angle_l4"
#define PARAM_FPR1_MAX_ANGLE_L5             "fpr1:max_angle_l5"
#define PARAM_FPR1_MAX_ANGLE_L6             "fpr1:max_angle_l6"
#define PARAM_FPR1_MAX_ANGLE_L7             "fpr1:max_angle_l7"
#define PARAM_FPR1_MAX_ANGLE_L8             "fpr1:max_angle_l8"
#define PARAM_FPR1_MAX_ANGLE_L9             "fpr1:max_angle_l9"
#define PARAM_FPR1_MAX_ANGLE_L10            "fpr1:max_angle_l10"

#define PARAM_FPR1_MAX_OBJ_VOL_LOW_L2       "fpr1:max_obj_volume_low_l2"
#define PARAM_FPR1_MAX_OBJ_VOL_LOW_L3       "fpr1:max_obj_volume_low_l3"
#define PARAM_FPR1_MAX_OBJ_VOL_LOW_L4       "fpr1:max_obj_volume_low_l4"
#define PARAM_FPR1_MAX_OBJ_VOL_LOW_L5       "fpr1:max_obj_volume_low_l5"
#define PARAM_FPR1_MAX_OBJ_VOL_LOW_L6       "fpr1:max_obj_volume_low_l6"
#define PARAM_FPR1_MAX_OBJ_VOL_LOW_L7       "fpr1:max_obj_volume_low_l7"
#define PARAM_FPR1_MAX_OBJ_VOL_LOW_L8       "fpr1:max_obj_volume_low_l8"
#define PARAM_FPR1_MAX_OBJ_VOL_LOW_L9       "fpr1:max_obj_volume_low_l9"
#define PARAM_FPR1_MAX_OBJ_VOL_LOW_L10      "fpr1:max_obj_volume_low_l10"

#define PARAM_FPR1_MAX_OBJ_VOL_HIGH_L2       "fpr1:max_obj_volume_high_l2"
#define PARAM_FPR1_MAX_OBJ_VOL_HIGH_L3       "fpr1:max_obj_volume_high_l3"
#define PARAM_FPR1_MAX_OBJ_VOL_HIGH_L4       "fpr1:max_obj_volume_high_l4"
#define PARAM_FPR1_MAX_OBJ_VOL_HIGH_L5       "fpr1:max_obj_volume_high_l5"
#define PARAM_FPR1_MAX_OBJ_VOL_HIGH_L6       "fpr1:max_obj_volume_high_l6"
#define PARAM_FPR1_MAX_OBJ_VOL_HIGH_L7       "fpr1:max_obj_volume_high_l7"
#define PARAM_FPR1_MAX_OBJ_VOL_HIGH_L8       "fpr1:max_obj_volume_high_l8"
#define PARAM_FPR1_MAX_OBJ_VOL_HIGH_L9       "fpr1:max_obj_volume_high_l9"
#define PARAM_FPR1_MAX_OBJ_VOL_HIGH_L10      "fpr1:max_obj_volume_high_l10"

/* default values */
#define DEFAULT_PARAM_FPR1                      1
#define DEFAULT_PARAM_FPR1_MAX_SCAN_DISTANCE    3
#define DEFAULT_PARAM_FPR1_DELTA_TOLERANCE      7.75f

#define DEFAULT_PARAM_FPR1_MAX_OBJ_LENGHT       13.0f
#define DEFAULT_PARAM_FPR1_MIN_MAX_RADIUS			1.6f
#define DEFAULT_PARAM_FPR1_MAX_MEAN_GL				1.f
#define DEFAULT_PARAM_FPR1_MIN_MEAN_GL				0.f
/***********************************************************/
/* CAD FPR2 */
/***********************************************************/

/* keys into ini hashtable */

#define PARAM_FPR2                      "fpr2:perform_fpr2"
#define PARAM_FPR2_DLL                  "fpr2:dll"
#define PARAM_FPR2_SVM_MODEL_FILE       "fpr2:svm_model_file"
#define PARAM_FPR2_SVM_NORM_FILE        "fpr2:svm_norm_file"

#define PARAM_FPR2_RESIZED_LEN          "fpr2:resized_len"

/* OLD FPR2 PARAMS (NEEDED ON FOR SOLUTION COMPILATION */
#define PARAM_FPR2_ROTATIONS            "fpr2:rotations"

#define PARAM_FPR2_MIN_ROT_POS_L2       "fpr2:min_rot_pos_l2"
#define PARAM_FPR2_MIN_ROT_POS_L3       "fpr2:min_rot_pos_l3"
#define PARAM_FPR2_MIN_ROT_POS_L4       "fpr2:min_rot_pos_l4"
#define PARAM_FPR2_MIN_ROT_POS_L5       "fpr2:min_rot_pos_l5"
#define PARAM_FPR2_MIN_ROT_POS_L6       "fpr2:min_rot_pos_l6"
#define PARAM_FPR2_MIN_ROT_POS_L7       "fpr2:min_rot_pos_l7"
#define PARAM_FPR2_MIN_ROT_POS_L8       "fpr2:min_rot_pos_l8"
#define PARAM_FPR2_MIN_ROT_POS_L9       "fpr2:min_rot_pos_l9"
#define PARAM_FPR2_MIN_ROT_POS_L10      "fpr2:min_rot_pos_l10"
#define PARAM_FPR2_MIN_ROT_POS_L11      "fpr2:min_rot_pos_l11"
#define PARAM_FPR2_MIN_ROT_POS_L12      "fpr2:min_rot_pos_l12"
#define PARAM_FPR2_MIN_ROT_POS_L13      "fpr2:min_rot_pos_l13"
#define PARAM_FPR2_MIN_ROT_POS_L14      "fpr2:min_rot_pos_l14"
#define PARAM_FPR2_MIN_ROT_POS_L15      "fpr2:min_rot_pos_l15"

#define PARAM_FPR2_MIN_POS_VIEWS_L2     "fpr2:min_pos_views_l2"
#define PARAM_FPR2_MIN_POS_VIEWS_L3     "fpr2:min_pos_views_l3"
#define PARAM_FPR2_MIN_POS_VIEWS_L4     "fpr2:min_pos_views_l4"
#define PARAM_FPR2_MIN_POS_VIEWS_L5     "fpr2:min_pos_views_l5"
#define PARAM_FPR2_MIN_POS_VIEWS_L6     "fpr2:min_pos_views_l6"
#define PARAM_FPR2_MIN_POS_VIEWS_L7     "fpr2:min_pos_views_l7"
#define PARAM_FPR2_MIN_POS_VIEWS_L8     "fpr2:min_pos_views_l8"
#define PARAM_FPR2_MIN_POS_VIEWS_L9     "fpr2:min_pos_views_l9"
#define PARAM_FPR2_MIN_POS_VIEWS_L10    "fpr2:min_pos_views_l10"
#define PARAM_FPR2_MIN_POS_VIEWS_L11    "fpr2:min_pos_views_l11"
#define PARAM_FPR2_MIN_POS_VIEWS_L12    "fpr2:min_pos_views_l12"
#define PARAM_FPR2_MIN_POS_VIEWS_L13    "fpr2:min_pos_views_l13"
#define PARAM_FPR2_MIN_POS_VIEWS_L14    "fpr2:min_pos_views_l14"
#define PARAM_FPR2_MIN_POS_VIEWS_L15    "fpr2:min_pos_views_l15"
/* END OLD FPR2 PARAMS */

#define PARAM_FPR2_MIP_RATIO			"fpr2:mip_ratio"
#define PARAM_FPR2_MIN_POS_LABELS		"fpr2:min_pos_labels"
#define PARAM_FPR2_CROP_SIZES			"fpr2:crop_sizes"
#define	PARAM_FPR2_MOM_ORDERS			"fpr2:mom_orders"

/* default values */
/* OLD FPR2 PARAMS */
#define DEFAULT_PARAM_FPR2              0
#define DEFAULT_PARAM_FPR2_ROTATIONS    24

/* END OLD FPR2_PARAMS */
#define DEFAULT_PARAM_FPR2_RESIZED_LEN	64


#define DEFAULT_PARAM_FPR2_MIP_RATIO	0.7
#define DEFAULT_PARAM_FPR2_MIN_POS_LABELS	2
#define DEFAULT_PARAM_FPR2_CROP_SIZES	64
#define DEFAULT_PARAM_FPR2_MOM_ORDERS   { 1, 2, 3, 4, 5, 6, 7, 8 }



#endif /* __MIG_CAD_PARAM_KEYS_H__ */
