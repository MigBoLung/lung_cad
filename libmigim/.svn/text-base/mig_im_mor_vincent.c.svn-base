#include "mig_im_mor_vincent.h"

/*****************************/
#define PIXON           0xFF
#define PIXOFF          0x00
#define LAB             0x01
#define MARK            0x02
#define DIR_BUFF_LEN    256

/*****************************/
/* PRIVATE */
/*****************************/

/*
   8 connectivity directions code :
   each 2-ple contains { vertical , horizontal }
   increment.
*/

static int
_dir[CONN][2] =
{
	{ 0 ,  1 } , {  1 ,  1 } , {  1 , 0 } , {  1 , -1 } ,
	{ 0 , -1 } , { -1 , -1 } , { -1 , 0 } , { -1 ,  1 }
};

static int
_encode_image ( unsigned char *im ,
                int w , int h ,
                loop_list_t *list );

static void
_dump_loop_list ( loop_list_t *list );

static void
_free_loop_list ( loop_list_t *list );

static void
_minkowski_add ( loop_list_t *loops , sel_t *sel );

static void
_invert_image ( unsigned char *im , int w , int h );

static void
_tracer ( unsigned char *im ,
          int w ,
          int *cx , int *cy ,
          int *dir );

static void
_trace_contour ( unsigned char *im ,
                 int w ,
                 int cx , int cy ,
                 int dir , loop_t *loop );

static void
_dump_loop ( loop_t *loop );

static void
_dump_pix ( pix_t *pix );

/*****************************/
/* EXPORTS */
/*****************************/

int
encode_sel ( unsigned char *im ,
             int w , int h ,
             sel_t *out )
{
	int i , j , k , cx , cy;
        unsigned char *idx , *tmp;

        assert ( im );
        assert ( out );
	assert ( w > 0 && h > 0 );

        cx = out->o.x;
        cy = out->o.y;

        /* reset structuring element */
        memset ( out , 0x00 , sizeof( sel_t ) );

        out->o.x = cx;
        out->o.y = cy;

        /* allocate memory for pixel list */
        out->pix = (pix_t*)
                calloc ( w * h , sizeof(pix_t) );
        if ( out->pix == NULL )
                goto error;

        for ( i = 0 ; i < CONN ; ++ i )
        {
                out->dir[i] = (pix_t*)
                        calloc ( w * h , sizeof(pix_t));
                if ( out->dir[i] == NULL )
                        goto error;
        }

        /* rows */
        for ( j = 1 ; j < ( h - 1 ) ; ++ j )
        {
        	/* columns */
                for ( i = 1 ; i < ( w - 1 ) ; ++i )
                {
			idx = im + i + j * w;

                        if ( *idx == PIXOFF )
                                continue;

                        /* we found a PIXON */
                        out->pix[out->num_pix].x = i - cx;
                        out->pix[out->num_pix].y = j - cy;
                        out->pix[out->num_pix].ptr = NULL;
                        ( out->num_pix ) ++;

                        for ( k = 0 ; k < CONN ; ++k )
                        {
                                tmp = idx + _dir[k][1] +
                                        w * _dir[k][0];

                                if ( *tmp == PIXOFF )
                                {
                                        out->dir[k][out->num_dir[k]].x = i - cx;
                                        out->dir[k][out->num_dir[k]].y = j - cy;
                                        out->dir[k][out->num_dir[k]].ptr = NULL;
                                        ( out->num_dir[k] ) ++;
                                }
                        }
                }
        }

	return 0;

error :

        if ( out->pix )
                free ( out->pix );

        for ( i = 0 ; i < CONN ; ++ i )
        {
                if ( out->dir[i] )
                        free ( out->dir[i] );
        }

        memset ( out , 0x00 , sizeof( sel_t ) );
        return -1;
}

/*****************************/

int
encode_disk ( int r , sel_t *out )
{
        unsigned char *dsk;
        int s , d;
        int i , j , rc;

        assert ( r > 0 );
        assert ( out );

        /* disk diameter */
        d = 2 * r + 1;

        /* dimension of new image :
           same for width and height */
        s = d + 2;

        dsk = (unsigned char*)
                calloc ( s * s , sizeof(unsigned char) );
        if ( dsk == NULL  )
                return -1;

        dsk += ( r + 1 + s * ( r + 1 ) );

        for ( j = -r ; j <= r ; ++j )
        {
                for ( i = -r ; i <= r ; ++i )
                {
                        *( dsk + i + j * s ) =
                                ( sqrt( (float) ( i*i + j*j ) ) > r ) ? PIXOFF : PIXON;
                }
        }

        dsk -= ( r + 1 + s * ( r + 1 ) );

        out->o.x = r + 1;
        out->o.y = r + 1;
        out->o.ptr = NULL;

        rc = encode_sel ( dsk , s , s , out );
        free ( dsk );

        return rc;
}

/*****************************/

void
free_sel ( sel_t *sel )
{
        int i;

        assert ( sel );

        free ( sel->pix );
        for ( i = 0 ; i < CONN ; ++ i )
                free ( sel->dir[i] );

        memset ( sel , 0x00 , sizeof( sel_t ) );
}

/*****************************/

void
dump_sel ( sel_t *sel )
{
        int i , j;

        assert ( sel );
        assert ( sel->pix );

        fprintf ( stdout , "\n" );
        fprintf ( stdout , "\nSEL:" );
        fprintf ( stdout , "\n\tpixels : %d" , sel->num_pix );

        fprintf ( stdout , "\npixels :\n" );
        for ( i = 0 ; i < sel->num_pix ; ++ i )
        {
                _dump_pix ( &( sel->pix[i] ) );
        }

        fprintf ( stdout , "\neach direction :" );
        for ( i = 0 ; i < CONN ; ++ i )
        {
                fprintf ( stdout , "\n%d %d" , i , sel->num_dir[i] );
                for ( j = 0 ; j < sel->num_dir[i] ; ++j )
                        _dump_pix ( &( sel->dir[i][j] ) );

        }
}

/*****************************/
int
mdilate ( unsigned char *im ,
          int w , int h , sel_t *sel )
{
        int rc;
        loop_list_t loops;

        assert ( im );
        assert ( w > 0 && h > 0 );
        assert ( sel );

        rc = _encode_image ( im , w , h , &loops );
        if ( rc != 0 )
                return rc;

        _minkowski_add ( &loops , sel );
        _free_loop_list ( &loops );

        return 0;
}

/*****************************/
int
merode ( unsigned char *im ,
         int w , int h , sel_t *sel )
{
        int rc;
        loop_list_t loops;

        assert ( im );
        assert ( w > 0 && h > 0 );
        assert ( sel );

        /* invert image */
        _invert_image ( im , w , h );

        rc = _encode_image ( im , w , h , &loops );
        if ( rc != 0 )
                return rc;

        _minkowski_add ( &loops , sel );

        /* invert image */
        _invert_image ( im , w , h );

        _free_loop_list ( &loops );

        return 0;
}


/*****************************/
int
mopen ( unsigned char *im ,
        int w , int h , sel_t *sel )
{
        int rc;

        assert ( im );
        assert ( w > 0 && h > 0 );
        assert ( sel );

        rc = merode ( im , w , h , sel );
        if ( rc != 0 )
                return rc;
        return mdilate ( im , w , h , sel );
}

/*****************************/
int
mclose ( unsigned char *im ,
         int w , int h , sel_t *sel )
{
        int rc;

        assert ( im );
        assert ( w > 0 && h > 0 );
        assert ( sel );

        rc = mdilate ( im , w , h , sel );
        if ( rc != 0 )
                return rc;
        return merode ( im , w , h , sel );
}

/*****************************/
static void
_minkowski_add ( loop_list_t *loops , sel_t *sel )
{
        int i , j , d;
        loop_t *loop;
        unsigned char *curr;

        assert ( loops );
        assert ( sel );

        /* for each loop */
        loop = loops->head;
        while ( loop )
        {
                curr = loop->p0.ptr;

                /* initilize position of structuring
                   element on current loop */
                for ( i = 0 ; i < sel->num_pix ; ++ i )
                {
                        *( curr +
                           sel->pix[i].x +
                           sel->pix[i].y *
                           loops->w ) = PIXON;
                }

                for ( i = 0 ; i < ( loop->len - 1 ) ; ++i )
                {
                        /* propagation of structuring
                           element along the loop */
                        d = loop->dir[i];
                        curr += _dir[d][1] + _dir[d][0] * loops->w;

                        for ( j = 0 ; j < sel->num_dir[d] ; ++ j )
                        {
                                *( curr +
                                   sel->dir[d][j].x +
                                   sel->dir[d][j].y *
                                   loops->w ) = PIXON;
                        }
                }

                loop = loop->next;
        }
}

/*****************************/
/* PRIVATE */
/*****************************/

int
_encode_image ( unsigned char *src ,
               int w , int h ,
               loop_list_t *list )
{
        int i , j;
	unsigned char *idx;
        loop_t *tmp = NULL , *loop = NULL;

	assert ( src );
	assert ( w > 0 && h > 0 );
        assert ( list );

        /* zero out loop list */
        memset ( list , 0x00 , sizeof( loop_list_t ) );

        /* store image width */
        list->w = w;

	/* rows */
        for ( j = 1 ; j < ( h - 1 ) ; ++ j )
        {
        	/* columns */
                for ( i = 1 ; i < ( w - 1 ) ; ++i )
                {
			idx = src + i + j * w;

                        if ( *idx == PIXOFF ||
                             *idx == MARK  )
                                continue;

                        if ( *idx == PIXON )
                        {
                                /* external contour */
                                if ( ( *( idx - 1 ) == PIXOFF ) ||
                                     ( *( idx - 1 ) == MARK ) )
                                {
                                        loop = (loop_t*)
                                                calloc ( 1 , sizeof( loop_t ) );
                                        if ( loop == NULL )
                                                return -1;

                                        _trace_contour ( src , w ,
                                                         i , j , 0 , loop );

                                        /* add loop to list */
                                        tmp = list->head;
                                        list->head = loop;
                                        list->head->next = tmp;
                                        list->len ++;
                                }
                        }

                        /* internal contour */
                        if ( *( idx + 1 ) == PIXOFF )
                        {
                                loop = (loop_t*)
                                        calloc ( 1 , sizeof( loop_t ) );
                                if ( loop == NULL )
                                        return -1;

                                _trace_contour ( src , w ,
                                                 i , j , 1 , loop );

                                /* add loop to list */
                                tmp = list->head;
                                list->head = loop;
                                list->head->next = tmp;
                                list->len ++;
                        }
                }
        }

        /* second pass -> reset MARK */
        for ( j = 0 ; j < h ; ++ j )
                for ( i = 0 ; i < w  ; ++ i , ++src )
                {
                        if ( *src == PIXON )
                                continue;

                        switch ( *src )
                        {
                                case LAB :
                                        *src = PIXON;
                                        break;

                                case MARK :
                                        *src = PIXOFF;
                                        break;

                                default :
                                        *src = PIXOFF;
                                        break;
                        }
                }

	return 0;
}

/*****************************/

static void
_invert_image ( unsigned char *im , int w , int h )
{
        int i;

        assert ( im );
        assert ( w > 0 && h > 0 );

        for ( i = 0 ; i < w * h ; ++i , ++im )
                *im ^= PIXON;
}

/*****************************/

static void
_dump_loop_list ( loop_list_t *list )
{
        loop_t *curr;

        assert ( list );
        assert ( list->head );

        fprintf ( stdout , "\n" );
        fprintf ( stdout , "\nLOOP LIST:" );
        fprintf ( stdout , "\n\tlenght : %d" , list->len );

        curr = list->head;

        while ( curr != NULL )
        {
                _dump_loop ( curr );
                curr = curr->next;
        }

        fprintf ( stdout , "\n" );
}

/*****************************/

static void
_free_loop_list ( loop_list_t *list )
{
        loop_t *tmp;

        assert ( list );

        while ( list->head )
        {
                tmp = list->head;
                list->head = tmp->next;

                free( tmp->dir );
                free ( tmp );
        }

        memset ( list , 0x00 , sizeof( loop_list_t ) );
}

/*****************************/

static void
_trace_contour ( unsigned char *im ,
                 int w ,
                 int cx , int cy ,
                 int dir ,
                 loop_t *loop )
{
	char stop = 0 , again = 1;
        int fx , fy , sx = cx , sy = cy;

        *im = LAB;

        loop->p0.x = cx;
        loop->p0.y = cy;
        loop->p0.ptr = im + cx + cy * w;
        loop->len  = 0;
        loop->dir  = (unsigned char*)
                calloc ( DIR_BUFF_LEN , sizeof(unsigned char) );
        if ( loop->dir == NULL )
                return;
        loop->dir_len = DIR_BUFF_LEN;

        _tracer ( im , w ,
                  &cx , &cy , &dir );

        if ( cx != sx || cy != sy )
        {
		fx = cx;
		fy = cy;

                while ( again )
		{
                        if ( loop->len >= loop->dir_len )
                        {
                                loop->dir_len <<= 1;
                                loop->dir = (unsigned char*)
                                        realloc ( loop->dir , ( loop->dir_len ) *
                                                sizeof(unsigned char) );
                                if ( loop->dir == NULL )
                                        return;
                        }

                        loop->dir[loop->len] = (unsigned char) dir;
                        loop->len ++;

			dir = ( dir + 6 ) % 8;
                        *( im + cx + cy * w ) = LAB;

                        _tracer ( im , w ,
                                  &cx , &cy , &dir );

                        if ( cx == sx && cy == sy )
                                stop = 1;
                        else if ( stop )
                        {
                                if ( cx == fx && cy == fy )
                                        again = 0;
                                else
                                        stop = 0;
                        }
                }
        }
}

/*****************************/

static void
_tracer ( unsigned char *im ,
          int w ,
          int *cx , int *cy ,
          int *dir )
{
	int i, y, x;
	unsigned char *pix;

        for ( i = 0 ; i < CONN - 1 ; ++ i )
        {
                y = *cy + _dir[*dir][0];
                x = *cx + _dir[*dir][1];

                pix = im + x + y * w;

                if ( ( *pix == PIXOFF ) ||
                     ( *pix == MARK ) )
                {
                        *pix = MARK;
                        *dir = (*dir + 1) % CONN;
                }
                else
                {
                        *cy = y;
                        *cx = x;
                        break;
                }
        }
}

/*****************************/

static void
_dump_loop ( loop_t *loop )
{
        int i;

        assert ( loop );

        fprintf ( stdout , "\nLOOP:" );
        fprintf ( stdout , "\n\tlenght : %d" , loop->len );
        fprintf ( stdout , "\n\tp0 : %d %d %p" ,
                loop->p0.x , loop->p0.y , loop->p0.ptr );
	fprintf ( stdout , "\n\tdir lenght : %d" , loop->dir_len );

        assert( loop->dir );
        fprintf ( stdout , "\n\tdir list : \n" );
        for ( i = 0 ; i < loop->len ; ++ i )
                        fprintf ( stdout , "(%x)" , loop->dir[i] );

	fprintf ( stdout , "\n\tnext : %p" , loop->next );
        fprintf ( stdout , "\n" );
	fflush ( stdout );
}

/*****************************/

static void
_dump_pix ( pix_t *pix )
{
        assert( pix );

        fprintf ( stdout , "(%d,%d,%p)" ,
                pix->x , pix->y , pix->ptr );
        fflush ( stdout );
}


#undef PIXON
#undef PIXOFF
#undef LAB
#undef MARK
#undef DIR_BUFF_LEN


