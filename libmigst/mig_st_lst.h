#ifndef __MIG_ST_LST_H__
#define __MIG_ST_LST_H__

#include "mig_config.h"
#include "mig_defs.h"

MIG_C_LINKAGE_START

/****************************************************************************/

/* list node */
typedef struct _mig_lst_node mig_lst_node;
struct _mig_lst_node
{
    /* pointer to node data */
    void            *data;
    mig_lst_node    *next;
    mig_lst_node    *prev;
};

/****************************************************************************/
typedef int  (*mig_lst_cmp_f)      ( const void  *a , const void *b );
typedef int  (*mig_lst_sel_f)      ( const void  *a );
typedef void (*mig_lst_data_f)     ( void *data , void *user_data );
typedef void (*mig_lst_data_new_f) ( void **data , void *user_data );
typedef void (*mig_lst_free_f)     ( void *data );

/****************************************************************************/
typedef struct _mig_lst_t mig_lst_t;
struct _mig_lst_t
{
    /* start node */
    mig_lst_node        *head;
    
    /* end node */
    mig_lst_node        *tail;
    
    /* number of nodes in list */
    int                 num;
    
    /* node free function : needed because windows does not allow us
       to free list nodes across different dlls.
       Note : this is not the node->data free function !
    */
    mig_lst_free_f      _free;
};


/****************************************************************************/
mig_lst_t*
mig_lst_alloc ();

void
mig_lst_static ( mig_lst_t list );

void
mig_lst_zero ( mig_lst_t *list );

void
mig_lst_free ( mig_lst_t *list );

void
mig_lst_free_custom ( mig_lst_t *list , mig_lst_free_f f );

void
mig_lst_empty ( mig_lst_t *list );

void
mig_lst_free_custom_static ( mig_lst_t *list , mig_lst_free_f f );

int
mig_lst_ins ( mig_lst_t *list , void *data , int pos );

int
mig_lst_ins_sort ( mig_lst_t *list , void *data , mig_lst_cmp_f f );

void*
mig_lst_find ( mig_lst_t *list , void *data , mig_lst_cmp_f f );

void*
mig_lst_peek ( mig_lst_t *list , int pos );

void*
mig_lst_get_head ( mig_lst_t *list );

void*
mig_lst_peek_head ( mig_lst_t *list );

void*
mig_lst_get_tail ( mig_lst_t *list );

void*
mig_lst_peek_tail ( mig_lst_t *list );

int
mig_lst_put_head ( mig_lst_t *list , void *data );

int
mig_lst_put_tail ( mig_lst_t *list , void *data );

void*
mig_lst_rem_pos ( mig_lst_t *list , int pos );

void*
mig_lst_rem ( mig_lst_t *list , void *data , mig_lst_cmp_f f );

void
mig_lst_rem_all ( mig_lst_t *list , mig_lst_sel_f fsel , mig_lst_free_f ffree );

void
mig_lst_cat ( mig_lst_t *src , mig_lst_t *dst );

void
mig_lst_sort ( mig_lst_t *list , mig_lst_cmp_f f );

void
mig_lst_foreach ( mig_lst_t *list , mig_lst_data_f f , void *user_data );

int
mig_lst_save ( const char *fname , mig_lst_t *lst , mig_lst_data_f data_save_f );

int
mig_lst_load ( const char *fname , mig_lst_t *lst , mig_lst_data_new_f data_load_f );

#define mig_lst_len(list)   ((list)->num)

/*****************************************************************************/
typedef struct _mig_lst_iter mig_lst_iter;
struct _mig_lst_iter
{
        mig_lst_t         *list;
        mig_lst_node    *curr;
};

/*****************************************************************************/
 mig_lst_iter*
mig_lst_iter_alloc (void);

void
mig_lst_iter_get ( mig_lst_iter *iterator , mig_lst_t *list );

void
mig_lst_iter_free ( mig_lst_iter *iterator );

void*
mig_lst_iter_next ( mig_lst_iter *iterator );

void*
mig_lst_iter_prev ( mig_lst_iter *iterator );

void*
mig_lst_iter_first ( mig_lst_iter *iterator );

void*
mig_lst_iter_last ( mig_lst_iter *iterator );

void
mig_lst_iter_reset ( mig_lst_iter *iterator );



void
mig_lst_free_custom_static_data_and_node ( mig_lst_t *list , mig_lst_free_f f, mig_lst_free_f free_node );

MIG_C_LINKAGE_END

#endif /* __MIG_ST_LST_H__ */
