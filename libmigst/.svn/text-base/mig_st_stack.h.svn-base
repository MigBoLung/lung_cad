#ifndef __MIG_STACK_H__
#define __MIG_STACK_H__

#include "mig_config.h"
#include "mig_defs.h"

MIG_C_LINKAGE_START

typedef struct _mig_stack_entry_t
{
	void *data;
	struct _mig_stack_entry_t *next;

} mig_stack_entry_t;

typedef struct _mig_stack_t
{
	int	size;
	void (*destroy)( void *data );

	mig_stack_entry_t *head;
	mig_stack_entry_t *tail;

} mig_stack_t;

extern void
mig_stack_init ( mig_stack_t *stack ,
                 void (*destroy)(void *data) );

extern void
mig_stack_destroy ( mig_stack_t *stack );

extern int
mig_stack_push ( mig_stack_t *stack ,
                 void *data );

extern int
mig_stack_pop ( mig_stack_t *stack ,
                void **data );

#define mig_stack_peek(stack) \
	( ( (stack)->head == NULL ) ? NULL : (stack)->head->data )

#define mig_stack_size(stack) \
	((stack)->size)

MIG_C_LINKAGE_END

#endif	/* __MIG_STACK_H__ */
