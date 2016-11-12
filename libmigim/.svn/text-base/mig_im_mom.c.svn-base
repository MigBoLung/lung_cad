/*
******************************************************************************
*
* Author      : Todor Petkov, Gianluca Ferri
* Filename    : mig_im_mom.c
* Created     : 2010/06/10
* Description : Image moments extraction (Zernike at the "moment" :-D )
*
******************************************************************************
*/

#include "mig_im_mom.h"

/*
******************************************************************************
*               LOCAL PROTOTYPES DECLARATION
******************************************************************************
*/

#if defined(__cplusplus)
extern "C" {
#endif

/* fill masks with computed polynomials values for the orders passed in orders */
static int _compute_masks ( mig_im_mom_t *Masks );

/* returns img computed with poly values */
static int _set_poly_img ( int size, int order, float* img_re, float* img_im );

/* get p and q from the progressive index (our "order") */
static void _get_pzer_orders ( int order, int* p, int* q );

/* precompute Dpqs coefficients for order p,q and put them in tbl
	NOTE: must tbl must be freed once used! */
static int*
_get_fact_coeffs ( int p, int q );

/* get Rpq value */
static float
_pzer_poly ( int p , int q ,
             int *tbl , float r );

/* get factorial of number n */
static double
fact ( int n );

#if defined(__cplusplus)
}
#endif


/*
******************************************************************************
*               GLOBAL PROTOTYPES IMPLEMENTATION
******************************************************************************
*/

mig_im_mom_t*
mig_im_mom_get ( int size, int *orders , int num_orders )
{
	mig_im_mom_t *Masks;	

	int i;
	int error_state = 0;

	Masks = (mig_im_mom_t*)
            calloc ( 1 , sizeof(mig_im_mom_t) );
	if ( Masks == NULL )
	{
		return NULL;
	}
	
	/* fill with passed data */

	Masks->size = size;
	Masks->orders = orders;
	Masks->num_orders = num_orders;
	Masks->elemsperorder = MIG_POW2(size);

	
	/* allocate space for computed value */

	Masks->values_re = (float**) calloc ( Masks->num_orders , sizeof(float*) );
	Masks->values_im = (float**) calloc ( Masks->num_orders , sizeof(float*) );
		

	if ( Masks->values_re == NULL || Masks->values_im == NULL)
	{
		return NULL;
	}
	
	memset (Masks->values_re, 0x00000000 , Masks->num_orders * sizeof(float*) );
	memset (Masks->values_im, 0x00000000 , Masks->num_orders * sizeof(float*) );

	for ( i = 0; i != Masks->num_orders; ++i )
	{
		Masks->values_re[i] = (float*) calloc ( Masks->elemsperorder , sizeof(float) );
		if ( Masks->values_re[i] == NULL )
		{
			error_state = 1;
			break;
		}
		Masks->values_im[i] = (float*) calloc ( Masks->elemsperorder , sizeof(float) );
		if ( Masks->values_im[i] == NULL )
		{
			error_state = 1;
			break;
		}
	}

	if (error_state)
	{
		for ( i = 0; i != Masks->num_orders; ++i )
		{
			if ( Masks->values_re[i] )
				free ( Masks->values_re[i] );
			if ( Masks->values_im[i] )
				free ( Masks->values_im[i] );
		}
		
		free ( Masks->values_re );
		free ( Masks->values_im );

		return MIG_ERROR_MEMORY;
	}


	/* fill masks with computed values */

	if ( _compute_masks( Masks ) )
	{
		return NULL;
	}
	
	return Masks;
}

/****************************************************************************/

/* TODO: take into account a normalization factor for different sizes */
int
mig_im_mom_crop_2D ( float *crop,
					int size ,
                    mig_im_mom_t *Masks , 
                    float *moments )
{
	int i;
	int order_idx;
	float temp_re = 0;
	float temp_im = 0;
	int nelem;

	if (size != Masks->size)
		return MIG_ERROR_PARAM;

	nelem = MIG_POW2(size);

	for ( order_idx = 0; order_idx < Masks->num_orders; order_idx++ )
	{
		temp_re = 0;
		temp_im = 0;

		for ( i = 0; i != nelem; i++ )
		{
			/*workaround for last row of resize */
			temp_re += ( MIG_MAX2(0,crop[i] / MIG_MAX_16U) ) * ( Masks->values_re[order_idx][i] );
			temp_im += ( MIG_MAX2(0,crop[i] / MIG_MAX_16U) ) * ( Masks->values_im[order_idx][i] );
		}

		moments[order_idx] = sqrt ( MIG_POW2((double)temp_re) + MIG_POW2((double)temp_im) );
	}
	
	return MIG_OK;
}


/****************************************************************************/

void
mig_im_mom_del ( mig_im_mom_t *Masks )
{
		if ( Masks != NULL )
		{
			if ( Masks->values_re )
				free ( Masks->values_re );
			if ( Masks->values_im )
				free ( Masks->values_im );
            free ( Masks );

		}
}				  

/*
******************************************************************************
*               LOCAL PROTOTYPES IMPLEMENTATION
******************************************************************************
*/

int _compute_masks( mig_im_mom_t *Masks )
{
	int order_idx = 0;

	for ( order_idx = 0; order_idx < Masks->num_orders; order_idx++ )
	{
		/*this indirection could be removed, but this way we can redefine the way
		  masks for an order are obtained, and their interface is independent from
		  mig_im_mom_t, remember: we do this only at Masks creation! */

		if ( _set_poly_img ( Masks->size, Masks->orders[order_idx],
			Masks->values_re[order_idx], Masks->values_im[order_idx] ) )
		{
			return MIG_ERROR_INTERNAL;
		}

	}

	return MIG_OK;
}

/*************************************************************************/

int _set_poly_img ( int size, int order, float* img_re, float* img_im )
{
	int ret = MIG_OK;
	int elem_idx;
	int n_elem = MIG_POW2(size);
	int half_size = size / 2;

	float x;
	float y;

	float r;
	float phi;

	float real_part;
	float imaginary_part;

	float zerR;

	int p,q;

	int *tbl = NULL;

	_get_pzer_orders ( order, &p, &q);

	tbl = _get_fact_coeffs ( p, q );

	if ( tbl == NULL )
		return MIG_ERROR_MEMORY;

	/* TODO: handle interpolation, ok if we have acceptable resolution*/

	for ( elem_idx = 0 ; elem_idx < n_elem; elem_idx++ ){
		/* normalized positions, we sample at the center of the pixel */
		x = ( ( elem_idx % size ) - half_size + 0.5) / half_size;
		y = ( half_size - ( elem_idx / size ) - 0.5) / half_size;
		
		r = sqrt ( MIG_POW2(x) + MIG_POW2(y) );
		phi = atan2 ( y, x );

		/* if outside unit circle set to 0 */
		if ( r > 1)
		{
			img_re[elem_idx] = 0;
			img_im[elem_idx] = 0;
		}
		else
		{	
			/* get Rpq value */
			zerR = _pzer_poly( p, q, tbl, r);

			/* get real and imaginary parts */
            real_part = zerR * cos ( ( phi ) * (float)q );

            imaginary_part = zerR * sin ( ( phi ) * (float)q );

			img_re[elem_idx] = real_part;
			img_im[elem_idx] = imaginary_part;	

		}
	} /*end for elem_idx*/
	
	/* clean factorial coefficients */
	free ( tbl );
	
	return MIG_OK;
}

/********************************************************************************/


static void
_get_pzer_orders (int order, int *p, int *q)
{
	int i = 0;
	int lp, lq;
	lp = 0;
	lq = 0;

	for ( i = 0; i != order; i++ )
	{
		if (lp != lq)
		{
			++lq;
		}
		else
		{
			++lp;
			lq = 0;
		}
	}

	*p = lp;
	*q = lq;
}


/* TODO: in Todor's code was double, ask if necessary */
static float
_pzer_poly ( int p , int q ,
             int *tbl , float r )
{
        int s;
        float sum = 0.0;

        for ( s = 0 ; s <= ( p - q ) ; ++s , ++tbl )
                sum += ((float)(*tbl)) *
                        pow ( r , (float)( p - s ) );

        return sum;
}

static int*
_get_fact_coeffs ( int p, int q)
{
	double a, b, c, d, e;
	int s;
	int *tbl = NULL;
	int *idx1;

	tbl = (int*) calloc( p - q + 1 , sizeof(int));
	if (tbl == NULL)
		return MIG_ERROR_MEMORY;
	
	a = -1.0;
	idx1 = tbl;
	for ( s = 0 ; s <= p - q ; ++s , ++idx1 )
	{
		a = -a;
		b = fact ( 2 * p + 1 - s );
		c = fact ( s );
		d = fact ( p + q + 1 - s );
		e = fact ( p - q - s );

		*idx1 = (int) ((a * b) / (c * d * e));
	}
	

	return tbl;
}

/***********************************/
/*TODO: see if necessary to use faster algorithms (a couple available)
best candidate for us: "poor man's" that doesn't need big integers */

static double
fact ( int n )
{
        double x = 1.0;

        while ( n > 1 )
                x *= n--;

        return x;
}
