#include "mig_st_lst.h"
#include "mig_error_codes.h"

/****************************************************************************/
mig_lst_t*
mig_lst_alloc ()
{
        mig_lst_t *list;

        list = (mig_lst_t*)
        	calloc ( 1 , sizeof(mig_lst_t));
        if (!list)
                return NULL;

        list->_free = free;
        return list;
}

/****************************************************************************/
void
mig_lst_zero ( mig_lst_t *list )
{
    assert ( list );

    list->head = list->tail = NULL;
    list->num = 0;
    list->_free = free;
}

/****************************************************************************/
void
mig_lst_free ( mig_lst_t *list )
{
        mig_lst_node *tmp;
        mig_lst_free_f freef;

        assert ( list );
        
        if ( list->_free == NULL )
            freef = free;
        else
            freef = list->_free;

        while ( ( tmp = list->head ) != NULL )
        {
                freef ( tmp->data );
                list->head = tmp->next;
                freef ( tmp );
        }

        freef ( list );
}

/****************************************************************************/
extern void
mig_lst_empty ( mig_lst_t *list )
{
        mig_lst_node *tmp;
        mig_lst_free_f freef;

        assert ( list );

        if ( list->_free == NULL )
            freef = free;
        else
			freef = list->_free;

        while ( ( tmp = list->head ) != NULL  )
        {
                freef ( tmp->data );
                list->head = tmp->next;
                freef ( tmp );
        }

        memset ( list , 0x00 , sizeof( mig_lst_t ) );
}


/****************************************************************************/
void
mig_lst_static ( mig_lst_t list )
{
        mig_lst_node *tmp;
        mig_lst_free_f freef;

        if ( list._free == NULL )
            freef = free;
        else
            freef = list._free;

        while ( ( tmp = list.head ) != NULL )
        {
                freef ( tmp->data );
                list.head = tmp->next;
                freef ( tmp );
        }

        list.head = list.tail = NULL;
        list.num = 0;
}

/****************************************************************************/
 void
mig_lst_free_custom ( mig_lst_t *list , mig_lst_free_f f )
{
        mig_lst_node *tmp;
        mig_lst_free_f freef;

        assert ( list && f );

        if ( list->_free == NULL )
            freef = free;
        else
            freef = list->_free;

        while ( ( tmp = list->head ) != NULL )
        {
                f ( tmp->data );
                list->head = tmp->next;
                freef ( tmp );
        }

        freef ( list );
}

/****************************************************************************/
void
mig_lst_free_custom_static ( mig_lst_t *list , mig_lst_free_f f )
{
    mig_lst_node *tmp;
	mig_lst_free_f tmp_f;
    mig_lst_free_f freef;

    assert ( list );

    if ( list->_free == NULL )
        freef = free;
    else
        freef = list->_free;

	
    if ( f == NULL ) 
	{
		tmp_f = free;
	}
	else
	{
		tmp_f = f;		
	}

    while ( ( tmp = list->head ) != NULL )
    {
            tmp_f ( tmp->data );
            list->head = tmp->next;
            freef ( tmp );
    }

    list->head = list->tail = NULL;
    list->num = 0;
}

/****************************************************************************/
int
mig_lst_ins ( mig_lst_t *list , void *data , int pos )
{
    int cnt = 0;
    mig_lst_node *ind , *tmp;

    assert ( list && data );

    tmp = (mig_lst_node*) malloc ( sizeof( mig_lst_node ) );
    if ( tmp == NULL )
            return MIG_ERROR_MEMORY;

    tmp->data = data;

    /* spacial case at head of list */
    if ( pos <= 0 )
    {
        tmp->next = list->head;
        tmp->prev = NULL;

        if ( list->head )
                list->head->prev = tmp;

        list->head = tmp;
    }

    /* sapcial case tail of list */
    if ( pos >= list->num )
    {
        tmp->next = NULL;
        tmp->prev = list->tail;

        if (list->tail)
                list->tail->next = tmp;

        list->tail = tmp;
    }

    if ( ( pos > 0 ) && ( pos < list->num ) )
    {
        ind = list->head;
        while (cnt++ < pos)
                ind = ind->next;

        tmp->next = ind;
        tmp->prev = ind->prev;
        ind->prev->next = tmp;
        ind->prev = tmp;
    }

    (list->num)++;
    return MIG_OK;
}

/****************************************************************************/
int
mig_lst_ins_sort (mig_lst_t *list, void *data, mig_lst_cmp_f f)
{
    mig_lst_node *ind, *tmp;
    int cmp;

    assert( list && data && f );

    tmp = (mig_lst_node*) malloc (sizeof(mig_lst_node));
    if ( tmp == NULL )
            return MIG_ERROR_MEMORY;

    tmp->data = data;
    tmp->prev = tmp->next = NULL;

    if (!(list->head))
    {
            list->head = list->tail = tmp;
            list->num++;
            return MIG_OK;
    }

    ind = list->head;
    cmp = (*f) (data, ind->data);
    while ((ind->next) && (cmp > 0))
    {
            ind = ind->next;
            cmp = (*f) (data, ind->data);
    }

    if ((!ind->next) && (cmp > 0))
    {
            ind->next = tmp;
            tmp->prev = ind;

            list->tail = tmp;
            list->num++;

            return MIG_OK;
    }

    if (ind->prev)
    {
            ind->prev->next = tmp;
            tmp->prev = ind->prev;
    }
    else
    {
            list->head = tmp;
    }

    tmp->next = ind;
    ind->prev = tmp;

    list->num++;

    return MIG_OK;
}

/****************************************************************************/
void*
mig_lst_find (mig_lst_t *list, void *data, mig_lst_cmp_f f)
{
    mig_lst_node *ind;
    int cmp;

    assert(list && data && f);

    ind = list->head;
    cmp = (*f) (ind->data, data);
    while ((ind) && (cmp!= 0))
    {
            ind = ind->next;
            cmp = (*f) (ind->data, data);
    }

    if (!ind)
            return NULL;

    return (ind->data);
}

/****************************************************************************/
void*
mig_lst_peek ( mig_lst_t *list , int pos )
{
    mig_lst_node *ind;
    int cnt = 0;

    assert( list && ( pos >=0 ) && ( pos < list->num ) );

    ind = list->head;
    while ( cnt++ < pos )
            ind = ind->next;

    if ( !ind )
            return NULL;

    return ( ind->data );
}

/****************************************************************************/
int
mig_lst_put_head (mig_lst_t *list, void *data)
{
    mig_lst_node *tmp;

    assert(list && data);

    tmp = (mig_lst_node*) malloc (sizeof(mig_lst_node));
    if (!tmp)
            return MIG_ERROR_MEMORY;

    tmp->data = data;
    tmp->prev = tmp->next = NULL;

    /* no head == no tail */
    if (list->head == NULL)
    {
            list->head = list->tail = tmp;
            list->num++;
            return MIG_OK;
    }

    tmp->next = list->head;
    list->head->prev = tmp;
    list->head = tmp;

    (list->num)++;

    return MIG_OK;
}

/****************************************************************************/
void*
mig_lst_get_head (mig_lst_t *list)
{
    mig_lst_node *tmp;
    void *data;

    assert(list);

    tmp = list->head;

    if ( tmp == NULL )
        return NULL;

    list->head = tmp->next;

    if ( tmp->next == NULL )
    {
            list->tail = NULL;
    }
    else
    {
            tmp->next->prev = NULL;
    }

    data = tmp->data;
    list->_free ( tmp );
    (list->num)--;
    return (data);
}

/****************************************************************************/
 void*
mig_lst_peek_head (mig_lst_t *list)
{
    assert(list);

    if (list->head == NULL)
            return NULL;

    return (list->head->data);
}

/****************************************************************************/
int
mig_lst_put_tail ( mig_lst_t *list , void *data )
{
    mig_lst_node *tmp;

    assert(list && data);

    tmp = (mig_lst_node*) calloc ( 1 , sizeof( mig_lst_node ) );
    if ( !tmp )
            return MIG_ERROR_MEMORY;

    tmp->data = data;

    if (!list->tail)
    {
            list->head = tmp;
    }
    else
    {
            tmp->prev = list->tail;
            list->tail->next = tmp;
    }

    list->tail = tmp;
    (list->num)++;

    return MIG_OK;
}

/****************************************************************************/
void*
mig_lst_get_tail (mig_lst_t *list)
{
    mig_lst_node *tmp;
    void *data;

    assert(list);

    tmp = list->tail;

    if (!tmp)
            return NULL;

    list->tail = tmp->prev;

    if (!tmp->prev)
    {
            list->head = NULL;
    }
    else
    {
            tmp->prev->next = NULL;
    }

    data = tmp->data;
    list->_free (tmp);
    (list->num)--;
    return (data);
}

/****************************************************************************/
 void*
mig_lst_peek_tail (mig_lst_t *list)
{
    assert(list);

    if (list->tail == NULL)
            return NULL;

    return (list->tail->data);
}

/****************************************************************************/
void*
mig_lst_rem_pos (mig_lst_t *list, int pos)
{
    mig_lst_node *ind;
    void *data;
    int cnt = 0;

    assert(list);

    if ((pos < 0) ||
        (pos >= list->num) ||
        (list->head == NULL))
            return NULL;

    ind = list->head;
    while (cnt++ < pos)
            ind = ind->next;

    data = ind->data;
    if (ind->prev)
    {
            ind->prev->next = ind->next;
    }
    else
    {
            list->head = ind->next;
    }

    if (ind->next)
    {
            ind->next->prev = ind->prev;
    }
    else
    {
            list->tail = ind->prev;
    }

    list->_free (ind);
    (list->num)--;
    return (data);
}

/****************************************************************************/
void*
mig_lst_rem (mig_lst_t *list, void *data, mig_lst_cmp_f f)
{
    mig_lst_node *ind;
    int cnt = 0, cmp;

    assert(list && f);

    if ( list->head == NULL )
        return NULL;

    ind = list->head;
    cmp = (*f) (data, ind->data);
    while ((ind->next) && (cmp != 0))
    {
            ind = ind->next;
            cmp = (*f) (data, ind->data);
            cnt++;
    }

    if (!(ind->next) && cmp )
            return NULL;

    return ( mig_lst_rem_pos (list, cnt));
}

/****************************************************************************/
void
mig_lst_rem_all ( mig_lst_t *list , mig_lst_sel_f fsel , mig_lst_free_f ffree )
{
    int pos = 0;
    void *data = NULL;

    assert( list && fsel && ffree );

    if ( list->head == NULL )
            return;

    while ( pos < list->num )
    {
        data = mig_lst_peek ( list , pos );
        if ( fsel ( data ) == 0 )
        {
            data = mig_lst_rem_pos ( list , pos );
            
            printf("calling free on element at pos %d\n" , pos);
            ffree ( data );
            -- pos;
            printf("lst->num: %d\n" , list->num);
        }
        ++pos;
     
    }
}

/****************************************************************************/
void
mig_lst_cat ( mig_lst_t *src , mig_lst_t *dst )
{
    if ( src->num == 0 )
        return;

    if ( dst->head == NULL )
    {
        dst->head = src->head;
        dst->tail = src->tail;
        dst->num  = src->num;
        
        /* AAAA -> not so correct if we mix list obtained from
           different dlls */
        dst->_free = src->_free;

    }
	else{

		dst->tail->next = src->head;
		dst->tail = src->tail;
		dst->num += src->num;

		/* AAAA -> not so correct if we mix list obtained from
			different dlls */
		dst->_free = src->_free;
	}

    memset ( src , 0x00 , sizeof( mig_lst_t ) ); 
}


/****************************************************************************/
void
mig_lst_sort ( mig_lst_t *list , mig_lst_cmp_f f )
{
        mig_lst_node **buff = NULL;     /* temporary buffer */
        mig_lst_node *tmp = list->head; /* list head */
        int i = 0;

        if ( tmp == NULL ) /* if list is empty nothing to do! */
                return;

        buff = (mig_lst_node**)
                calloc ( list->num , sizeof( mig_lst_node*) );
        if ( buff == NULL )
                return;

        /* copy all list node addresses to temporary buffer */
        do
        {
                buff[i++] = tmp;
                tmp = tmp->next;
        }
        while ( tmp != NULL );

        /* sort temporary buffer */
		qsort ( buff ,
                list->num ,
                sizeof(mig_lst_node*) ,
                f );
		

        /* copy data back to original list */
        list->head = buff[0];
        list->head->prev = NULL;

        list->tail = buff[list->num-1];
        list->tail->next = NULL;

        tmp = list->head;
        for ( i = 0 ;
              i < list->num - 1 ;
              ++i , tmp = tmp->next )
        {
                tmp->next = buff[i+1];
                tmp->next->prev = tmp;
        }


        free ( buff );
}

/****************************************************************************/
void
mig_lst_foreach ( mig_lst_t *list , 
                  mig_lst_data_f f , 
                  void *user_data )
{
        mig_lst_node *ind;

        ind = list->head;
        while ( ind != NULL )
        {
                (*f) ( ind->data , user_data );
                ind = ind->next;
        }
}

/****************************************************************************/
int
mig_lst_save ( const char *fname , mig_lst_t *lst , mig_lst_data_f data_save_f )
{
    FILE *f = NULL;
    mig_lst_node *node;
    
    /* open file for writing in text mode */
    f = fopen ( fname , "w" );
    if ( f == NULL )
        return -1;

    /* write number of nodes in list */
    fprintf ( f , "%d\n" , lst->num );
    
    /* cycle through nodes */
    node = lst->head;
    while ( node )
    {
        /* apply node saving function to each node */
        (*data_save_f) ( node->data , f );
        node = node->next;
    }

    fflush ( f );
    fclose ( f ); 
    return 0;
}

/****************************************************************************/
int
mig_lst_load ( const char *fname , mig_lst_t *lst , mig_lst_data_new_f data_load_f )
{
    FILE *f = NULL;
    void *data;
    int i , num;
    
    /* open file for reading in text mode */
    f = fopen ( fname , "r" );
    if ( f == NULL )
        goto error;
    
    /* read number of nodes in list */
    fscanf ( f , "%d\n" , &num );

    /* for each node */
    for ( i = 0 ; i < num ; ++i )
    {
        /* get node data from file */
        data_load_f ( &data , f );
        
        /* put node data in list's tail */
        if ( mig_lst_put_tail ( lst , data ) )
            goto error;
    }
    
    fclose ( f ); 
    return 0;

error :

    if ( f != NULL );
        fclose ( f );

    return -1;
}

/****************************************************************************/
mig_lst_iter*
mig_lst_iter_alloc (void)
{
        mig_lst_iter *iter = (mig_lst_iter*)
                malloc (sizeof(mig_lst_iter));
        if (!iter)
                return NULL;

        iter->list = NULL;
        iter->curr = NULL;

        return iter;
}

/****************************************************************************/
 void
mig_lst_iter_get (mig_lst_iter *iter, mig_lst_t *list)
{
        assert(iter && list);

        iter->list = list;
        iter->curr = list->head;
}

/****************************************************************************/
void
mig_lst_iter_free (mig_lst_iter *iter)
{
        assert(iter);
        free (iter);
}

/****************************************************************************/
void*
mig_lst_iter_next ( mig_lst_iter *iter )
{
        void *data;

        assert(iter);

        if (iter->curr)
        {
                data = iter->curr->data;
                iter->curr = iter->curr->next;
                return data;
        }

        return NULL;
}

/****************************************************************************/
 void*
mig_lst_iter_prev (mig_lst_iter *iter)
{
        void *data;

        assert(iter);

        if (iter->curr)
        {
                data = iter->curr->data;
                iter->curr = iter->curr->prev;
                return data;
        }

        return NULL;
}

/****************************************************************************/
 void*
mig_lst_iter_first (mig_lst_iter *iter)
{
        assert(iter);

        if (iter->list->head)
                return (iter->list->head->data);
        else
                return NULL;
}

/****************************************************************************/
 void*
mig_lst_iter_last (mig_lst_iter *iter)
{
        assert(iter);

        if (iter->list->tail)
                return (iter->list->tail->data);
        else
                return NULL;
}

/****************************************************************************/
 void
mig_lst_iter_reset (mig_lst_iter *iter)
{
        assert(iter);

        iter->curr = iter->list->head;
}

 /****************************************************************************/
void
mig_lst_free_custom_static_data_and_node ( mig_lst_t *list , mig_lst_free_f f, mig_lst_free_f free_node )
{
    mig_lst_node *tmp;
	mig_lst_free_f tmp_f;
    mig_lst_free_f freef;
	mig_lst_free_f tmp_f_node;

    assert ( list );

    if ( list->_free == NULL )
        freef = free;
    else
		freef = list->_free;

	
    if ( f == NULL ) 
	{
		tmp_f = free;
	}
	else
	{
		tmp_f = f;		
	}

	if ( free_node == NULL ) 
	{
		tmp_f_node = free;
	}
	else
	{
		tmp_f_node = free_node;		
	}

    while ( ( tmp = list->head ) != NULL )
    {
            tmp_f ( tmp->data );
            list->head = tmp->next;
			tmp_f_node ( tmp );
    }

    list->head = list->tail = NULL;
    list->num = 0;
}
