#include "libmigtag.h"

/*******************************************************************/
/* PRIVATE FUNCTIONS */
/*******************************************************************/

static void
_node_put ( FILE *f , 
            mig_im_region_t *node );

static mig_im_region_t*
_node_get ( FILE *f );

/*******************************************************************/
/* EXPORTED FUNCTIONS */
/*******************************************************************/
int
mig_tag_write ( char *name , mig_lst_t *results )
{
        int i , NumNodes;
        FILE *f = NULL;
        mig_im_region_t *CurrNode;
        
        assert( name );
        assert( results );

        /* get number of nodes in results list */
        NumNodes = mig_lst_len( results );
        
        /* if results list is empty return */
        /* GF 20100930 - if results list is empty write 0 and return */
		/*
		if ( NumNodes == 0 )
                return MIG_OK;
		*/

        /* try opening/creating output file */
        f = fopen ( name , "w" );
        if ( f == NULL )
                return MIG_ERROR_IO;

        /* put results list size */
        fprintf ( f ,  "%d\n" , NumNodes );

        /* for all nodes inside results list */
        for ( i = 0 ; i < NumNodes ; ++i )
        {
                CurrNode = (mig_im_region_t*) 
                        mig_lst_peek ( results , i );

                /* write node id = i */
                fprintf ( f ,  "%d\n" , i + 1 );

                /* write node */
                _node_put ( f , CurrNode );
        }

        fflush ( f );
        fclose ( f );
        
        return MIG_OK;
}

/*******************************************************************/
int
mig_tag_read ( char *name , mig_lst_t *results )
{
        FILE *f = NULL;
        char line[LINE_MAX];
        mig_im_region_t *new_node;
        int i , num_nodes;
        
        assert( name );
        assert( results );
		

		/* AAAAAA GIANLUCA:20071112 shouldn't we clean the structure? possible LEAK*/
		
		/* GIANLUCA:20071112 to make it possible to preserve data from previous dlls
		/	we comment this and do it after the file is successfully opened */
        /* zero input list */
        /* memset ( results , 0x00 , sizeof( mig_lst_t ) ); */

        /* open tag file for text reading */
        f = fopen ( name , "r" );
        if ( f == NULL )
                return MIG_ERROR_IO;

        /* read a line from file */
        if ( fgets ( (char*) &line , LINE_MAX , f ) == NULL )
                goto error;

        /* parse line to get number of nodes in tag file */
        if ( sscanf ( (const char*) &line , "%d\n" , &num_nodes ) != 1 )
                goto error;
        
        /* wrong tag file syntax */
        if ( num_nodes == 0 )
                goto error;
        
		/* AAAAAA OK: here we didn't use results yet and we got some confidence that
		/our file is ok, so clean up (REMEMBER TO CHANGE FOR LEAK) */
		
		/* for better error checking consider writing to a temp list, then copy to results */
		
		/* zero input list */
        memset ( results , 0x00 , sizeof( mig_lst_t ) );

        /* for all nodes inside tag file */
        for ( i = 0 ; i < num_nodes ; ++i )
        {
                new_node = _node_get ( f );
                if ( new_node == NULL )
                        goto error;

                /* add new node to list */
                if ( mig_lst_put_tail ( results , new_node ) != 0 )
                        goto error;
        }

        fclose ( f );
        
        return MIG_OK;

error :
        if ( f != NULL )
                fclose ( f );

        mig_lst_empty ( results );
        
        return MIG_ERROR_IO;
}

/*******************************************************************/
void
mig_tag_free ( mig_lst_t *results )
{
    if ( results == NULL )
        return; 
}

/*******************************************************************/
/* PRIVATE FUNCTIONS */
/*******************************************************************/

static void
_node_put ( FILE *f , mig_im_region_t *reg )
{
    mig_lst_node *node;
    int num;

    /* number of 2d objects in region */
    num = ( reg->objs.num == 0 ) ? 1 : reg->objs.num;

    /* write total number of slices */
    fprintf ( f , "%d\n" , num );
    
    /* if region has only one slice dump it and exit */
    if ( num == 1 )
    {
        /* z coord , x coord , y coord , min diam pixels , max diam pixels */
        fprintf ( f , "%d %d %d %d %d\n" , 
                            (int) reg->centroid[2] , 
                            (int) reg->centroid[0] , 
                            (int) reg->centroid[1] , 
                            (int) floorf( 2.0f * reg->radius + 0.5f ) ,
                            (int) floorf( 2.0f * reg->radius + 0.5f ) );
        return;
    }

    /* if region has more than one slice dump all */
    node = reg->objs.head;
    while ( node != NULL )
    {
        reg = (mig_im_region_t*) node->data;
        
        /* z coord , x coord , y coord , min diam pixels , max diam pixels */
        fprintf ( f , "%d %d %d %d %d\n" , 
                        (int) reg->centroid[2] , 
                        (int) reg->centroid[0] , 
                        (int) reg->centroid[1] , 
                        (int) floorf( 2.0f * reg->radius + 0.5f ) ,
                        (int) floorf( 2.0f * reg->radius + 0.5f ) );
        
        node = node->next;    
    }
}

/*******************************************************************/

static mig_im_region_t*
_node_get ( FILE *f )
{
        char line[LINE_MAX];
        mig_im_region_t *new_node = NULL , *new_node_tmp;
        int i , id , num_slices;

        int x , y , z , diam;
        
        /* read a line from file */
        if ( fgets ( (char*) &line , LINE_MAX , f ) == NULL )
                goto error;

        /* parse line to get id current region */
        if ( sscanf ( (const char*) &line , "%d\n" , &id ) != 1 )
                goto error;
        
        /* read a line from file */
        if ( fgets ( (char*) &line , LINE_MAX , f ) == NULL )
                goto error;

        /* parse line to get number of slices current region */
        if ( sscanf ( (const char*) &line , "%d\n" , &num_slices ) != 1 )
                goto error;
        
        /* allocate memory for new node */
        new_node = (mig_im_region_t*) calloc ( 1 , sizeof( mig_im_region_t ) );
        if ( new_node == NULL )
                return NULL;
        
        /* read a line from file */
        if ( fgets ( (char*) &line , LINE_MAX , f ) == NULL )
                goto error;
        
        /* parse and decode node line */
        if ( sscanf ( (const char*) &line , "%d %d %d %d\n" , 
                                &z , &x , &y , &diam ) != 4 )
            goto error;

        new_node->centroid[0] = (float) x;
        new_node->centroid[1] = (float) y;
        new_node->centroid[2] = (float) z;
        new_node->radius      = (float) diam / 2.0f;
        new_node->size        = MIG_PI * MIG_POW2( new_node->radius );

        /* if 3d object has only one slice return now */
        if ( num_slices == 1 )
            return new_node;

        /* if we have more than one slice we have to process then all separately */
        
        /* copy first slice as is */
        new_node_tmp = (mig_im_region_t*) calloc ( 1 , sizeof( mig_im_region_t ) );
        if ( new_node_tmp == NULL )
            goto error;

        memcpy ( new_node_tmp , new_node , sizeof( mig_im_region_t ) );
        
		/* set free function in new_node->objs */
		new_node->objs._free = free;

        if ( mig_lst_put_tail ( &( new_node->objs ) , new_node_tmp ) != 0 )
            goto error;

        /* now copy all other slices */
        for ( i = 0 ; i < ( num_slices - 1 ) ; ++i )
        {             
            if ( fgets ( (char*) &line , LINE_MAX , f ) == NULL )
                goto error;
        
            /* parse and decode node line */
            if ( sscanf ( (const char*) &line , "%d %d %d %d\n" , 
                                &z , &x , &y , &diam ) != 4 )
                goto error;

            new_node_tmp = (mig_im_region_t*) calloc ( 1 , sizeof( mig_im_region_t ) );
            if ( new_node_tmp == NULL )
                goto error;

            new_node_tmp->centroid[0] = (float) x;
            new_node_tmp->centroid[1] = (float) y;
            new_node_tmp->centroid[2] = (float) z;
            new_node_tmp->radius      = (float) diam / 2.0f;
            new_node_tmp->size        = MIG_PI * MIG_POW2( new_node->radius );
        
            if ( mig_lst_put_tail ( &( new_node->objs ) , new_node_tmp ) != 0 )
                goto error;
        }
    
        return new_node;

error :
        
        if ( new_node == NULL )
            return NULL;

        if ( mig_lst_len ( &( new_node->objs ) ) == 0 )
        {
            free ( new_node );
            return NULL;
        }

        mig_lst_empty ( &( new_node->objs ) );
        free ( new_node );
        return NULL;
}


int
mig_tag_resize ( mig_lst_t *results, float list_z_res, float target_z_res)
{	
	float ratio = list_z_res / target_z_res;
	mig_lst_iter iter, iter_inner;
	mig_im_region_t* nod;
	mig_im_region_t* nod_inner;

	/* iterate through elements and assign a 3d centroid value */
	mig_lst_iter_get ( &iter , results );
	while ( ( nod = (mig_im_region_t*) mig_lst_iter_next ( &iter ) ) != NULL )
	{
		if ( nod )
		{
			/* change global (3d) centroid */
			nod->centroid[2] = (int) ( nod->centroid[2] * ratio + 0.5 ) + 1;//instance number starts at 1
			/* if 2d tags are present change them as well */
			if ( nod->objs.num != 0 )
			{
				mig_lst_iter_get ( &iter_inner , &nod->objs );
				while ( ( nod_inner = (mig_im_region_t*) mig_lst_iter_next ( &iter ) ) != NULL )
				{
					if ( nod_inner )
					{
						/* change global (3d) centroid */
						nod_inner->centroid[2] = (int) ( nod_inner->centroid[2] * ratio + 0.5 );
					}
				}
			}
		}
	}
	return MIG_OK;
}
