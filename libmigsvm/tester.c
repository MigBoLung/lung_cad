#include "mig_config.h"
#include "mig_defs.h"
#include "mig_error_codes.h"

#include "libmigsvm.h"

static float feat1[] = { 0.708333f , 1.0f , 1.0f , -0.320755f , -0.105023f , -1.0f , 1.0f , -0.419847f , -1.0f , -0.225806f , 1.0f , -1.0f };
static float feat2[] = { 0.583333f , -1.0f , 0.333333f , -0.603774f , 1.0f , -1.0f , 1.0f , 0.358779f , -1.0f , -0.483871f , -1.0f , 1.0f };
static mig_svm_example_t x = { 0 , 12 , NULL };

int
main ( int argc , char **argv )
{
    mig_svm_t model;
    int i , j , rc;
    float **sv2;
    float f = 1.0f;

    printf ( "\nLoading model..." );
    rc = mig_svm_model_load ( argv[1] , &model );
    printf ( "\nreturn code was : %d " , rc );

    printf ( "\nDumping model model..." );
    printf ( "\nSVM type      : %d" , (int)( model.type_svm ) );
    printf ( "\nKERNEL type   : %d" , (int)( model.type_kernel ) );
    printf ( "\nDEGREE        : %d" , model.deg );
    printf ( "\nGAMMA         : %f" , model.gamma );
    printf ( "\nCOEF0         : %f" , model.coef0 );
    printf ( "\nNUM SVs       : %d" , model.num_sv );
    printf ( "\nLENGTH OF SVs : %d" , model.len_sv );
    printf ( "\nRHO           : %f\n" , model.rho );
    printf ( "----------------------------------");
    printf ( "\nCLASS 1    : ");
    printf ( "\nLABEL      : %d" , model.labels[0] );
    printf ( "\nNUM SVs    : %d" , model.num_sv_class[0] );
    printf ( "\nSVs COEFFS : " );
    for ( i = 0 ; i < model.num_sv_class[0] ; ++i )
        printf( " %f " , model.sv_coef[i] );
    printf( "\nSVs        : " );
    for ( i = 0 ; i < model.num_sv_class[0] ; ++i )
    {
        for ( j = 0 ; j < model.len_sv ; ++j )
            printf ( " %f " , model.sv[i][j] );
        printf ( "\n" );
    }
    printf ( "\n----------------------------------");
    printf ( "\nCLASS 2    : ");
    printf ( "\nLABEL      : %d" , model.labels[1] );
    printf ( "\nNUM SVs    : %d" , model.num_sv_class[1] );
    printf ( "\nSVs COEFFS : " );
    for ( i = 0 ; i < model.num_sv_class[1] ; ++i )
        printf( " %f " , model.sv_coef[model.num_sv_class[0]+i] );
    printf( "\nSVs        : " );
    sv2 = model.sv + model.num_sv_class[0];
    for ( i = 0 ; i < model.num_sv_class[1] ; ++i )
    {
        for ( j = 0 ; j < model.len_sv ; ++j )    
            printf ( " %f " , sv2[i][j] );
        printf ( "\n" );
    }
 
    printf ( "\nPredicting labels..." );
    x.feat = &feat1;
    rc = mig_svm_predict ( &model , &x );
    printf ( "\n 1. Return code was : %d , label was : %d" , rc , x.label );

    x.feat = &feat2;
    rc = mig_svm_predict ( &model , &x );
    printf ( "\n 2. Return code was : %d , label was : %d" , rc , x.label );

    printf ( "\nFreeing model..." );
    mig_svm_model_free ( &model );

    exit( EXIT_SUCCESS );
}
