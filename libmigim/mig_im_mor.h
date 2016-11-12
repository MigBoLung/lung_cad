#ifndef __MIG_IM_MOR_H__
#define __MIG_IM_MOR_H__

#include "mig_config.h"
#include "mig_defs.h"

#include "mig_data_types.h"

typedef enum
{
	MIG_MORPH_CROSS ,
	MIG_MORPH_HORIZ ,
	MIG_MORPH_VERT ,
	MIG_MORPH_DIAG_1 ,
	MIG_MORPH_DIAG_2 ,
	MIG_MORPH_SQUARE

} MigElementarySeType;

typedef enum
{
	MIG_DISK_HEX ,
	MIG_DISK_SQUARE ,
	MIG_DISK_DIAMOND ,
	MIG_DISK_OCTAGON ,
	MIG_DISK_FULL

} MigDiskType;

MIG_C_LINKAGE_START

/* disk approximation */
void
mig_im_mor_erode_disk ( Mig8u *msk , int w , int h , int r , MigDiskType t );

void
mig_im_mor_dilate_disk ( Mig8u *msk , int w , int h , int r , MigDiskType t  );

/* elementary cross - diamond */
void
mig_im_mor_erode_cross ( Mig8u *msk , int w , int h );

void
mig_im_mor_dilate_cross ( Mig8u *msk , int w , int h );

/* elementary horizontal line */
void
mig_im_mor_erode_line_h ( Mig8u *msk , int w , int h );

void
mig_im_mor_dilate_line_h ( Mig8u *msk , int w , int h );

/* elementary veritcal line */
void
mig_im_mor_erode_line_v ( Mig8u *msk , int w , int h );

void
mig_im_mor_dilate_line_v ( Mig8u *msk , int w , int h );

/* elementary 45 deg line */
void
mig_im_mor_erode_line_d1 ( Mig8u *msk , int w , int h );

void
mig_im_mor_dilate_line_d1 ( Mig8u *msk , int w , int h );

/* elementary -45 deg line */
void
mig_im_mor_erode_line_d2 ( Mig8u *msk , int w , int h );

void
mig_im_mor_dilate_line_d2 ( Mig8u *msk , int w , int h );

/* elementary square */
void
mig_im_mor_erode_square ( Mig8u *msk , int w , int h );

void
mig_im_mor_dilate_square ( Mig8u *msk , int w , int h );

MIG_C_LINKAGE_END

#endif /* __MIG_IM_MOR_H__ */
