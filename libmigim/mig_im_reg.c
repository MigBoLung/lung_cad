#include "mig_im_reg.h"
#include "mig_error_codes.h"

/****************************************************************************/
/* EXPORTS */
/****************************************************************************/
int
mig_reg_prop_sep ( Mig8u *labs , 
                   int w , int h , 
                   mig_lst_t **props , 
                   int num_cc )
{
        int i , j;
        mig_reg_props_t *RegionProperties = NULL;      /* properties for one single region */
        mig_lst_t *Regions = NULL;                       /* list of region properties -> 
                                                          length is equal to num_cc */
        Regions = mig_lst_alloc ();
        if ( Regions == NULL )
                goto error;

        /* prepare region properties list */
        for ( i = 0 ; i < num_cc ; ++i )
        {
                RegionProperties = (mig_reg_props_t*)
                        malloc ( sizeof (mig_reg_props_t) );
                if ( RegionProperties == NULL )
                        goto error;

                /* reset all props */
                RegionProperties->id = i + 1;
                RegionProperties->area = 0;
                RegionProperties->centroid[0] = 0.0;
                RegionProperties->centroid[1] = 0.0;

                if ( mig_lst_put_tail ( Regions , 
                                        RegionProperties ) != MIG_OK )
                        goto error;
        }

        /* scan image and calculate properties */
        for ( j = 0 ; j < h ; ++j )
        {
                for ( i = 0 ; i < w ; ++i , ++labs )
                {
                        if ( *labs == 0x00 )
                                continue;

                        RegionProperties = (mig_reg_props_t*)
                                mig_lst_peek ( Regions , ((int)*labs) - 1 );

                        /* centroid horizontal */
                        RegionProperties->centroid[0]  =
                                ( (double)i + 
                                ( RegionProperties->area ) * 
                                ( RegionProperties->centroid[0] ) ) / 
                                ( RegionProperties->area + 1.0 );

                        /* centroid vertical */
                        RegionProperties->centroid[1]  =
                                ( (double)j + 
                                ( RegionProperties->area ) * 
                                ( RegionProperties->centroid[1] ) ) / 
                                ( RegionProperties->area + 1.0 );

                        RegionProperties->area ++;
                }
        }

        *props = Regions;
        return MIG_OK;

error :
        if ( Regions != NULL )
                mig_lst_free ( Regions );
        return MIG_ERROR_MEMORY;        
}

/****************************************************************************/
int
mig_reg_prop_vol ( Mig8u *labs , 
                   int w , int h , int z , 
                   mig_lst_t **props , 
                   int num_cc )
{
        int i , j , k;
        mig_reg_props_t *RegionProperties = NULL;
        mig_lst_t *Regions = NULL;

        Regions = mig_lst_alloc ();
        if ( Regions == NULL )
                goto error;

        /* prepare region properties list */
        for ( i = 0 ; i < num_cc; ++i )
        {
                RegionProperties = (mig_reg_props_t*)
                        malloc ( sizeof ( mig_reg_props_t ) );
                if ( RegionProperties == NULL )
                        goto error;

                /* reset all props */
                RegionProperties->id = i + 1;
                RegionProperties->area = 0;
                RegionProperties->min_coord[0] = w;
                RegionProperties->min_coord[1] = h;
                RegionProperties->min_coord[2] = z;
                
                RegionProperties->max_coord[0] = 0;
                RegionProperties->max_coord[1] = 0;
                RegionProperties->max_coord[2] = 0;

                if ( mig_lst_put_tail ( Regions , 
                        RegionProperties ) != MIG_OK )
                        goto error;
        }

        for ( k = 0 ; k < z ; ++ k )    /* SLICES */
        {
                for ( j = 0 ; j < h ; ++ j )    /* VERTICAL */
                {
                        for ( i = 0 ; i < w ; ++ i , ++ labs )  /* HORIZONTAL */
                        {
                                /* if pixel is off continue */
                                if ( *labs == 0x00 )
                                        continue;

                                /* get Region properties structure corresponding
                                   to current pixel label */
                                RegionProperties = (mig_reg_props_t*) 
                                        mig_lst_peek ( Regions , ((int)*labs) - 1 );

                                if ( RegionProperties == NULL )
                                        goto error;

                                /* bounding box */

                                /* min coordinates */
                                if ( i < RegionProperties->min_coord[0] )
                                        RegionProperties->min_coord[0] = i;
                                
                                if ( j < RegionProperties->min_coord[1] )
                                        RegionProperties->min_coord[1] = j;
                                
                                if ( k < RegionProperties->min_coord[2] )
                                        RegionProperties->min_coord[2] = k;
                                
                                /* max coordinates */
                                if ( i > RegionProperties->max_coord[0] )
                                        RegionProperties->max_coord[0] = i;
                                
                                if ( j > RegionProperties->max_coord[1] )
                                        RegionProperties->max_coord[1] = j;
                                
                                if ( k > RegionProperties->max_coord[2] )
                                        RegionProperties->max_coord[2] = k;
                                
                                /* volume */
                                ++ ( RegionProperties->area );
                        }
                }
        }

        /* save results */
        *props = Regions;

        return MIG_OK;

error :
        if ( Regions != NULL )
                mig_lst_free ( Regions );

        return MIG_ERROR_MEMORY;
}
