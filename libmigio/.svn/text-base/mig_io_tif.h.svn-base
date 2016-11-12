#ifndef __MIG_IO_TIFF_H__
#define __MIG_IO_TIFF_H__

#include "mig_config.h"
#include "mig_defs.h"

#include "mig_data_types.h"

typedef enum
{
	MIG_CMP_NONE			= 1,
	MIG_CMP_CCITTRLE		= 2,
	MIG_CMP_CCITTFAX3		= 3,
	MIG_CMP_CCITTFAX4		= 4,
	MIG_CMP_LZW			= 5,
	MIG_CMP_DEFLATE_ADOBE           = 8,
	MIG_CMP_DEFLATE                 = 32946,
	MIG_CMP_PACKBITS                = 32773

} MigTiffCmp;

typedef enum
{
	MIG_UNITS_NONE	= 1,
	MIG_UNITS_CM    = 3

} MigTiffUnit;

typedef struct
{
	Mig32u w;
	Mig32u h;
	Mig16u bps;
	Mig16u spp;
	Mig32f hres;
	Mig32f vres;
	Mig16u unitsres;
	Mig16u comp;
	char dsc[50+1];

} mig_tiff_info_t;

MIG_C_LINKAGE_START

extern int
mig_io_tif_w ( char *name , Mig8u *src , mig_tiff_info_t *info );

extern int
mig_io_tif_r ( char *name , Mig8u **dst , mig_tiff_info_t *info );

extern int
mig_io_tif_w_stack ( char *name , Mig8u *src ,
                     int slices ,
                     mig_tiff_info_t *info );

extern int
mig_io_tif_w_1 ( char *name , Mig8u *src , int w , int h );

extern int
mig_io_tif_w_8 ( char *name , Mig8u *src , int w , int h );

extern int
mig_io_tif_w_16 ( char *name , Mig16u *src , int w , int h );

extern int
mig_io_tif_w_stack_1 ( char *name , Mig8u *src ,
                       int w , int h , int slices );

extern int
mig_io_tif_w_stack_8 ( char *name , Mig8u *src ,
                       int w , int h , int slices );

extern int
mig_io_tif_w_stack_16 ( char *name , Mig8u *src ,
                        int w , int h , int slices );

extern int
mig_io_tif_w_rgb ( char *fname ,
		   unsigned char *im , int w , int h );

MIG_C_LINKAGE_END

#endif /* __MIG_IO_TIFF_H__ */

