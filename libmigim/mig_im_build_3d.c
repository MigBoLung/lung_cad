/*
 ******************************************************************************
 *
 * Author      : Todor Petkov
 * Filename    : mig_im_build_3d.c
 * Created     : 2007/06/21
 * Description : Building and Pruning of 3D regions out of 2D regions
 *
 ******************************************************************************
 */

#include "mig_im_build_3d.h"

/*
 ******************************************************************************
 *               PRIVATE PROTOTYPES DECLARATION
 ******************************************************************************
 */

static int
_obj3d_list_sort_cmp ( const void *a , const void *b );


/*
 ******************************************************************************
 *               GLOBAL PROTOTYPES IMPLEMENTATION
 ******************************************************************************
 */

    int
mig_im_build_obj3d ( mig_lst_t *Src , 
        mig_lst_t *Results ,
        mig_lst_cmp_f build_f , 
        mig_lst_sel_f cut_f ,
        mig_lst_free_f free_f,
        int just_copy_and_filter )
{
    mig_im_region_t *Curr, *Tmp1 , *Tmp2;
    mig_stack_t stack = { 0 , NULL , NULL , NULL };

    if (just_copy_and_filter)
        mig_lst_cat( Src, Results);
    else
    {
        /* while there are still nodes in source list */
        while ( Curr = (mig_im_region_t*) mig_lst_get_head ( Src ) )
        {
            /* we found a new 3D object */
            /*printf("\nget from list\n");
              printf("%f %f %f\n",Curr->centroid[0],Curr->centroid[1],Curr->centroid[2]);*/
            /* register list free function for this 3d view */
            //Curr->objs._free = Src->free;
            Curr->objs._free = &free;

            /* push new node on current 3D object stack */
            /*printf("\npush into stack\n");
              printf("%f %f %f\n",Curr->centroid[0],Curr->centroid[1],Curr->centroid[2]);*/
            if ( mig_stack_push ( & stack , Curr ) == -1 )
                return -1;

            /* start processing stack */
            while ( mig_stack_pop ( &stack , (void**) &Tmp1 ) != -1 )
            {
                /*printf("\npop from stack and add to Curr->objs\n");
                  printf("%f %f %f\n",Tmp1->centroid[0],Tmp1->centroid[1],Tmp1->centroid[2]);*/
                /* add poped node to current region */
                if ( mig_lst_put_head ( &( Curr->objs ) , Tmp1 ) == -1 )
                    return -1;

                /* search input list for all other nodes matching inclusion criteria */
                while ( Tmp2 = (mig_im_region_t*) mig_lst_rem ( Src , Tmp1 , build_f ) )
                {
                    /*printf("\npush this region to stack\n");
                      printf("%f %f %f\n",Tmp2->centroid[0],Tmp2->centroid[1],Tmp2->centroid[2]);*/
                    if ( mig_stack_push ( & stack , Tmp2 ) == -1 )
                        return -1;
                }
            }

            /* here we have finished processing one 3D region */

            /* compact newly found 3D region : only one 2D view
               for each composing 2d object */
            obj3d_compact ( Curr );

            /* sort new found 3d region views with respect to coordinates */
            /* AAAAAA doesn't work */
            mig_lst_sort ( &( Curr->objs ) , &_obj3d_list_sort_cmp );

            /* add 3D region to results list */
            if ( mig_lst_put_head ( Results , Curr ) == -1 )
                return -1;
        } 
    }
    /* here we have finished processing all 3D regions : now prune list */
    mig_lst_rem_all ( Results , cut_f , free_f );

    return 0;
}

/*
 ******************************************************************************
 *               PRIVATE PROTOTYPES IMPLEMENTATION
 ******************************************************************************
 */

    static int
_obj3d_list_sort_cmp ( const void *a , const void *b )
{
    /*
       mig_im_region_t *reg1 = (mig_im_region_t*) a;
       mig_im_region_t *reg2 = (mig_im_region_t*) b;
       */
    mig_lst_node **node_a = a;
    mig_lst_node **node_b = b;
    mig_im_region_t *reg1 = (mig_im_region_t*)(*node_a)->data;
    mig_im_region_t *reg2 = (mig_im_region_t*)(*node_b)->data;

    if ( reg1->centroid[2] > reg2->centroid[2] )
        return 1;

    if ( reg1->centroid[2] < reg2->centroid[2] )
        return -1;

    return 0;
}


/*******************************************************************************/

    void
obj3d_compact ( mig_im_region_t *Region )
{
    mig_lst_t l = { NULL , NULL , 0 , &free };
    mig_im_region_t *prev , *curr;
    int cnt = 1;
    float mean_x = 0.0f , mean_y = 0.0f, mean_z = 0.0f , mean_r = 0.0f;

    if ( mig_lst_len( &( Region->objs ) ) <= 1 )
        return;

    prev = (mig_im_region_t*) mig_lst_get_head ( &( Region->objs ) );

    /* while there are still nodes in the source list */
    while ( curr = (mig_im_region_t*) mig_lst_get_head ( &( Region->objs ) ) )
    {
        /* 2d objects found on same slice */
        if ( MIG_ABS( curr->centroid[2] - prev->centroid[2] ) < 1.0f )
        {
            prev->centroid[0] += curr->centroid[0];
            prev->centroid[1] += curr->centroid[1];
            prev->radius      += curr->radius;
            cnt ++;
            free ( curr );
        }
        /* 2d object found on a different slice */
        else
        {
            /* calculate mean for prev data */
            prev->centroid[0] /= cnt;
            prev->centroid[1] /= cnt;
            prev->radius /= cnt;

            /* update 3d region data */
            mean_x += prev->centroid[0];
            mean_y += prev->centroid[1];
            mean_z += prev->centroid[2];
            mean_r += prev->radius;

            /* add previous 2d object to local list */
            mig_lst_put_tail ( &l , prev );

            /* zero counter */
            cnt = 1;

            /* change prev */
            prev = curr;

        }
    } /* while */

    /* copy local list back to Region */
    mig_lst_cat ( &l , &(Region->objs) );

    /* calculate mean for 3d region */
    /*
       Region->centroid[0] /= mig_lst_len( &( Region->objs ) );
       Region->centroid[1] /= mig_lst_len( &( Region->objs ) );
       Region->centroid[2] /= mig_lst_len( &( Region->objs ) );
       Region->radius      /= mig_lst_len( &( Region->objs ) );
       */
    Region->centroid[0] = mean_x / mig_lst_len( &( Region->objs ) );
    Region->centroid[1] = mean_y / mig_lst_len( &( Region->objs ) );
    Region->centroid[2] = mean_z / mig_lst_len( &( Region->objs ) );
    Region->radius      = mean_r / mig_lst_len( &( Region->objs ) );
}
