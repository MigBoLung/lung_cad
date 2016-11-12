#ifndef __MIG_IO_DCM_H__
#define __MIG_IO_DCM_H__

/* DCMTK  */
#include "dcmtk/config/osconfig.h"

#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "dcmtk/dcmdata/dctk.h"          /* for various dcmdata headers */
#include "dcmtk/dcmimgle/dcmimage.h"     /* for DicomImage */
#include "dcmtk/ofstd/ofstd.h"           /* for OFStandard */
#include "dcmtk/ofstd/ofstream.h"

/* MY STUFF */
#include "mig_config.h"
#include "mig_defs.h"

#include "mig_data_types.h"
#include "mig_data_dicom.h"
#include "mig_data_image.h"

MIG_C_LINKAGE_START

extern int
mig_dcm_get_info ( mig_dcm_data_t *dicom_data , 
                   mig_size_t *size_data );

extern void
mig_dcm_dump_info ( mig_dcm_data_t *dicom_data , 
                    mig_size_t *size_data , 
                    std::stringstream& os );

int
mig_dcm_rdir_16u ( Mig16u **dst , 
                   mig_dcm_data_t *dicom_data , 
                   mig_size_t *size_data ,
                   int voi_lut_type , 
                   int wc , 
                   int ww ,
                   int max_slice_num );

MIG_C_LINKAGE_END

#endif /* __MIG_IO_DCM_H__ */
