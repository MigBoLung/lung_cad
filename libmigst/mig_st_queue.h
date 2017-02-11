#ifndef __MIG_QUEUE_H__
#define __MIG_QUEUE_H__

#include "pthread.h"
#include "semaphore.h"
#include "sched.h"

#include "mig_config.h"
#include "mig_defs.h"

MIG_C_LINKAGE_START

/* single entry */
typedef struct _mig_entry_t
{
	void	*data;
	struct _mig_entry_t *next;   	/* next entry */

} mig_entry_t;

/* sentries queue */
typedef struct _mig_queue_t
{
    mig_entry_t *head;	/* head */
    mig_entry_t *tail;	/* tail */
    int len;		/* curretn number of entries in queue */
    int max_len;	/* maximum allowed lenght for queue */
    
    sem_t *empty;	/* number of empty slots in the queue */
    sem_t *full;		/* number of available items to process */

    pthread_mutex_t mutex;	/* queue mutex */
    pthread_cond_t  cond;	/* queue cond */

} mig_queue_t;

/* create queue */
extern int
mig_queue_init ( mig_queue_t *queue , 
		         int max_queue_len );

/* add an entry */
extern int
mig_queue_add ( mig_queue_t *queue , void *data );

/* get first entry */
extern void*
mig_queue_get ( mig_queue_t *queue );

/* get current number of entries */
extern int
mig_queue_get_len ( mig_queue_t *queue );

/* get maximum allowed number of entries */
extern int
mig_queue_get_max_len ( mig_queue_t *queue );

/* custom free function */
typedef void( *queue_free_f )( void *data );

/* free queue */
extern void
mig_queue_del ( mig_queue_t *queue , queue_free_f f );

/* custom display function */
typedef void ( *queue_dump_f )( void *data );

extern void
mig_queue_dump ( mig_queue_t *queue, queue_dump_f f );

MIG_C_LINKAGE_END

#endif /* __MIG_QUEUE_H__ */
