#ifndef __MIG_ST_HASH_H__
#define __MIG_ST_HASH_H__

#include "mig_config.h"
#include "mig_defs.h"

MIG_C_LINKAGE_START

/*****************************************************************************/
/* hashtable entry structure -> PRIVATE, do not use directly */
typedef struct _mig_hshe_t
{
	void                    *k, *v; /* key , value pair */
	unsigned int            h;      /* hash value */
	struct _mig_hshe_t      *next;  /* next entry in table */

}  mig_hshe_t;

/*****************************************************************************/
/* hashtable structure */
typedef struct
{
	int             len;  /* current table length -> global */
	mig_hshe_t      **tbl; /* current hashtable */
	int             cnt;  /* actual entries in table */
	int             lim;  /*  when to augment table */
	int             pidx; /* current prime index -> needed when resizing! */

	/* hash function associate with current table */
	int             (*hfn) ( void *k );

	/* key comparaison function associated with current table */
	int             (*efn) ( void *k1 , void *k2 );

}  mig_hsht_t;

/*****************************************************************************/
/* hashtable iterator */
typedef struct
{
        mig_hsht_t      *h;	/* hashtable associated with iterator */
        mig_hshe_t      *curr;	/* current hashtabel entry */
        mig_hshe_t      *prev;	/* previous hashtable entry */
        int             idx;	/* current entry index */

} mig_hshi_t;

/*****************************************************************************/
extern unsigned int
mig_st_hsh_djb2 ( void *key );

/*****************************************************************************/
extern unsigned int
mig_st_hsh_sdbm ( void *key );

/*****************************************************************************/
extern int
mig_st_hsh_cmp ( void *k1 , void *k2 );

/*****************************************************************************/
extern mig_hsht_t*
mig_st_hsh_new ( int min ,
                int ( *hfn ) ( void* ) ,
                int ( *efn ) ( void* , void* ) );

extern int
mig_st_hsh_ins ( mig_hsht_t *h , void *k , void *v );

extern int
mig_st_hsh_ins_int ( mig_hsht_t *h , void *k , int v );

extern int
mig_st_hsh_ins_float ( mig_hsht_t *h , void *k , float v );

extern int
mig_st_hsh_ins_double ( mig_hsht_t *h , void *k , double v );

extern int
mig_st_hsh_ins_string ( mig_hsht_t *h , void *k , char *v );

extern void*
mig_st_hsh_fnd ( mig_hsht_t *h , void *k );

extern int
mig_st_hsh_fnd_int ( mig_hsht_t *h , void *k );

extern float
mig_st_hsh_fnd_float ( mig_hsht_t *h , void *k );

extern double
mig_st_hsh_fnd_double ( mig_hsht_t *h , void *k );

extern char*
mig_st_hsh_fnd_string ( mig_hsht_t *h , void *k );

extern void*
mig_st_hsh_rem ( mig_hsht_t *h , void *k );

#define mig_st_hsh_cnt( h )     ( ( h )-> cnt )

extern void
mig_st_hsh_free ( mig_hsht_t *h , int free_vals );

extern void*
mig_st_hsh_mod ( mig_hsht_t *h , void *k , void *v );

/*****************************************************************************/
extern mig_hshi_t*
mig_st_hshi_new ( mig_hsht_t *h );

extern void*
mig_st_hshi_key ( mig_hshi_t *i );

extern void*
mig_st_hshi_val ( mig_hshi_t *i );

extern int
mig_st_hshi_next ( mig_hshi_t *itr );

extern int
mig_st_hshi_rem ( mig_hshi_t *itr );

extern int
mig_st_hshi_fnd ( mig_hshi_t *itr , mig_hsht_t *h , void *k );

MIG_C_LINKAGE_END


#endif /* __MIG_ST_HASH_H__ */


/**
        \file mig_st_hsh.h
        \brief Hashtable implementation

*/
