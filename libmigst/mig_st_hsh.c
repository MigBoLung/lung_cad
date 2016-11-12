#include "mig_st_hsh.h"
#include "libmigut.h"

#define idx_for( tablelen , hashval )   ( ( hashval )  % ( tablelen ) )
#define free_key( key ) free( key )

static const unsigned int primes[] =
{
        53, 97, 193, 389,
        769, 1543, 3079, 6151,
        12289, 24593, 49157, 98317,
        196613, 393241, 786433, 1572869,
        3145739, 6291469, 12582917, 25165843,
        50331653, 100663319, 201326611, 402653189,
        805306457, 1610612741
};

/* prime table length */
static const int ptbl_len = sizeof( primes ) / sizeof( unsigned int );
static const float max_ld_factor = 0.65f;

/*****************************************************************************/
unsigned int
mig_st_hsh_djb2 ( void *key )
{
        unsigned char *str = ( unsigned char* ) key;
        unsigned long hash = 5381;
        int c;

        while ( c = *str++ )
                hash = ( ( hash << 5 ) + hash ) + c;
        return hash;
}

/*****************************************************************************/
unsigned int
mig_st_hsh_sdbm ( void *key )
{
        unsigned char *str = ( unsigned char* ) key;
        unsigned long hash = 0;
        int c;

        while ( c = *str++ )
                hash = c + ( hash << 6 ) + ( hash << 16 ) - hash;
        return hash;
}

/*****************************************************************************/
#define MAX_KEY_LEN     20
int
mig_st_hsh_cmp ( void *k1, void *k2 )
{
        return ( 1 - strncmp( ( const char* ) k1 , ( const char* ) k2 , MAX_KEY_LEN ) );
}
#undef MAX_KEY_LEN

/*****************************************************************************/
mig_hsht_t*
mig_st_hsh_new ( int min , int ( *hfn ) ( void* ) , int ( *efn ) ( void* , void* ) )
{
        mig_hsht_t      *h;
        int             i , s = primes[0];

        /* Check wether requested hashtable isn't too large */
        if ( min > ( 1u << 30 ) )
                return NULL;

        /* Force size as prime */
        for ( i = 0 ; i < ptbl_len ; ++i )
        {
                if ( primes[i] > min )
                {
                        s = primes[i];
                        break;
                }
        }

        h = ( mig_hsht_t* ) malloc ( sizeof ( mig_hsht_t ) );
        if ( !h )
                return NULL;

        h->tbl = ( mig_hshe_t** )
                calloc ( s , sizeof ( mig_hshe_t* ) );
        if ( !( h->tbl ) )
        {
                free ( h );
                return NULL;
        }

        h->len          = s;
        h->pidx         = i;
        h->cnt          = 0;
        h->hfn          = hfn;
        h->efn          = efn;
        h->lim          = ( int ) ceil ( s * max_ld_factor );

        return h;
}

/*****************************************************************************/
static int
_mig_st_hsh_grow ( mig_hsht_t *h )
{
        /* increase size of the table to accomodate more entries */

        mig_hshe_t      **tbl;          /* new table */
        mig_hshe_t      *e;
        mig_hshe_t      **pe;
        int             s , i , idx;    /* new size , ... */

        /* hitting max capacity -> no more space left */
        if ( h->pidx == ( ptbl_len - 1 ) )
                return -1;

        s = primes[ ++( h->pidx )];     /* get next available size */
        tbl = ( mig_hshe_t** ) calloc ( s , sizeof( mig_hshe_t* ) );

        if ( tbl )      /* there is space for the new table */
        {
                /* This algorithm is not 'stable'. ie. it reverses the list
                 * when it transfers entries between the tables */
                for ( i = 0 ; i < h->len ; ++i )
                {
                        while ( (e = h->tbl[i]) != NULL )
                        {
                                h->tbl[i] = e->next;
                                idx = idx_for( s , e->h );
                                e->next = tbl[idx];
                                tbl[idx] = e;
                        }
                }

                free ( h->tbl );
                h->tbl = tbl;
        }
        else    /* no space for new copy - > realloc instead */
        {
                tbl = ( mig_hshe_t** )
                        realloc ( h->tbl , s * sizeof( mig_hshe_t* ) );
                if ( !tbl )
                {
                        --( h->pidx );
                        return -1;
                }

                h->tbl = tbl;
                memset ( tbl[ h->len ] , 0 , s - h->len );

                for ( i = 0 ; i < h->len ; ++i )
                {
                        for ( pe = &( tbl[i] ) , e = *pe ; e != NULL ; e = *pe )
                        {
                                idx = idx_for( s , e->h );
                                if ( idx == i )
                                {
                                        pe = &( e->next );
                                }
                                else
                                {
                                        *pe = e->next;
                                        e->next = tbl[idx];
                                        tbl[idx] = e;
                                }
                        }
                }
        }

        h->len = s;
        h->lim = ( int ) ceil ( s * max_ld_factor );

        return 0;
}

/*****************************************************************************/
int
mig_st_hsh_ins ( mig_hsht_t *h , void *k , void *v )
{
        /* This method allows duplicate keys - but they shouldn't be used */

        int             idx;
        mig_hshe_t      *e;

        if ( ++( h->cnt ) > h->lim )
                _mig_st_hsh_grow ( h );

        e = ( mig_hshe_t* ) malloc ( sizeof ( mig_hshe_t ) );
        if ( !e )
        {
                --( h->cnt );
                return -1;
        }

        e->h = h->hfn ( k );
        idx  = idx_for ( h->len , e->h );
        e->k = k;
        e->v = v;
        e->next = h->tbl[idx];
        h->tbl[idx] = e;

        return 0;
}

/*****************************************************************************/
/* returns value associated with key */
void*
mig_st_hsh_fnd ( mig_hsht_t *h , void *k )
{
        mig_hshe_t      *e;
        unsigned int    hash , idx;

        hash = h->hfn ( k );
        idx = idx_for ( h->len , hash );
        e = h->tbl[idx];

        while ( e )
        {
                /* Check hash value to short circuit heavier comparison */
                if ( ( hash == e->h ) && ( h->efn ( k , e->k ) ) )
                        return e->v;
                e = e->next;
        }

        return NULL;
}

/*****************************************************************************/
/* returns value associated with key */
void*
mig_st_hsh_rem ( mig_hsht_t *h , void *k )
{
        /* TODO:
          consider compacting the table when the load factor drops enough                or provide a 'compact' method.
        */

        mig_hshe_t      *e;
        mig_hshe_t      **pe;
        void            *v;
        unsigned int    hash, idx;

        hash = h->hfn ( k );
        idx  = idx_for ( h->len , hash );
        pe   = &( h->tbl[idx] );

        e = *pe;
        while ( e )
        {
                /* Check hash value to short circuit heavier comparison */
                if ( ( hash == e->h ) && ( h->efn ( k , e->k ) ) )
                {
                        *pe = e->next;
                        ( h->cnt ) --;
                        v = e->v;
                        free_key ( e->k );
                        free ( e );
                        return v;
                }

                pe = &( e->next );
                e = e->next;
        }

        return NULL;
}

/*****************************************************************************/
/* destroy */
void
mig_st_hsh_free ( mig_hsht_t *h , int free_vals )
{
        unsigned int    i;
        mig_hshe_t      *e , *f;
        mig_hshe_t      **tbl = h->tbl;

        for ( i = 0 ; i < h->len ; ++i )
        {
                e = tbl[i];
                while ( e )
                {
                        f = e;
                        e = e->next;
                        free_key ( f->k );
                        if ( free_vals )
                                free ( f->v );
                        free ( f );
                }
        }

        free ( h->tbl );
        free ( h );
}

/*****************************************************************************/
/* hashtable_change
 *
 * function to change the value associated with a key, where there already
 * exists a value bound to the key in the hashtable.
 *
 *
 */
void*
mig_st_hsh_mod ( mig_hsht_t *h , void *k , void *v )
{
        mig_hshe_t      *e;
        void            *oldv = NULL;
        unsigned int    hash , idx;

        hash = h->hfn( k );
        idx = idx_for ( h->len , hash );
        e = h->tbl[idx];

        while ( e )
        {
                /* Check hash value to short circuit heavier comparison */
                if ( ( hash == e->h ) && ( h->efn ( k , e->k ) ) )
                {
                        oldv = e->v;
                        //free ( e->v );
                        e->v = v;
                        return oldv;
                }
                e = e->next;
        }

        return NULL;
}

/*****************************************************************************/
void*
mig_st_hshi_key ( mig_hshi_t *i )
{
        return ( i->curr->k );
}

/*****************************************************************************/
void*
mig_st_hshi_val ( mig_hshi_t *i )
{
        return ( i->curr->v );
}

/*****************************************************************************/
mig_hshi_t*
mig_st_hshi_new ( mig_hsht_t *h )
{
        unsigned int    i, len;
        mig_hshi_t      *itr = NULL;

        itr = ( mig_hshi_t* )
                malloc( sizeof( mig_hshi_t ) );
        if ( !itr )
                return NULL;

        itr->h          = h;
        itr->curr       = NULL;
        itr->prev       = NULL;
        len             = h->len;
        itr->idx        = len;

        if ( h->cnt == 0 )
                return itr;

        /* find first entry in hashtable */
        for ( i = 0 ; i < len ; ++i )
        {
                if ( h->tbl[i] )
                {
                        itr->curr = h->tbl[i];
                        itr->idx = i;
                        break;
                }
        }

        return itr;
}

/*****************************************************************************/
int
mig_st_hshi_next ( mig_hshi_t *itr )
{
        unsigned int    j , len;
        mig_hshe_t      **tbl;
        mig_hshe_t      *next;

        if ( !( itr->curr ) )
                return -1;

        next = itr->curr->next;
        if ( next )
        {
                itr->prev = itr->curr;
                itr->curr = next;
                return 0;
        }

        len = itr->h->len;
        itr->prev = NULL;
        if ( len <= ( j = ++( itr->idx ) ) )
        {
                itr->curr = NULL;
                return -1;
        }

        tbl = itr->h->tbl;
        while ( !( ( next = tbl[j] ) == NULL ) )
        {
                if ( ++j >= len )
                {
                        itr->idx = len;
                        itr->curr = NULL;
                        return -1;
                }
        }

        itr->idx = j;
        itr->curr = next;
        return 0;
}

/*****************************************************************************/
/* remove - remove the entry at the current iterator position
 *          and advance the iterator, if there is a successive
 *          element.
 *          If you want the value, read it before you remove:
 *          beware memory leaks if you don't.
 *          Returns -1 if end of iteration. */

int
mig_st_hshi_rem ( mig_hshi_t *itr )
{
        mig_hshe_t *e, *p;
        int ret;

        /* Do the removal */
        if ( !( itr->prev ) )
        {
                /* element is head of a chain */
                itr->h->tbl[itr->idx] = itr->curr->next;
        }
        else
        {
                /* element is mid-chain */
                itr->prev->next = itr->curr->next;
        }

        /* itr->curr is now outside the hashtable */
        e = itr->curr;
        ( itr->h->cnt ) -- ;
        free_key ( e->k );

        /* Advance the iterator, correcting the parent */
        p = itr->prev;
        ret = mig_st_hshi_next ( itr );
        if ( itr->prev == e )
                itr->prev = p;

        free ( e );
        return ret;
}

/*****************************************************************************/
/* returns -1 if not found */
int
mig_st_hshi_fnd ( mig_hshi_t *itr , mig_hsht_t *h , void *k )
{
        mig_hshe_t      *e, *p = NULL;
        unsigned int    hash , idx;

        hash = h->hfn ( k );
        idx = idx_for ( h->len , hash );

        e = h->tbl[idx];
        while ( e )
        {
                /* Check hash value to short circuit heavier comparison */
                if ( ( hash == e->h ) && ( h->efn ( k , e->k ) ) )
                {
                        itr->idx = idx;
                        itr->curr = e;
                        itr->prev = p;
                        itr->h = h;
                        return 0;
                }
                p = e;
                e = e->next;
        }

        return -1;
}

