#include "mig_im_draw.h"

/*******************************************************************/
/* EXPORTS */
/*******************************************************************/

void
mig_im_draw_line ( unsigned char *canvas ,
                   int w , int h ,
                   int x0 , int y0 ,
                   int x1 , int y1 ,
                   unsigned char color )
{
        int  d;
        int x , y;
        int ax , ay;
        int sx , sy;
        int dx , dy;

        /* clip coordinates to canvas size */
	if ( x0 < 0 )
		x0 = 0;

	if ( x0 > w )
		x0 = w;

	if ( y0 < 0 )
		y0 = 0;

	if ( y0 > h )
		y0 = h;

	if ( x1 < 0 )
		x1 = 0;

	if ( x1 > w )
		x1 = w;

	if ( y1 < 0 )
		y1 = 0;

	if ( y1 > h )
		y1 = h;

        dx = x1 - x0;
        ax = MIG_ABS( dx ) << 1;
        sx = MIG_SGN( dx );

        dy = y1 - y0;
        ay = MIG_ABS( dy ) << 1;
        sy = MIG_SGN( dy );

        x = x0;
        y = y0;

        if ( ax > ay )
        {
                d = ay - ( ax >> 1 );
                while ( x != x1 )
                {
                        canvas[ x + y * w ] = color;
                        if ( d >= 0 )
                        {
                                y += sy;
                                d -= ax;
                        }
                        x += sx;
                        d += ay;
                }
        }
        else
        {
                d = ax - ( ay >> 1 );
                while ( y != y1 )
                {
                        canvas[ x + y * w ] = color;
                        if ( d >= 0 )
                        {
                                x += sx;
                                d -= ay;
                        }
                        y += sy;
                        d += ax;
                }
        }

        canvas[ x + y * w ] = color;
}

/*******************************************************************/

void
mig_im_draw_circle ( unsigned char *canvas ,
                     int w , int h ,
                     int x0 , int y0 ,
                     int radius ,
                     unsigned char color )
{
        int f = 1 - radius;
        int ddF_x = 0;
        int ddF_y = -2 * radius;
        int x = 0;
        int y = radius;
        
        canvas[ x0 + ( y0 + radius ) * w ] = color;
        canvas[ x0 + ( y0 - radius ) * w ] = color;
        canvas[ x0 + radius + y0 * w ] = color;
        canvas[ x0 - radius + y0 * w ] = color;
        
        while ( x < y ) 
        {
                if ( f >= 0 ) 
                {
                        y--;
                        ddF_y += 2;
                        f += ddF_y;
                }
                
                x++;
                ddF_x += 2;
                f += ddF_x + 1;    
                
                canvas[ x0 + x + ( y0 + y ) * w ] = color;
                canvas[ x0 - x + ( y0 + y ) * w ] = color;
                
                canvas[ x0 + x + ( y0 - y ) * w ] = color;
                canvas[ x0 - x + ( y0 - y ) * w ] = color;
                
                canvas[ x0 + y + ( y0 + x ) * w ] = color;
                canvas[ x0 - y + ( y0 + x ) * w ] = color;
                

                canvas[ x0 + y + ( y0 - x ) * w ] = color;
                canvas[ x0 - y + ( y0 - x ) * w ] = color;
        }
}

/*******************************************************************/

void
mig_im_draw_line_rgb ( unsigned char *canvas ,
                       int w , int h ,
                       int x0 , int y0 ,
                       int x1 , int y1 ,
                       RGB_COLOR color )
{
        /* draw R component */
        mig_im_draw_line ( canvas , w , h , x0 , y0 , x1 , y1 , color[0] );

        /* draw G component */
        mig_im_draw_line ( canvas + w * h , w , h , x0 , y0 , x1 , y1 , color[1] );

        /* draw B component */
        mig_im_draw_line ( canvas + 2 * w * h , w , h , x0 , y0 , x1 , y1 , color[2] );
}

/*******************************************************************/

void
mig_im_draw_circle_rgb ( unsigned char *canvas ,
                         int w , int h ,
                         int x0 , int y0 ,
                         int radius ,
                         RGB_COLOR color )
{
        /* draw R component */
        mig_im_draw_circle ( canvas , w , h , x0 , y0 , radius , color[0] );

        /* draw G component */
        mig_im_draw_circle ( canvas + w * h , w , h , x0 , y0 , radius , color[1] );

        /* draw B component */
        mig_im_draw_circle ( canvas + 2 * w * h , w , h , x0 , y0 , radius , color[2] );        
}
