#ifndef __MIG_IM_DRAW_H__
#define __MIG_IM_DRAW_H__

#include "mig_config.h"
#include "mig_defs.h"
#include "mig_data_types.h"
#include "mig_data_image.h"
#include "mig_error_codes.h"

typedef unsigned char RGB_COLOR[3];

MIG_C_LINKAGE_START


extern void
mig_im_draw_line ( unsigned char *canvas ,
                   int w , int h ,
                   int x0 , int y0 ,
                   int x1 , int y1 ,
                   unsigned char color );

extern void
mig_im_draw_circle ( unsigned char *canvas ,
                     int w , int h ,
                     int x0 , int y0 ,
                     int radius ,
                     unsigned char color );

extern void
mig_im_draw_line_rgb ( unsigned char *canvas ,
                       int w , int h ,
                       int x0 , int y0 ,
                       int x1 , int y1 ,
                       RGB_COLOR color );

extern void
mig_im_draw_circle_rgb ( unsigned char *canvas ,
                         int w , int h ,
                         int x0 , int y0 ,
                         int radius ,
                         RGB_COLOR color );

MIG_C_LINKAGE_END

#endif /* __MIG_IM_DRAW_H__ */


