#include "mig_im_rot.h"

/******************************************/
/* private function declarations */
static void _affine0 ( Mig8u *in , Mig8u *out ,
                      int w , int h ,
                      Mig64f x0 , Mig64f x1 , Mig64f x2 ,
                      Mig64f y0 , Mig64f y1 , Mig64f y2 );

static void _affine1 ( Mig8u *in , Mig8u *out ,
                      int w , int h ,
                      Mig64f x0 , Mig64f x1 , Mig64f x2 ,
                      Mig64f y0 , Mig64f y1 , Mig64f y2 );

static void _affine2 ( Mig8u *in , Mig8u *out ,
                      int w , int h ,
                      Mig64f x0 , Mig64f x1 , Mig64f x2 ,
                      Mig64f y0 , Mig64f y1 , Mig64f y2 );

/******************************************/
#define _BI( c , y1 , y2 , d ) \
{ \
        Mig64f p1 , p2; \
        p1 = (Mig64f) y1; \
        p2 = (Mig64f) y2 - (Mig64f) y1; \
        c = p1 + d * p2; \
}

/******************************************/
#define _CC( c , y1 , y2 , y3 , y4 , d ) \
{ \
      Mig64f p1 , p2 , p3 , p4; \
      p1 =  (Mig64f) y2; \
      p2 =  - (Mig64f) y1 + (Mig64f) y3; \
      p3 = 2.0 * (Mig64f) y1 - 2.0 * (Mig64f) y2 + (Mig64f) y3 - (Mig64f) y4; \
      p4 = - (Mig64f) y1 + (Mig64f) y2 - (Mig64f) y3 + (Mig64f) y4; \
      c = p1 + d * ( p2 + d * ( p3 + d * p4 ) ); \
}

/******************************************/
void
mig_im_rot_8u ( Mig8u *in , Mig8u *out ,
         	int w , int h ,
         	Mig64f theta ,
         	Mig64f rx , Mig64f ry ,
         	MigInterpType ip )
{

        Mig64f x0 , x1 , x2;
        Mig64f y0 , y1 , y2;

	theta = MIG_DEG_TO_RAD(theta);

        x0 = -cos( theta ) * rx + sin( theta ) * ry + rx;
        x1 = cos( theta );
        x2 = -sin( theta );

        y0 = -sin( theta ) * rx - cos( theta ) * ry + ry;
        y1 = sin( theta );
        y2 = cos( theta );

        switch( ip )
        {
                case MIG_NEAREST_NEIGHBOR :
                        _affine0 ( in , out ,
                                  w , h ,
                                  x0 , x1 , x2 ,
                                  y0 , y1 , y2 );
                        break;

                case MIG_BILINEAR :
                        _affine1 ( in , out ,
                                  w , h ,
                                  x0 , x1 , x2 ,
                                  y0 , y1 , y2 );
                        break;

                case MIG_BICUBIC :
                        _affine2 ( in , out ,
                                  w , h ,
                                  x0, x1 , x2 ,
                                  y0 , y1 , y2 );
                        break;

                default :
                	/* perform bilinear */
			_affine1 ( in , out ,
                                  w , h ,
                                  x0 , x1 , x2 ,
                                  y0 , y1 , y2 );
                        break;
        }
}


/******************************************/
static void
_affine0 ( Mig8u *in , Mig8u *out ,
          int w , int h ,
          Mig64f x0 , Mig64f x1 , Mig64f x2 ,
          Mig64f y0 , Mig64f y1 , Mig64f y2 )
{
        int i , j;
        int u , v;

        for ( j = 0 ; j < h ; ++j )
                for ( i = 0 ; i < w ; ++i )
                {
                        u = (int) ( ( x0 + x1 * i + x2 * j ) + 0.5 );
                        v = (int) ( ( y0 + y1 * i + y2 * j ) + 0.5 );

                        if ( ( u >= 0 ) &&
                             ( u < w ) &&
                             ( v >= 0 ) &&
                             ( v < h ) )
                                *( out + i + j * w ) =
                                *( in + u + v * w );
                        else
                                *( out + i + j * w ) = 0x00;
                }
}

/******************************************/
static void
_affine1 ( Mig8u *in , Mig8u *out ,
          int w , int h ,
          Mig64f x0 , Mig64f x1 , Mig64f x2 ,
          Mig64f y0 , Mig64f y1 , Mig64f y2 )

{
        int i , j , l , k;
        Mig64f u , v;
        Mig64f du , dv;

        Mig64f bi1 , bi2 , bi3;

        for ( j = 0 ; j < h ; ++j )
                for ( i = 0 ; i < w ; ++i )
                {
                        u = x0 + x1 * i + x2 * j;
                        v = y0 + y1 * i + y2 * j;

                        k = (int) floor( u );
                        l = (int) floor( v );

                        du = u - k;
                        dv = v - l;

	                if ( ( k >= 0 ) &&
                             ( ( k + 1 )< w ) &&
                             ( l >= 0 ) &&
                             ( ( l + 1 ) < h ) )
                        {
                                _BI( bi1 ,
                                    *( in + k     + l * w ) ,
                                    *( in + k + 1 + l * w ) ,
                                    du );

                                _BI( bi2 ,
                                    *( in + k     + ( l + 1 ) * w ) ,
                                    *( in + k + 1 + ( l + 1 ) * w ) ,
                                    du );

                                _BI( bi3 , bi1 , bi2 , dv );

                                *( out + i + j * w ) = (Mig8u)(bi3 + 0.5);
                        }
                        else
                                *( out + i + j * w ) = 0x00;
                }
}

/******************************************/
static void
_affine2 ( Mig8u *in , Mig8u *out ,
          int w , int h ,
          Mig64f x0 , Mig64f x1 , Mig64f x2 ,
          Mig64f y0 , Mig64f y1 , Mig64f y2 )
{
        int i , j , k , l;
        Mig64f u , v;
        Mig64f du , dv;

        Mig64f cu1 , cu2 , cu3 , cu4 , cu5;

        for ( j = 0 ; j < h ; ++j )
                for ( i = 0 ; i < w ; ++i )
                {
                        u = x0 + x1 * i + x2 * j;
                        v = y0 + y1 * i + y2 * j;

                        k = (int) floor( u );
                        l = (int) floor( v );

                        du = u - k;
                        dv = v - l;

                        if ( ( k >= 0 ) &&
                             ( ( k + 3 ) < w ) &&
                             ( l >= 0 ) &&
                             ( ( l + 3 ) < h ) )
                        {
                                _CC( cu1 ,
                                    *( in + k +     l * w ) ,
                                    *( in + k + 1 + l * w ) ,
                                    *( in + k + 2 + l * w ) ,
                                    *( in + k + 3 + l * w ) ,
                                    du );

                                _CC( cu2 ,
		                    *( in + k +     ( l + 1 ) * w ) ,
                                    *( in + k + 1 + ( l + 1 ) * w ) ,
                                    *( in + k + 2 + ( l + 1 ) * w ) ,
                                    *( in + k + 3 + ( l + 1 ) * w ) ,
                                    du );

                                _CC( cu3 ,
		                    *( in + k +     ( l + 2 ) * w ) ,
                                    *( in + k + 1 + ( l + 2 ) * w ) ,
                                    *( in + k + 2 + ( l + 2 ) * w ) ,
                                    *( in + k + 3 + ( l + 2 ) * w ) ,
                                    du );

                                _CC( cu4 ,
		                    *( in + k +     ( l + 3 ) * w ) ,
                                    *( in + k + 1 + ( l + 3 ) * w ) ,
                                    *( in + k + 2 + ( l + 3 ) * w ) ,
                                    *( in + k + 3 + ( l + 3 ) * w ) ,
                                    du );

	                        _CC( cu5 ,
	                            cu1 ,
	                            cu2 ,
	                            cu3 ,
	                            cu4 , dv );

                                if ( cu5 <= 0x00 )
                                        cu5 = 0x00;

                                if ( cu5 > 0xFF )
                                        cu5 = 0xFF;

                                *( out + i + j * w ) = (Mig8u)(cu5 + 0.5);
                        }
                        else
                                *( out + i + j * w ) = 0x00;
                }
}

