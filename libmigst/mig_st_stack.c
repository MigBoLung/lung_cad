#include "mig_st_stack.h"

/*****************************************************************************/
/* EXPORTS */
/*****************************************************************************/

void
mig_stack_init ( mig_stack_t *stack ,
                 void (*destroy)(void *data) )
{
        stack->size = 0;
        stack->destroy = destroy;
        stack->head = NULL;
        stack->tail = NULL;
}

/*****************************************************************************/
void
mig_stack_destroy ( mig_stack_t *stack )
{
        void *data;

        while ( stack->size > 0 )
        {
                if ( mig_stack_pop( stack , 
                     (void**)&data ) == 0 &&
                     stack->destroy != NULL )
                        stack->destroy( data );
        }
}

/*****************************************************************************/
int
mig_stack_push ( mig_stack_t *stack , 
                 void *data )
{
        mig_stack_entry_t *NewEntry;

        NewEntry = (mig_stack_entry_t*)
                calloc ( 1 , sizeof(mig_stack_entry_t) );
        if ( NewEntry == NULL )
                return -1;

        NewEntry->data = data;

        if ( stack->size == 0 )
                stack->tail = NewEntry;

        NewEntry->next = stack->head;
        stack->head = NewEntry;

        stack->size ++;

        return 0;
}

/*****************************************************************************/
int
mig_stack_pop ( mig_stack_t *stack , 
                void **data )
{
        mig_stack_entry_t *OldEntry;

        if ( stack->size == 0 )
                return -1;

        *data = stack->head->data;
        OldEntry = stack->head;
        stack->head = stack->head->next;

        if ( stack->size == 1 )
                stack->tail = NULL;

        free ( OldEntry );
        stack->size --;

        return 0;
}

