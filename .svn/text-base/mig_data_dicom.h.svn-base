#ifndef __MIG_DATA_DICOM_H__
#define __MIG_DATA_DICOM_H__

#include "mig_config.h"
#include "mig_defs.h"

#include "mig_ut_cpu.h"

#define MIG_VOI_LUT_STORED                  0
#define MIG_VOI_LUT_FORCE                   1
#define MIG_VOI_LUT_WIN_MIN_MAX             2
#define MIG_VOI_LUT_WIN_MIN_MAX_NO_EXTREMES 3
#define MIG_VOI_LUT_HIST                    4

#define MIG_DCM_FIELD_LEN     128
#define MIG_DCM_DATE_LEN      (8+1)
#define MIG_DCM_TIME_LEN      (6+1)

#define MIG_DCM_UNKNOWN_TAG             "UNKNOWN"
#define MIG_DCM_UNKNOWN_DATE_TAG        "00000000"
#define MIG_DCM_UNKNOWN_TIME_TAG        "000000"

typedef struct _mig_dcm_data_t
{
        /* patient id - unique id in the hospital */
        char patient_id[MIG_DCM_FIELD_LEN];
        
        /* patient name */
        char patient_name[MIG_DCM_FIELD_LEN];
        
        /* study unique identifier  */
        char study_uid[MIG_DCM_FIELD_LEN];
        
        /* study date ddmmyy */
        char study_date[MIG_DCM_DATE_LEN];
        
        /* study time - hhmmss */
        char study_time[MIG_DCM_TIME_LEN];
        
        /* series unique identifier */
        char series_uid[MIG_DCM_FIELD_LEN];
        
        /* dicom images source directory */
        char storage[MAX_PATH];

        /* stack images file names */
        char **file_names;

		/* stack images uids */
		char **instance_uids;

		/* data received date ddmmyy */
		char received_date[MIG_DCM_DATE_LEN];

		/* data received time */
		char received_time[MIG_DCM_TIME_LEN];

		/* data sent date */
		char sent_date[MIG_DCM_DATE_LEN];

		/* data sent time */
		char sent_time[MIG_DCM_TIME_LEN];

		/* CAD processed date */
		char processed_date[MIG_DCM_DATE_LEN];

		/* CAD processed time */
		char processed_time[MIG_DCM_TIME_LEN];

		/* data convolution kernel */
		char convolution_kernel[MIG_DCM_FIELD_LEN];

} mig_dcm_data_t;

#endif /* __MIG_DATA_DICOM_H__ */

/*******************************************************************/
/* DIXYGEN DOCUMENTATION */
/*******************************************************************/

/** \file mig_data_dicom.h
    \brief Global LUNG CAD dicom data.
    
    This file defines the structure containing
    DICOM related information as well as source of
    DICOM images and destination of CAD results.
*/
