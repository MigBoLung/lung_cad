/*
******************************************************************************
*
* Author      : Todor Petkov
* Filename    : mig_im_regc.c
* Created     : 2007/06/21
* Description : 2D - 3D Region labeling and centroid
*
******************************************************************************
*/

#include "mig_im_regc.h"

/*
******************************************************************************
*                               LOCAL PROTOTYPES DECLARATION
******************************************************************************
*/

/*
******************************************************************************
*                       PROCESS A SINGLE 3D CONNECTED REGION
*
* Description : This function finds the pixels belonging to a single
*               26 connected 3D region starting from a single pixel
*               known to belong to the region.
*
* Arguments   : x      - horizontal coordinate of first pixel
*               y      - vertical coordinate of first pixel
*               z      - z coordinate of first pixel
*               Binary - 3D input stack
*               Width  - input stack width
*               Height - input stack height
*               Depth  - input stack z
*               Region - found 3D region ( returned )
*
* Returns     : 0 on success
*               -1 or error
*
* Notes       : Input array ( Binary ) is zeroed as new pixels blonging to 3D region
*               are found !
*
******************************************************************************
*/

static int
_process_region_3d ( int x , int y , int z ,
                     float *Binary ,
                     int Width , int Height , int Depth ,
                     mig_im_region_t *Region );

/*
******************************************************************************
*                       PROCESS A SINGLE 2D CONNECTED REGION
*
* Description : This function finds the pixels belonging to a single
*               8 connected 2D region starting from a single pixel
*               known to belong to the region.
*
* Arguments   : x      - horizontal coordinate of first pixel
*               y      - vertical coordinate of first pixel
*               Binary - 2D input stack
*               Width  - input image width
*               Height - input image height
*               Region - found 2D region ( returned )
*
* Returns     : 0 on success
*               -1 or error
*
* Notes       : Input array ( Binary ) is zeroed as new pixels blonging to 2D region
*               are found !
*
******************************************************************************
*/

static int
_process_region_2d ( int x , int y ,
                     float *Binary ,
                     int Width , int Height ,
                     mig_im_region_t *Region );

/*
******************************************************************************
*                               GLOBAL PROTOTYPES IMPLEMENTATION
******************************************************************************
*/

int
mig_im_regc_3d ( float *Binary ,
                 int Width , int Height , int Depth ,
                 mig_lst_t *Regions )
{
        int i , j , k;
        mig_im_region_t *NewRegion = NULL;

        for ( k = 0 ; k < Depth  ; ++ k )
        {
                for ( j = 0 ; j < Height ; ++ j )
                {
                        for ( i = 0 ; i < Width ; ++i )
                        {
                                /* skip zero pixels */
							if ( Binary[i+j*Width+k*Width*Height] <= MIG_EPS_32F )
                                        continue;

                                /* if we get here we struck a white (on)
                                   pixel -> new region */
                                NewRegion = (mig_im_region_t*) calloc ( 1 , sizeof(mig_im_region_t) );
                                if ( NewRegion == NULL )
                                        goto error;

                                /* calculate properties for this region */
                                if ( _process_region_3d ( i , j , k , Binary ,
                                        Width , Height , Depth , NewRegion ) == -1 )
                                        goto error;

                                /* add new region to stack */
                                if ( mig_lst_put_tail ( Regions , NewRegion ) != 0 )
                                        goto error;
                        }
                }
        }

        return 0;

error :

        /* clean up */
        return -1;
}

/******************************************************************************/

int
mig_im_regc_2d ( float *Binary ,
                 int Width , int Height ,
                 mig_lst_t *Regions )
{
    int i , j;
    mig_im_region_t *NewRegion = NULL;
    
    for ( j = 0 ; j < Height ; ++ j )
    {
        for ( i = 0 ; i < Width ; ++i )
        {
            /* skip zero pixels */
            if ( Binary[i+j*Width] <= MIG_EPS_32F )
                continue;

            /* if we get here we struck a white (on) pixel -> new region */
            NewRegion = (mig_im_region_t*) calloc ( 1 , sizeof(mig_im_region_t) );
            if ( NewRegion == NULL )
                goto error;

            /* calculate properties for this region */
            if ( _process_region_2d ( i , j , Binary ,
                  Width , Height , NewRegion ) == -1 )
                goto error;

            /* add new region to stack */
            if ( mig_lst_put_tail ( Regions , NewRegion ) != 0 )
                goto error;
        }
    }
    
    return 0;

error :

    /* clean up */
    return -1;
}

/*
******************************************************************************
*       THE FOLLOWING IO FUNCTIONS COME FROM mig_im_region.h
******************************************************************************
*/
//void
//mig_im_roi_save ( void *roi , void *file )
//{
//    mig_im_region_t *reg = (mig_im_region_t*) roi;
//    FILE *f = (FILE*) file;
//    mig_lst_node *node;
//    int num;
//
//    fprintf ( f , "\n" );
//
//    /* number of 2d objects in region */
//    num = ( reg->objs.num == 0 ) ? 1 : reg->objs.num;
//    
//    /* if region has only one slice dump it and exit */
//    if ( num == 1 )
//    {
//        fprintf ( f , "%d %d %d %f\n" , 
//                        (int) reg->centroid[0] , 
//                        (int) reg->centroid[1] , 
//                        (int) reg->centroid[2] , 
//                        reg->radius );
//        return;
//    }
//
//    /* if region has more than one slice dump all */
//    node = reg->objs.head;
//    while ( reg != NULL )
//    {
//        reg = (mig_im_region_t*) node->data;
//        fprintf ( f , "%d %d %d %f\n" , 
//                        (int) reg->centroid[0] , 
//                        (int) reg->centroid[1] , 
//                        (int) reg->centroid[2] , 
//                        reg->radius );
//        
//        node = node->next;    
//    }
//}

/******************************************************************************/
//void
//mig_im_roi_load ( void **roi , void *file )
//{
//      
//}

/******************************************************************************/
//void
//mig_im_roi_free ( void **roi )
//{
//    
//}

/*
******************************************************************************
*                               LOCAL PROTOTYPES IMPLEMENTATION
******************************************************************************
*/

/* search directions (x,y,z) */
static int _search_dir[26][3] =
{
    /* slice above 8 connectivity */
    {  0 , 0 , -1 } ,
    { -1 , 0 , -1 } ,
    { -1 ,+1 , -1 } ,
    {  0 ,+1 , -1 } ,
    { +1 ,+1 , -1 } ,
    { +1 , 0 , -1 } ,
    { +1 ,-1 , -1 } ,
    {  0 ,-1 , -1 } ,
    { -1 ,-1 , -1 } ,

    /* current slice 8 connecitivity */
    { -1 , 0 , 0 } ,
    { -1 ,+1 , 0 } ,
    {  0 ,+1 , 0 } ,
    { +1 ,+1 , 0 } ,
    { +1 , 0 , 0 } ,
    { +1 ,-1 , 0 } ,
    {  0 ,-1 , 0 } ,
    { -1 ,-1 , 0 } ,

    /* slice below 8 connectivity */
    {  0 , 0 , +1 } ,
    { -1 , 0 , +1 } ,
    { -1 ,+1 , +1 } ,
    {  0 ,+1 , +1 } ,
    { +1 ,+1 , +1 } ,
    { +1 , 0 , +1 } ,
    { +1 ,-1 , +1 } ,
    {  0 ,-1 , +1 } ,
    { -1 ,-1 , +1 }
};

/****************************************************************************/

static int
_process_region_3d ( int x , int y , int z ,
                     float *Binary ,
                     int Width , int Height , int Depth ,
                     mig_im_region_t *Region )
{    
    typedef struct _pix_t { int coord[3]; } pix_t;

    pix_t *CurrentPixel = NULL;
    pix_t *NewPixel = NULL;
    mig_stack_t Pixels;
    int i;
    int newx , newy , newz;
	int index;

    /* initialize stack */
    mig_stack_init ( &Pixels , &free );

    /* push first pixel on the stack */
    NewPixel = (pix_t*) malloc ( sizeof(pix_t) );
    if ( NewPixel == NULL )
        goto error;

    NewPixel->coord[0] = x;
    NewPixel->coord[1] = y;
    NewPixel->coord[2] = z;

    if ( mig_stack_push( &Pixels , NewPixel ) == -1 )
        goto error;

    /* Mark pushed pixel as being in queue */
    Binary[x+y*Width+z*Width*Height] = 0.0f;

    /* start processing all neighbourhood
       pixels -> when queue is empty we have finished */
    while ( mig_stack_pop( &Pixels , (void**) &CurrentPixel ) != -1 )
    {
        /* add coordinates to Region properties */
        Region->centroid[0] += (float) CurrentPixel->coord[0];
        Region->centroid[1] += (float) CurrentPixel->coord[1];
        Region->centroid[2] += (float) CurrentPixel->coord[2];
        Region->size ++;

        /* for all neighbours of current pixel */
		for ( i = 0 ; i < 26 ; ++i )
        {
            newx = CurrentPixel->coord[0] + _search_dir[i][0];
			newy = CurrentPixel->coord[1] + _search_dir[i][1];
			newz = CurrentPixel->coord[2] + _search_dir[i][2];

			/* check boundary pixels */
            if ( ( newx < 0 ) || ( newy < 0 ) || ( newz < 0 ) ||
                 ( newx >= Width ) || ( newy >= Height ) || ( newz >= Depth ) )
                continue;

            /* we found an "on" neighbour */
			/* AAAA Gianluca: Thresholding sets to zero all pixels
				values under threshold but doesn't set the others to 1.0f */
			/* if ( Binary[(int)( newx + newy * Width + newz * Width * Height ) ] == 1.0f )*/

			/* TODO: this fails on linux with newz greather than about 300, WHY?)*/
			index = newx + newy * Width + newz * Width * Height;
			if ( Binary[ index ] > MIG_EPS_32F )
			{
				NewPixel = (pix_t*) malloc ( sizeof(pix_t));
				if ( NewPixel == NULL )
					goto error;

				NewPixel->coord[0] = newx;
				NewPixel->coord[1] = newy;
				NewPixel->coord[2] = newz;

				/* add NewPixel to Pixels stack */
				if ( mig_stack_push ( &Pixels , NewPixel ) == -1 )
					goto error;

				/* mark NewPixel as being in queue */
				Binary[ index ] = 0.0f;
			}
        }

        /* free CurrentPixel structure */
        free ( CurrentPixel );
    } /* while stack is full */

    /* final region centroid calculation */
    Region->centroid[0] /= Region->size;
    Region->centroid[1] /= Region->size;
    Region->centroid[2] /= Region->size;

    return 0;

error :

    /* cleanup*/
    return -1;
}


/****************************************************************************/

static int
_process_region_2d ( int x , int y ,
                     float *Binary , 
                     int Width , int Height ,
                     mig_im_region_t *Region )
{
    typedef struct _pix_t { float coord[2]; } pix_t;

    pix_t *CurrentPixel = NULL;
    pix_t *NewPixel = NULL;
    mig_stack_t Pixels;
    int i;
    float newx , newy;

    /* initialize stack */
    mig_stack_init ( &Pixels , &free );

    /* push first pixel on the stack */
    NewPixel = (pix_t*) malloc ( sizeof(pix_t) );
    if ( NewPixel == NULL )
        goto error;

    NewPixel->coord[0] = x;
    NewPixel->coord[1] = y;
    
    if ( mig_stack_push( &Pixels , NewPixel ) == -1 )
        goto error;

    /* Mark pushed pixel as being in queue */
    Binary[x+y*Width] = 0.0f;

    /* start processing all neighbourhood pixels -> when queue is empty we have finished */
    while ( mig_stack_pop( &Pixels , (void**) &CurrentPixel ) != -1 )
    {
        /* add coordinates to Region properties */
        Region->centroid[0] += CurrentPixel->coord[0];
        Region->centroid[1] += CurrentPixel->coord[1];
        Region->size ++;

        /* for all neighbours of current pixel */
		for ( i = 0 ; i < 8 ; ++i )
        {
            newx = CurrentPixel->coord[0] + _search_dir[i+9][0];
			newy = CurrentPixel->coord[1] + _search_dir[i+9][1];
			
			/* check boundary pixels */
            if ( ( newx < 0 ) || ( newy < 0 ) ||
                 ( newx >= Width ) || ( newy >= Height ) )
                continue;

            /* we found an "on" neighbour */
			/* AAAA Gianluca: Thresholding sets to zero all pixels
				values under threshold but doesn't set the others to 1.0f */
			/*if ( Binary[(int)( newx + newy * Width ) ] == 1.0f )*/
			if ( Binary[(int)( newx + newy * Width ) ] > MIG_EPS_32F )
			{
				NewPixel = (pix_t*) malloc ( sizeof(pix_t));
				if ( NewPixel == NULL )
					goto error;

				NewPixel->coord[0] = newx;
				NewPixel->coord[1] = newy;

				/* add NewPixel to Pixels stack */
				if ( mig_stack_push ( &Pixels , NewPixel ) == -1 )
					goto error;

				/* mark NewPixel as being in queue */
				Binary[(int)( newx + newy * Width ) ] = 0.0f;
			}
        }

        /* free CurrentPixel structure */
        free ( CurrentPixel );
    } /* while stack is full */

    /* final region centroid calculation */
    Region->centroid[0] /= Region->size;
    Region->centroid[1] /= Region->size;
    
    return 0;

error :

    /* cleanup*/
    return -1;
}


int
mig_im_regc_3d_odd ( float *Binary ,
                 int Width , int Height , int Depth ,
                 mig_lst_t *Regions )
{
        int i , j , k;
        mig_im_region_t *NewRegion = NULL;

        for ( k = 0 ; k < Depth  ; ++ k )
        {
                for ( j = 0 ; j < Height ; ++ j )
                {
                        for ( i = 0 ; i < Width ; ++i )
                        {
                                /* skip zero pixels */
                                if ( Binary[i+j*Width+k*Width*Height] <= MIG_EPS_32F )
                                        continue;

                                /* if we get here we struck a white (on)
                                   pixel -> new region */
                                NewRegion = (mig_im_region_t*) calloc ( 1 , sizeof(mig_im_region_t) );
                                if ( NewRegion == NULL )
                                        goto error;

                                /* calculate properties for this region */
                                /*
								if ( _process_region_3d ( i , j , k , Binary ,
                                        Width , Height , Depth , NewRegion ) == -1 )
                                        goto error;
								*/

								NewRegion->centroid[0] = i;
								NewRegion->centroid[1] = j;
								NewRegion->centroid[2] = k;
								NewRegion->radius = 0;
								NewRegion->size = 0;

                                /* add new region to stack */
                                if ( mig_lst_put_tail ( Regions , NewRegion ) != 0 )
                                        goto error;
                        }
                }
        }

        return 0;

error :

        /* clean up */
        return -1;
}
