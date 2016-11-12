#include "mig_im_lab.h"
#include "mig_ut_mem.h"

/****************************************************************************/
#define TBL_SIZE        256
#define START_LAB       0x02
#define PIXON           0xFF
#define PIXOFF          0x00
#define MARK            0x01

/****************************************************************************/
/* PRIVATE */
/****************************************************************************/
static int
_search_dir[8][2] = { {0,1},{1,1},{1,0},
                      {1,-1},{0,-1},{-1,-1},
                      {-1,0},{-1,1}};

/****************************************************************************/
static void
_tracer_8u ( Mig8u *src ,
             int w ,
             int *cx , int *cy ,
             int *dir );

static void
_trace_contour_8u ( Mig8u *src ,
                    int w ,
                    int cx , int cy ,
                    int dir , int lab );

static int
_tbl_find ( Mig8u *tbl , int ind );

static void
_tbl_set ( Mig8u *tbl , int ind , int root );

static int
_tbl_union_2 ( Mig8u *tbl , int i , int j );

static int
_tbl_union_3 ( Mig8u *tbl , int i , int j , int k );

static int
_tbl_flatten ( Mig8u *tbl , int max );

static void
_tbl_relab ( Mig8u *s0  , Mig8u *s1 , Mig8u *tbl );

static void
_tbl_reset ( Mig8u *tbl , int max_lab );

static void
_label_2d ( Mig8u *idx ,
            int w ,
            int *new_lab , Mig8u *tbl );

static void
_label_3d ( Mig8u *idx , int w , int h ,
            int *new_lab , Mig8u *tbl );

/****************************************************************************/
/* EXPORTS */
/****************************************************************************/
void
mig_im_lab_on ( Mig8u *lab ,
                Mig8u *msk , int s , int id )
{
        int i;
        Mig8u res;
        Mig8u tmp = ( Mig8u ) id;

        for ( i = 0 ; i < s ; ++i , ++lab , ++msk )
        {
                res = (*lab)^tmp;
                res =    ( res        & 0x01 ) |
                       ( ( res >> 1 ) & 0x01 ) |
                       ( ( res >> 2 ) & 0x01 ) |
                       ( ( res >> 3 ) & 0x01 ) |
                       ( ( res >> 4 ) & 0x01 ) |
                       ( ( res >> 5 ) & 0x01 ) |
                       ( ( res >> 6 ) & 0x01 ) |
                       ( ( res >> 7 ) & 0x01 );
                *msk = res - 1;
        }
}

/****************************************************************************/
void
mig_im_lab_on_i ( Mig8u *lab ,
                  int s , int id )
{
        int i;
        Mig8u res;
        Mig8u tmp = ( Mig8u ) id;

        for ( i = 0 ; i < s ; ++i , ++lab )
        {
                res = (*lab)^tmp;
                res =   ( res        & 0x01 ) |
                      ( ( res >> 1 ) & 0x01 ) |
                      ( ( res >> 2 ) & 0x01 ) |
                      ( ( res >> 3 ) & 0x01 ) |
                      ( ( res >> 4 ) & 0x01 ) |
                      ( ( res >> 5 ) & 0x01 ) |
                      ( ( res >> 6 ) & 0x01 ) |
                      ( ( res >> 7 ) & 0x01 );
                *lab = res - 1;
        }
}

/****************************************************************************/
/* N.B. If number of connected components is bigger than 254
   there has been a label overflow.
*/
void
mig_im_lab_trace_2d ( Mig8u *src ,
                      int w , int h ,
                      int *num_cc )
{
        int i , j;
        int max_lab = START_LAB;
        Mig8u *idx;

        *num_cc = 0;

        /* first pass -> label start at 2 */
        for ( j = 1 ; j < ( h - 1 ) ; ++ j )
        {
                for ( i = 1 ; i < ( w - 1 ) ; ++i )
                {
                        idx = src + i + j * w;

                        if ( *idx == PIXOFF || *idx == MARK )
                                continue;

                        if ( *idx == PIXON )
                        {
                                /* external contour */
                                if ( *( idx - 1 ) == PIXOFF ||
                                     *( idx - 1 ) == MARK )
                                {
                                        *idx = max_lab;
                                        _trace_contour_8u ( src , w ,
                                                            i , j , 0 ,
                                                            max_lab );
                                        ++ ( *num_cc );
                                        ++ max_lab ;

                                        /* overflow */
                                        if ( max_lab == MARK )
                                                max_lab = START_LAB;
                                }
                                /* internal object pixel */
                                else
                                {
                                        *idx = *( idx - 1 );
                                }
                        }

                        /* internal contour */
                        if ( *( idx + 1 ) == PIXOFF )
                        {
                                _trace_contour_8u ( src , w ,
                                                    i , j , 1 , *idx );
                        }
                }
        }

        /* second pass -> reset MARK and subtract 1 from all labels */
        for ( i = 0 ; i < w * h ; ++ i , ++ src )
                *src = ( *src <= MARK ) ? PIXOFF : (*src - MARK );
}

/****************************************************************************/
void
mig_im_lab_union_2d ( Mig8u *src , int w , int h ,  int *num_cc )
{
        int i, j, curr_lab = 1;
        Mig8u *idx, *t;
        Mig8u tbl[TBL_SIZE];

        *num_cc = -1;

        /* zero out equivalence table */
        mig_memz( &tbl , sizeof(Mig8u) * TBL_SIZE );
        t = ( Mig8u* ) &tbl;

        for ( j = 1; j < ( h - 1 ) ; ++ j )
                for ( i = 1 ; i < ( w - 1 ) ; ++ i )
                {
                        idx = src + i + j * w;

                        if ( *idx == PIXOFF )
                                continue;

                        if ( curr_lab == TBL_SIZE )
                        {
                                curr_lab = _tbl_flatten ( t, curr_lab );
                                _tbl_relab ( src , idx , t );
                                _tbl_reset ( t , TBL_SIZE );
                        }

                        _label_2d ( idx , w , &curr_lab , t );
                }

        *num_cc = _tbl_flatten ( t , curr_lab ) - 1;
        _tbl_relab ( src , idx + 1 , t );
}

/****************************************************************************/
void
mig_im_lab_union_3d ( Mig8u *src , int w , int h , int z , int *num_cc )
{
        int i , j , k;            /* indexes */
        int lab;                  /* next label to assign */
        Mig8u *idx , *t;
        Mig8u tbl[TBL_SIZE];      /* equivalence table -> 256 entries */


        /* zero out equivalence table */
        mig_memz( &tbl , sizeof(Mig8u) * TBL_SIZE );

        /* helper pointer */
        t = (Mig8u*) &tbl;

        /* zero number of connected components */
        *num_cc = 0;

        /* label first slice separately */
        mig_im_lab_union_2d ( src , w , h , num_cc );

        /* update equivalence table for first slice */
        for ( i = 1 ; i <= *num_cc ; ++i )
                tbl[i] = i;

        /* next label to assign is num_cc + 1 after
           labeling slice number one */
        lab = *num_cc + 1;

        for ( k = 1 ; k < z ; ++k )
                for ( j = 1 ; j < h ; ++j )
                        for ( i = 1 ; i < w ; ++i )
                        {
                                /* current pixel */
                                idx = src + i + j * w + k * w * h;

                                /* current pixel is off */
                                if ( *idx == PIXOFF )
                                        continue;

                                /* current pixel is on */

                                /* check wether there is label overflow */
                                if ( lab == TBL_SIZE )
                                {
                                        /* find minimum equivalent label for
                                           each class */
                                        lab = _tbl_flatten ( t , TBL_SIZE );

                                        /* relabel all pixels up to
                                           current one */
                                        _tbl_relab ( src , idx , t );

                                        /* reset equivalence table */
                                        _tbl_reset ( t , lab );
                                }

                                /* check neighbours */
                                _label_3d ( idx , w , h , &lab , t );
                        }

        /* flatten table and get next label to assign.
           It minus one is the number of connected components */
        *num_cc = _tbl_flatten ( t , lab ) - 1;

        /* relabel last chunck of data */
        _tbl_relab ( src , idx + 1 , t );
}

/****************************************************************************/
/* private functions */
/****************************************************************************/
static void
_tracer_8u ( Mig8u *src , int w , int *cx , int *cy , int *dir )
{
        int i, y, x;
        Mig8u *pix;

        for ( i = 0 ; i < 7 ; ++ i )
        {
                y = *cy + _search_dir[*dir][0];
                x = *cx + _search_dir[*dir][1];

                pix = src + x + y * w;

                if ( ( *pix == PIXOFF ) ||
                     ( *pix == MARK ) )
                {
                        *pix = MARK;
                        *dir = (*dir + 1) % 8;
                }
                else
                {
                        *cy = y;
                        *cx = x;
                        break;
                }
        }
}

/****************************************************************************/
static void
_trace_contour_8u ( Mig8u *src , int w , int cx , int cy , int dir , int lab )
{
        char stop = 0, again = 1;
        int fx, fy, sx = cx, sy = cy;

        *( src + cx + cy * w ) = lab;           /* first contour pixel */
        _tracer_8u ( src , w , &cx , &cy , &dir );

        if(cx != sx || cy != sy)
        {
                fx = cx;
                fy = cy;

                while (again)
                {
                        dir = (dir + 6) % 8;
                        *( src + cx + cy * w ) = lab;
                        _tracer_8u ( src , w , &cx , &cy , &dir );

                        if(cx == sx && cy == sy)
                        {
                                stop = 1;
                        }
                        else if (stop)
                        {
                                if(cx == fx && cy == fy)
                                {
                                        again = 0;
                                }
                                else
                                {
                                        stop = 0;
                                }
                        }
                }
        }
}

/****************************************************************************/
/* find root of tree from node ind */
static int
_tbl_find ( Mig8u *tbl , int ind )
{
        int root = ind;
        while ( tbl[ root ] < root )
                root = tbl[ root ];
        return root;
}

/****************************************************************************/
/* set all nodes to point to a new root. */
static void
_tbl_set ( Mig8u *tbl , int ind , int root )
{
        int i = ind , j;

        while ( tbl[i] < i )
        {
                j = tbl[i];
                tbl[i] = root;
                i = j;
        }

        tbl[i] = root;
}

/****************************************************************************/
/* combine two trees containing node i and j.
   Return the root of the union. */
static int
_tbl_union_2 ( Mig8u *tbl , int i , int j )
{
        int rooti, rootj;

        rooti = _tbl_find ( tbl , i );
        if ( i != j )
        {
                rootj = _tbl_find ( tbl , j );
                if ( rooti > rootj )
                        rooti = rootj;

                _tbl_set ( tbl , i , rooti );
                _tbl_set ( tbl , j , rooti );
        }

        return rooti;
}

/****************************************************************************/
/* combine three trees containing nodes i , j and k.
   Return the root of the union. */

#define MIN3(X,Y,Z) \
(((X)<(Y))?(((X)<(Z))?(X):(Z)):((Y)<(Z))?(Y):(Z))

static int
_tbl_union_3 ( Mig8u *tbl , int i , int j , int k )
{
        int rooti, rootj, rootk;

        rooti = _tbl_union_2 ( tbl , i , j );
        rootj = _tbl_union_2 ( tbl , i , k );
        rootk = _tbl_union_2 ( tbl , j , k );

        return ( MIN3( rooti , rootj , rootk ) );
}

#undef MIN3

/****************************************************************************/
/* flatten union-find tree : for each label the minimum equivalence
   label is found and assigned to it. For example :

   initial table :

   index : 0 1 2 3
   value : 0 1 1 2

   after flattening :

   index : 0 1 2 3
   value : 0 1 1 1

   returns next label to assign

   attention : after performing flattening operation
   data should be relabeled immediately as new labels
   will overwrite old ones.
*/
static int
_tbl_flatten ( Mig8u *tbl , int max )  /* max -> next non assigned label */
{
        int i , lab = 1;

        for ( i = 1 ; i < max ; ++i )
        {
                if ( tbl[ i ] < i )
                        tbl[i] = tbl[ tbl[i] ];
                else
                {
                        tbl[i] = lab;
                        ++ lab;
                }
        }

        return lab;
}

/****************************************************************************/
/* relabel pixels starting at s0 and
   ending at s1 ( excluded ).
   Pixels are relabeled using minimum in
   there equivalence class. Before calling
   this functionthe table tbl should have been
   flattened.
*/
static void
_tbl_relab ( Mig8u *s0 , Mig8u *s1 , Mig8u *tbl )
{
        while ( s0 < s1 )
        {
                *s0 = tbl[ *s0 ];
                ++ s0;
        }
}

/****************************************************************************/
/* set all values up to max_lab ( excluded )
   equal to there index */
static void
_tbl_reset ( Mig8u *tbl , int max_lab )
{
        int i;
        mig_memz( tbl , TBL_SIZE * sizeof( Mig8u ) );
        for ( i = 0 ; i < max_lab ; ++i )
                tbl[i] = i;
}

/****************************************************************************/
/* check 3d neighbours of pixel idx. We use only three neighbours :
   1. above ( upper slice )
   2. above ( upper row )
   3. left.
*/
static void
_label_3d ( Mig8u *idx , int w , int h , int *lab , Mig8u *tbl )
{
        Mig8u n[3];

        n[0] = *( idx - ( w * h ) );    /* upper slice */
        n[1] = *( idx - w );            /* upper row */
        n[2] = *( idx - 1 );            /* left */

        if ( n[0] )
        {
                if ( n[1] )
                {
                        if ( n[2] )
                        {
                                *idx = (Mig8u)
                                	_tbl_union_3 ( tbl ,
                                		n[0] , n[1] , n[2] );
                                return;
                        }

                        *idx = (Mig8u)
                        	_tbl_union_2 ( tbl ,
                        		n[0] , n[1] );
                        return;
                }

                if ( n[2] )
                {
                        *idx =
                        	_tbl_union_2 ( tbl ,
                        	n[0] , n[2] );
                        return;
                }

                *idx = tbl[n[0]];
                return;
        }

        if ( n[1] )
        {
                if ( n[2] )
                {
                        *idx = (Mig8u)
                        	_tbl_union_2 ( tbl ,
                        	n[1] , n[2] );
                        return;
                }

                *idx = tbl[n[1]];
                return;
        }

        if ( n[2] )
        {
                *idx = tbl[n[2]];
                return;
        }

        /* none of the neighbours has been labeled :
           assign new label to pixel and increment
           current label by one */
        *idx = ( Mig8u ) *lab;
        tbl[ *idx ] = *idx;
        ++ ( *lab );
}

/****************************************************************************/
/* check 2d neighbours of pixel idx. We use four neighbours :
   1. upper left
   2. upper
   3. upper right
   4. left
*/
static void
_label_2d ( Mig8u *idx , int w , int *lab , Mig8u *tbl )
{
        Mig8u n[4];

        n[0] = *( idx - w - 1 );
        n[1] = *( idx - w );
        n[2] = *( idx - w + 1 );
        n[3] = *( idx - 1 );

        if ( n[1] )
        {
                *idx = tbl[n[1]];
                return;
        }

        if ( n[2] )
        {
                if ( n[0] )
                {
                        *idx =
                        	_tbl_union_2 ( tbl ,
				n[0] , n[2] );
                        return;
                }

                if ( n[3] )
                {
                        *idx =
                        	_tbl_union_2 ( tbl ,
				n[2] , n[3] );
                        return;
                }

                *idx = tbl[n[2]];
                return;
        }

        if ( n[0] )
        {
                *idx = tbl[n[0]];
                return;
        }

        if ( n[3] )
        {
                *idx = tbl[n[3]];
                return;
        }

        /* none of the neighbours has been labeled :
           assign new label to pixel and increment
           current label by one */
        *idx = ( Mig8u ) *lab;
        tbl[ *idx ] = *idx;
        ( *lab ) ++;
}

/****************************************************************************/
#undef TBL_SIZE
#undef START_LAB
#undef PIXON
#undef PIXOFF
#undef MARK

