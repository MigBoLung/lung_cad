#include "libmigsvm.h"
#include "svm.h"

/******************************************************************/
/* PRIVATE PROTOTYPES */
/******************************************************************/

static double 
_dot ( const float *sv , const float *x , int len );

static double 
_squared_norm ( const float *sv , const float *x , int len );

static double
_f_lin  ( const float *sv , const float *x , float param1 , float param2 , float param3 , int len );

static double 
_f_poly ( const float *sv , const float *x , float param1 , float param2 , float param3 , int len );

static double 
_f_rbf  ( const float *sv , const float *x , float param1 , float param2 , float param3 , int len );

static double 
_f_sig  ( const float *sv , const float *x , float param1 , float param2 , float param3 , int len );

/******************************************************************/
/* EXPORTS */
/******************************************************************/

int
mig_svm_model_load ( const char *model_file_name , mig_svm_t *model )
{
    int i , j , max_idx = 1;
    struct svm_model *tmp_model;
    struct svm_node *curr;
    
    tmp_model = svm_load_model ( model_file_name );
    if ( tmp_model == NULL )
        return MIG_ERROR_UNSUPPORTED;

    /* zero our model structure */
    memset ( model , 0x00 , sizeof(mig_svm_t) ); 
    
    /* verify if we support this kind of svm */
    if ( (tmp_model->param).svm_type != C_SVC )
        goto error;

    /* verify if we support this kind of kernel */
    if ( tmp_model->param.kernel_type == PRECOMPUTED )
        goto error;
    
    /* we only support 2 class classification problems */
    if ( tmp_model->nr_class != 2 )
        goto error;
    
    /* start copying data into our structure */
    model->type_svm        = MIG_C_SVC;
    model->type_kernel     = (MIG_SVM_KERNEL_TYPE) tmp_model->param.kernel_type;
    switch (model->type_kernel)
    {
        case MIG_LINEAR :
            
                model->kernel_f = &_f_lin;
                break;
        
        case MIG_POLY :
            
                model->kernel_f = &_f_poly;
                break;

        case MIG_RBF :
            
                model->kernel_f = &_f_rbf;
                break;

        case MIG_SIGMOID :
            
                model->kernel_f = &_f_sig;
                break;

        default :
                goto error;        
    }

    model->deg             = tmp_model->param.degree;
    model->gamma           = tmp_model->param.gamma;
    model->coef0           = tmp_model->param.coef0;
    model->rho             = tmp_model->rho[0];
    model->num_sv          = tmp_model->l;        
    model->labels[0]       = tmp_model->label[0];
    model->labels[1]       = tmp_model->label[1];
	model->num_sv_class[0] = tmp_model->nSV[0];
    model->num_sv_class[1] = tmp_model->nSV[1];

    for ( i = 0 ; i < model->num_sv ; ++i )
    {
        j = 0;
        curr = tmp_model->SV[i];
        while ( curr->index != -1 )
        {
            if ( curr->index > max_idx )
                max_idx = curr->index;
            ++j;
            curr = tmp_model->SV[i]+j;
        }
    }
	
	/* was this, but it's wrong as of libsvm 2.91! */
    /* model->len_sv = max_idx;*/
	model->len_sv = max_idx +1;

    /* allocate memory for support vector coefficients */
    model->sv_coef = (float*) calloc ( model->num_sv , sizeof(float) );
    if ( model->sv_coef == NULL )
        goto error;

    /* allocate memory for support vectors */
    model->sv = (float**) calloc ( model->num_sv , sizeof(float*) );
    if ( model->sv == NULL )
        goto error;


    /* start filling in support vector coefficients and support
       vecto values */
    for ( i = 0 ; i < model->num_sv ; ++i )
    {
        /* copy coeff */
        model->sv_coef[i] = tmp_model->sv_coef[0][i];

        /* allocate space for sv */
        model->sv[i] = (float*) calloc ( model->len_sv , sizeof(float) );
        if ( model->sv[i] == NULL )
            goto error;
        
        /* copy sv */
        j = 0;
        curr = tmp_model->SV[i];
        while ( curr->index != -1 )
        {
			/* update! index starts from 0
            model->sv[i][curr->index-1] = curr->value;
			*/
			model->sv[i][curr->index] = curr->value;
            ++j;
            curr = tmp_model->SV[i]+j;
        }
    }

	svm_free_and_destroy_model ( &tmp_model );
    return MIG_OK;

error :
    
    svm_free_and_destroy_model ( &tmp_model );
    return MIG_ERROR_UNSUPPORTED;
}

/******************************************************************/

void
mig_svm_model_free ( mig_svm_t *model )
{
    int i;

    if ( model->sv_coef )
        free ( model->sv_coef );    
    
    for ( i = 0 ; i < model->num_sv ; ++ i )
        if ( model->sv[i] )
            free ( model->sv[i] );

    free ( model->sv );
}

/******************************************************************/

#define NORM_LINE_LEN    (10000+1)

int
mig_svm_scale_params_load ( const char *scale_params_file_name , 
                            mig_svm_scale_t *scale_params )
{
    int rc = MIG_OK;
    FILE *f = NULL;
    char line[NORM_LINE_LEN];
    char *idx;
    int id;
    float val;
    int parse_error_f = 0;
    
    if ( ( scale_params_file_name == NULL ) || ( scale_params == NULL ) )
         return MIG_ERROR_PARAM;

    /* zero input */
    memset ( scale_params , 0x00 , sizeof( mig_svm_scale_t ) ); 

    f = fopen ( scale_params_file_name , "r" );
    if ( f == NULL )
        return MIG_ERROR_IO;

    /* read number of coeffs */
    if ( fgets ( (char*) &line , NORM_LINE_LEN , f ) == NULL )
    {
        rc = MIG_ERROR_IO;
        goto error;
    }

    /* parse line to get number of coeffs */
    if ( sscanf ( (const char*) &line , "%d\n" , &( scale_params->len ) ) != 1 )
    {
        rc = MIG_ERROR_UNSUPPORTED;
        goto error;
    }

    /* load first line : mean values (float) */
    scale_params->mean = (float*) calloc ( scale_params->len , sizeof( float ) );
    if ( scale_params->mean == NULL )
    {
        rc = MIG_ERROR_MEMORY;
        goto error;
    }

    /* get one line from file : we are going to parse one line at a time */
    if ( fgets ( (char*) &line , NORM_LINE_LEN , f ) == NULL )
    {
        rc = MIG_ERROR_IO;
        goto error;
    }

    /* parse mean line one entry at a time */
    idx = line; 
    while ( parse_error_f == 0 )
    {
        /* try reading id:val from line */
        if ( ( sscanf ( idx , "%d:%f" , &id , &val ) == 0 ) ||
             ( id > scale_params->len ) )
        { 
            parse_error_f = 1;
            break;
        }

        /* if successful store val */
        scale_params->mean[id-1] = val;

        /* advance current line index while
           current char is different from space ( 32 ) and
           new-line ( 10 ) */
        while ( ( *idx != 32 ) && ( *idx != 10 ) )
        {
            /* if current char is different from
               . : digit ( 0->9 ) than we have a syntax error */
            if ( ( *idx != 46 ) &&
                 ( *idx != 58 ) &&
                 ( ( *idx < 48 ) || ( *idx > 57 ) ) )
            {
                parse_error_f = 1;
                break;
            }
            /* else advance one more char */
            else
            {
                ++idx;
            }
        }
        
        /* if last char read was space we have more entries to read :
           eat trailing space */
        if ( *idx == 32 )
            ++idx;

        /* if last char read was new line : break from line reading cycle */
        if ( *idx == 10 )
            break;    
    }
    
    /* if while parsing we found an error or if the number of entries is different
       from the number delcared exit */
    if ( ( parse_error_f == 1 ) || ( id != scale_params->len ) )
    {
        rc = MIG_ERROR_UNSUPPORTED;
        goto error;
    }
    
    /* load second line : std values (float) */
    scale_params->std = (float*) calloc ( scale_params->len , sizeof( float ) );
    if ( scale_params->std == NULL )
    {
        rc = MIG_ERROR_MEMORY;
        goto error;
    }

    if ( fgets ( (char*) &line , NORM_LINE_LEN , f ) == NULL )
    {
        rc = MIG_ERROR_IO;
        goto error;
    }

    /* parse mean line one entry at a time */
    idx = line; 
    while ( parse_error_f == 0 )
    {
        if ( ( sscanf ( idx , "%d:%f" , &id , &val ) == 0 ) ||
             ( id > scale_params->len ) )
        { 
            parse_error_f = 1;
            break;
        }

        scale_params->std[id-1] = val;

        while ( ( *idx != 32 ) && ( *idx != 10 ) )
        {
            /* if current char is different from
               . : digit ( 0->9 ) than we have a syntax error */
            if ( ( *idx != 46 ) &&
                 ( *idx != 58 ) &&
                 ( ( *idx < 48 ) || ( *idx > 57 ) ) )
            {
                parse_error_f = 1;
                break;
            }
            /* else advance one more char */
            else
            {
                ++idx;
            }
        }

        if ( *idx == 32 )
            ++idx;

        if ( *idx == 10 )
            break;    
    }
    
    if ( ( parse_error_f == 1 ) || ( id != scale_params->len ) )
    {
        rc = MIG_ERROR_UNSUPPORTED;
        goto error;
    }

    fclose ( f );
    return MIG_OK;

error :

    if ( f )
        fclose ( f );

    if ( scale_params->mean )
    {
        free ( scale_params->mean );
        scale_params->mean = NULL;
    }

    if ( scale_params->std )
    {
        free ( scale_params->std );
        scale_params->std = NULL;
    }

    return rc;
}

/*******************************************************/

int
mig_svm_scale_params_write ( const char *scale_params_file_name ,
							mig_svm_scale_t *scale_params )
{
	int rc;

	int i;

	FILE *fp;
	fp = fopen ( scale_params_file_name, "w" );
	if ( fp == NULL )
		return MIG_ERROR_IO;

	/* write len */
	fprintf ( fp , "%d\n", scale_params->len );

	/* write mean */
	for ( i = 0; i != scale_params->len; ++i)
	{
		fprintf ( fp , "%d:%f " , i + 1 , scale_params->mean[i] );
	}
	fprintf ( fp, "\n" );

	/* write std */
	for ( i = 0; i != scale_params->len; ++i)
	{
		fprintf ( fp , "%d:%f " , i + 1 , scale_params->std[i] );
	}
	fprintf ( fp, "\n" );
	
	
	rc = fclose ( fp );
	if ( rc == EOF )
		return MIG_ERROR_IO;

	return MIG_OK;
}

/******************************************************************/

void
mig_svm_scale_params_free ( mig_svm_scale_t *scale_params )
{
    if ( scale_params )
    {
        if ( scale_params->mean )
            free ( scale_params->mean );

        if ( scale_params->std )
            free ( scale_params->std );

        memset ( scale_params , 0x00 , sizeof( mig_svm_scale_t ) );
    }
}

/******************************************************************/


int
mig_svm_predict ( const mig_svm_t *svm , mig_svm_example_t *x )
{
    int i;
    double sum = 0.0f;

    if ( x->len != (svm->len_sv))
        return MIG_ERROR_UNSUPPORTED;

    for ( i = 0 ; i < svm->num_sv ; ++ i )
    {
        sum += ( svm->sv_coef[i] ) * svm->kernel_f ( svm->sv[i] , x->feat , svm->gamma , svm->coef0 , (float)svm->deg , svm->len_sv );
    }
	
	/*GF: updated to libsvm 2.91 (- instead of +)*/
    sum = MIG_SGN( sum - svm->rho );
	/*sum = MIG_SGN( sum + svm->rho );*/
    x->label = ( sum == 1 ) ? ( svm->labels[0] ) : ( svm->labels[1] );
    return MIG_OK;
}

/******************************************************************/
/* PRIVATE IMPLEMENTATIONS */
/******************************************************************/

static double
_dot ( const float *sv , const float *x , int len )
{
    int i;
    double dot = 0.0;

    for ( i = 0 ; i < len ; ++i )
        dot += sv[i] * x[i];
    return dot;
}

/******************************************************************/

static double
_squared_norm ( const float *sv , const float *x , int len )
{
    int i;
    double res = 0.0;

    for ( i = 0 ; i < len ; ++i )
        res += MIG_POW2( sv[i] - x[i] );
    return res;
}

/******************************************************************/

static double
_f_lin ( const float *sv , const float *x , float param1 , float param2 , float param3 , int len )
{
    double res;
    res = _dot ( sv , x , len );
    return res;
}

/******************************************************************/

static double
_f_poly ( const float *sv , const float *x , float param1 , float param2 , float param3 , int len )
{
    double res;
    res = _dot ( sv , x , len );
    res *= (double) param1;
    res += (double) param2;
    res = pow( res , (double) param3 ); 
    return res;
}

/******************************************************************/

static double
_f_rbf ( const float *sv , const float *x , float param1 , float param2 , float param3 , int len )
{
    double res;
    res = _squared_norm ( sv , x , len );
    res *= -(double) param1;
    res = exp( res ); 
    return res;
}

/******************************************************************/

static double
_f_sig ( const float *sv , const float *x , float param1 , float param2 , float param3 , int len )
{
    double res;
    res = _dot ( sv , x , len );
    res *= (double) param1;
    res += (double) param2;
    res = tanh( res ); 
    return res;    
}


/*
 ****************************************************************************
 * Allocate scaling structure with feat_len length
 *
 ****************************************************************************
 */
mig_svm_scale_t*
mig_svm_scale_alloc ( int feat_len )
{
	mig_svm_scale_t* scales =
		(mig_svm_scale_t*) calloc (1, sizeof(mig_svm_scale_t) );
	if ( scales == NULL )
		return NULL;

	scales->len = feat_len;
	
	scales->mean = NULL;
	scales->std = NULL;

	scales->mean = (float*) calloc ( feat_len, sizeof(float) );
	if ( scales->mean == NULL )
	{
		free ( scales );
		return NULL;
	
	}

	scales->std = (float*) calloc ( feat_len, sizeof(float) );
	if ( scales->std == NULL )
	{
		free ( scales->mean );
		free ( scales );
		return NULL;
	}

	mig_svm_scale_zero ( scales );
	return scales;
}

/*
 ****************************************************************************
 * Free scale structure
 *
 ****************************************************************************
 */

void
mig_svm_scale_free ( mig_svm_scale_t* scales )
{
	if ( scales )
	{
		if ( scales->std )
			free ( scales->std );
		
		if ( scales->mean )
			free ( scales->mean);
		
		free ( scales );
	}
}

void
mig_svm_scale_zero ( mig_svm_scale_t* scales )
{
	int i;
	float *pmean;
	float *pstd;
	if ( scales && scales->mean && scales->std )
	{
		pmean = scales->mean;
		pstd  = scales->std;
		for (i = 0; i != scales->len; ++i)
		{
			*pmean++ = 0;
			*pstd++ = 0;
		}
	}
}
