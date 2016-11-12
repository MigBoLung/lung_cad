#include "mig_im_lab.h"
#include "mig_ut_mem.h"

/****************************************************************************/
#define TBL_SIZE	65536

/****************************************************************************/
/* PRIVATE */
/****************************************************************************/

static void
_copy_to_uint16 ( unsigned char *src ,
                  unsigned short *dst ,
                  int s );

static void
_copy_from_uint16 ( unsigned short *src ,
                    unsigned char *dst ,
                    int s );

static void
_label_3d ( unsigned short *idx ,
            int w , int h ,
            int *lab ,
            unsigned short *tbl );

static int
_tbl_flatten ( unsigned short *tbl ,
               int max );

static void
_tbl_relab ( unsigned short *buff ,
             int len ,
             unsigned short *tbl );
static int
_tbl_find ( unsigned short *tbl ,
            int ind );

static void
_tbl_set ( unsigned short *tbl ,
           int ind , int root );

static int
_tbl_union_2 ( unsigned short *tbl ,
               int i , int j );

static int
_tbl_union_3 ( unsigned short *tbl ,
               int i , int j , int k );

/****************************************************************************/
/* EXPORTS */
/****************************************************************************/
int
mig_im_lab_union_3d_fast ( Mig8u *im ,
                           int w , int h , int z ,
                           int *num_cc )
{
	unsigned short *buffer = NULL;	/* work buffer */
	unsigned short *idx    = NULL;  /* buffer index */
	unsigned short *tbl    = NULL;	/* equivalence table */

	int lab;     			/* next label to assign */
	int i , j , k;			/* indices */

	/* zero out number of connected components */
	*num_cc = 0;

	/* make place for buffer */
	buffer = (unsigned short*)
		mig_malloc ( w * h * z * sizeof(unsigned short) );
	if ( buffer == NULL )
		return -1;

	/* make place for equivalece table */
	tbl = (unsigned short*)
		calloc ( TBL_SIZE ,
		         sizeof(unsigned short) );
	if ( tbl == NULL )
		goto error;

	/* 2d label first slice */
        mig_im_lab_trace_2d ( im , w , h , num_cc );

	/* update equivalence table for first slice */
        for ( i = 1 ; i <= *num_cc ; ++i )
        	tbl[i] = i;

        /* next label to assign is num_cc + 1 after
           labeling slice number one */
        lab = *num_cc + 1;

        /* copy input data to unsigned short buffer */
	_copy_to_uint16 ( im , buffer , w * h * z );

	for ( k = 1 ; k < z ; ++k )
	{
		for ( j = 1 ; j < h ; ++j )
		{
			for ( i = 1 ; i < w ; ++i )
                        {
                                /* current pixel */
                                idx = buffer + i + j * w + k * w * h;

                                /* current pixel is off */
                                if ( *idx == 0x0000 )
                                        continue;

                                /* current pixel is on */

                                /* check neighbours */
                                _label_3d ( idx , w , h , &lab , tbl );
                        }
                }
        }

        /* flatten table and get next label to assign.
           It minus one is the number of connected components */
        *num_cc = _tbl_flatten ( tbl , lab ) - 1;

        /* relabel using euivalence table */
        _tbl_relab ( buffer , w * h * z , tbl );

	/* copy data back to original image */
	_copy_from_uint16 ( buffer , im , w * h * z );

	/* clean up */
	mig_free ( buffer );
	free ( tbl );

	return 0;

error :

	if ( buffer != NULL )
		mig_free( buffer );

	if ( tbl != NULL )
		free ( tbl );

	return -1;
}


/****************************************************************************/
static void
_copy_to_uint16 ( unsigned char *src ,
                  unsigned short *dst ,
                  int s )
{
	int i;

	for ( i = 0 ; i < s ; ++i , ++src , ++dst )
		*dst = (unsigned short) *src;

}

/****************************************************************************/
static void
_copy_from_uint16 ( unsigned short *src ,
                    unsigned char *dst ,
                    int s )
{
	int i;

	for ( i = 0 ; i < s ; ++i , ++src , ++dst )
		*dst = (unsigned char) *src;
}

/****************************************************************************/
/* find root of tree from node ind */
static int
_tbl_find ( unsigned short *tbl ,
            int ind )
{
        int root = ind;
        while ( tbl[ root ] < root )
                root = tbl[ root ];
        return root;
}

/****************************************************************************/
/* set all nodes to point to a new root. */
static void
_tbl_set ( unsigned short *tbl ,
           int ind , int root )
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
_tbl_union_2 ( unsigned short *tbl ,
               int i , int j )
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
_tbl_union_3 ( unsigned short *tbl ,
               int i , int j , int k )
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
_tbl_flatten ( unsigned short *tbl ,
               int max )  /* max -> next non assigned label */
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
_tbl_relab ( unsigned short *buff ,
             int len ,
             unsigned short *tbl )
{
        int i;

        for ( i = 0 ; i < len ; ++i , ++buff )
		*buff = tbl[*buff];
}

/****************************************************************************/
/* check 3d neighbours of pixel idx. We use only three neighbours :
   1. above ( upper slice )
   2. above ( upper row )
   3. left.
*/
static void
_label_3d ( unsigned short *idx ,
            int w , int h ,
            int *lab ,
            unsigned short *tbl )
{
        unsigned short n[3];

        n[0] = *( idx - ( w * h ) );    /* upper slice */
        n[1] = *( idx - w );            /* upper row */
        n[2] = *( idx - 1 );            /* left */

        if ( n[0] )
        {
                if ( n[1] )
                {
                        if ( n[2] )
                        {
                                *idx = (unsigned short)
                                	_tbl_union_3 ( tbl ,
                                	               n[0] , n[1] , n[2] );
                                return;
                        }

                        *idx = (unsigned short)
                        	_tbl_union_2 ( tbl ,
                        	               n[0] , n[1] );
                        return;
                }

                if ( n[2] )
                {
                        *idx = (unsigned short)
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
                        *idx = (unsigned short)
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
        *idx = (unsigned short) *lab;
        tbl[*idx] = *idx;
        ++ (*lab);
}

/****************************************************************************/
#undef TBL_SIZE



