/*
******************************************************************************
*
* Author      : Todor Petkov
* Filename    : mig_kernel.c
* Created     : 2007/06/21
* Description : Convolution kernel definition and building
*
******************************************************************************
*/

#include "mig_im_kernel.h"

/*
******************************************************************************
*               GLOBAL PROTOTYPES IMPLEMENTATION
******************************************************************************
*/

mig_kernel_t*
mig_im_kernel_get_log_1d ( float sigma )
{
        mig_kernel_t *kernel;
        float sigma2 , sigma2over;
        float sum = 0.0f;
        int i;

        if ( sigma <= 0.0 )
                return NULL;

        kernel = (mig_kernel_t*)
                calloc ( 1 , sizeof(mig_kernel_t) );
        if ( kernel == NULL )
                return NULL;

        kernel->r = (int)( 3.0f * sigma + 0.5f );
        if ( kernel->r == 0 )
                kernel->r = 1;
        kernel->d = 2 * kernel->r + 1;
        kernel->n = kernel->d;
        kernel->sigma  = sigma;
        kernel->scale  = sigma * MIG_SQRT2;
        kernel->border = BORDER_REFLECT;
        kernel->data = (float*)
                malloc ( kernel->d * sizeof(float) );
        if ( kernel->data == NULL )
        {
                free ( kernel );
                return NULL;
        }
        kernel->center = kernel->data + kernel->r;

        /* populate kernel */
        sigma2 = MIG_POW2( sigma );
        sigma2over = 1.0f / sigma2;

        /* exponential part */
        for ( i = -kernel->r ; i <= kernel->r ; ++i )
        {
                kernel->center[i] = expf( -((float)MIG_POW2(i)) * 0.5f * sigma2over );
                sum += kernel->center[i];
        }

        /* normalize exponential part */
        if ( sum != 0.0f )
        {
                sum = 1.0f / sum;

                for( i = 0 ; i < kernel->n ; ++i )
                        kernel->data[i] *= sum;
        }

        /* now multiplier */
        sum = 0.0f;
        for ( i = -kernel->r ; i <= kernel->r ; ++i )
        {
                kernel->center[i] *= ( -1.0f + (float)MIG_POW2(i) * sigma2over );
                sum += kernel->center[i];
        }

        sum /= kernel->n;

        for ( i = 0 ; i < kernel->n ; ++i )
                kernel->data[i] -= sum;

        return kernel;
}

/*****************************************************************************/

mig_kernel_t*
mig_im_kernel_get_log_2d ( float sigma )
{
    mig_kernel_t *log;
    float sigma2 , sigma4 , norm = 0.0f;
    float *idx;
    int i , j;

    log = (mig_kernel_t*)
        calloc ( 1 , sizeof( mig_kernel_t ) );
    if ( log == NULL )
        return NULL;

    log->sigma = sigma;
    log->scale = 2.0f * MIG_SQRT2 * sigma;
    log->d = (int) ( 3.0f * log->scale );

    if ( ( log->d % 2 ) == 0 )
            log->d ++;
    log->r = (int) ( log->d * 0.5f );
    log->n = log->d * log->d;

    log->data = (float*)
        malloc ( log->n * sizeof(float) );
    if ( log->data == NULL )
    {
        free ( log );
        return NULL;
    }

    idx = log->data + log->r + log->r * log->d;
    sigma2 = MIG_POW2( sigma );
    sigma4 = MIG_POW2( sigma2 );

    /* calculate exponent */
    for ( j = -log->r ; j <= log->r ; ++j )
    {
        for ( i = -log->r ; i <= log->r ; ++i )
        {
            idx[i+j*log->d] = expf( ( -((float)(i*i+j*j)) * 0.5f ) / sigma2 );
            norm += idx[i+j*log->d];
        }
    }

    /* normalize */
    if ( norm != 0.0f )
    {
        norm = 1.0f / norm;
        for ( i = 0 ; i < log->n ; ++i )
                log->data[i] *= norm;
    }

    /* calculate rest */
    norm = 0.0f;

    for ( j = -log->r ; j <= log->r ; ++j )
    {
        for ( i = -log->r ; i <= log->r ; ++i )
        {
            idx[i+j*log->d] *= ( ( (float)( i*i + j*j ) ) - 2.0f * sigma2 ) / sigma4;
            norm += idx[i+j*log->d];
        }
    }

    /* normalize */
    norm /= (float)( log->n );

    for ( i = 0 ; i < log->n ; ++i )
            log->data[i] -= norm;

    return log;
}



/*****************************************************************************/

/* hat is negative! */
mig_kernel_t*
mig_im_kernel_get_log_3d ( float sigma )
{
	mig_kernel_t *log;
    float sigma2 , sigma4 , norm = 0.0f;
	float abs_sum = 0.0f;
    float *idx;
    int i , j, k;
	int offset;

    log = (mig_kernel_t*)
        calloc ( 1 , sizeof( mig_kernel_t ) );
    if ( log == NULL )
        return NULL;

    log->sigma = sigma;
    log->scale = 2.0f * MIG_SQRT2 * sigma;
    log->d = (int) ( 3.0f * 2.0f * log->sigma );

    if ( ( log->d % 2 ) == 0 )
            log->d ++;
    log->r = (int) ( log->d * 0.5f );
	log->n = MIG_POW3(log->d);

    log->data = (float*)
        malloc ( log->n * sizeof(float) );
    if ( log->data == NULL )
    {
        free ( log );
        return NULL;
    }

    idx = log->data + log->d * log->d * log->r   + log->r + log->r * log->d;
    sigma2 = MIG_POW2( sigma );
    sigma4 = MIG_POW2( sigma2 );

    /* calculate exponent */
    for ( k = -log->r; k <= log->r ; ++k )
	{
		for ( j = -log->r ; j <= log->r ; ++j )
		{
			for ( i = -log->r ; i <= log->r ; ++i )
			{
				idx[i+ j*log->d + k * log->d * log->d ] = expf( ( -((float)(i*i+j*j+k*k)) * 0.5f ) / sigma2 );
				//norm += idx[i+j*log->d+ k * log->d * log->d ];
			}
		}
	}


	/*why this now? */
    /* normalize */
    //if ( norm != 0.0f )
    //{
    //    norm = 1.0f / norm;
    //    for ( i = 0 ; i < log->n ; ++i )
    //            log->data[i] *= norm;
    //}

    /* calculate rest */
    norm = 0.0f;
	offset = 0;
	for ( k =  -log->r ; k <= log->r ; ++k )
	{
		for ( j = -log->r ; j <= log->r ; ++j )
		{
			for ( i = -log->r ; i <= log->r ; ++i )
			{
				offset = i+j*log->d + + k * log->d * log->d;
				idx[offset] *=  ( ( (float)( i*i + j*j +k*k) ) - 3.0f * sigma2 ) / sigma4 ;
				/*norm is in such a way that sum of all terms is zero and maximum response is always 1*/
				norm += idx[offset];
				
			}
		}
	}

    /* normalize */
    /*norm /= (float)( log->n );*/
	abs_sum = 0.0f;
    for ( i = 0 ; i < log->n ; ++i )
	{
		log->data[i] -= (norm / log->n);
		abs_sum += MIG_ABS(log->data[i]);
	}
	
	for (i = 0; i < log->n ; ++i )
	{
		log->data[i] /= abs_sum;
	}

    return log;
}

/*****************************************************************************/
void
mig_im_kernel_delete ( mig_kernel_t *kernel )
{
        if ( kernel != NULL )
        {
                if ( kernel->data != NULL )
                        free ( kernel->data );
        
				free ( kernel );
		}
}


mig_kernel_t*
mig_im_kernel_get_mean_1d ( int radius )
{
        mig_kernel_t *kernel;
        int i;
		float norm_val;

        if ( radius < 1 )
                return NULL;

        kernel = (mig_kernel_t*)
                calloc ( 1 , sizeof(mig_kernel_t) );
        if ( kernel == NULL )
                return NULL;

        kernel->r = radius;
        kernel->d = 2 * kernel->r + 1;
        kernel->n = kernel->d;
        kernel->sigma  = 0.;
        kernel->scale  = 0.;
        kernel->border = BORDER_REFLECT;
        kernel->data = (float*)
                malloc ( kernel->d * sizeof(float) );
        if ( kernel->data == NULL )
        {
                free ( kernel );
                return NULL;
        }
        kernel->center = kernel->data + kernel->r;

		norm_val = 1.0 / kernel->d;
        /* populate kernel */
        for ( i = 0 ; i < kernel->d ; ++i )
        {
			kernel->data[i] = norm_val ;
        }

        return kernel;
}