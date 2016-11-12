/*
******************************************************************************
*
* Author      : Todor Petkov
* Filename    : libmigsvm.h
* Created     : 2007/06/21
* Description : support vector machine functions
*
******************************************************************************
*/

#ifndef __LIBMIG_SVM_H__
#define __LIBMIG_SVM_H__

#include "mig_config.h"
#include "mig_defs.h"
#include "mig_error_codes.h"

#ifdef __cplusplus
extern "C" {
#endif

/* kernel function */
typedef double (*mig_svm_kernel_f) ( const float * , const float * , float , float , float , int );

/* svm type */
typedef enum
{
    MIG_C_SVC

} MIG_SVM_TYPE;

/* kernel type */
typedef enum
{
    MIG_LINEAR ,
    MIG_POLY ,
    MIG_RBF , 
    MIG_SIGMOID

} MIG_SVM_KERNEL_TYPE;

/* single instance ( example ) to classify */

typedef struct _mig_svm_example_t
{
	int     label;      /* svm predicted label */
	int     len;        /* number of features in current example */
    float   *feat;      /* features vector */

} mig_svm_example_t;

/* support only 2 class classification */
typedef struct _mig_svm_t
{
    MIG_SVM_TYPE                type_svm;       /* svm type */
	MIG_SVM_KERNEL_TYPE         type_kernel;    /* svm kernel type */
    mig_svm_kernel_f            kernel_f;

    int     deg;                /* degree for polynomial kernel */
	float   gamma;              /* gamma for poly/rbf */
	float   coef0;              /* coef0 for poly */

	int     num_sv;		        /* total number of support vectors */
	int     len_sv;             /* support vector lenght */
	float   **sv;		        /* support vectors */
	float   *sv_coef;	        /* coefficients for SVs in decision functions */
	float   rho;		        /* constants in decision functions */

	int     labels[2];		    /* label to assign to each class */
	int     num_sv_class[2];	/* number of support vectors for each class */

} mig_svm_t;


/* scaling parameters */
typedef struct _mig_svm_scale_t
{
    int len;
    float *mean;
    float *std;

} mig_svm_scale_t;


/*
******************************************************************************
*                               PROTOTYPES
******************************************************************************
*/

/*
******************************************************************************
*               LOAD SVM MODEL FROM FILE
*
* Description : This function loads an svm model as saved by libsvm from file.
*
*
* Arguments   : model_file_name - filename ( full path ).
*               model           - loaded model structure ( preallocated ).
*
* Returns     : MIG_OK on success
*               MIG_ERROR_UNSUPPORTED if model could not be loaded.
*
******************************************************************************
*/

int
mig_svm_model_load ( const char *model_file_name , mig_svm_t *model );

/*
******************************************************************************
*               FREE SVM MODEL
*
* Description : This function frees svm model structure.
*
* Arguments   : model - model structure to free.
*
* Notes : Only data internal to the structure is freed. The structure should be
*         freed explicitly using free.
* 
******************************************************************************
*/

void
mig_svm_model_free ( mig_svm_t *model );

/*
******************************************************************************
*               LOAD SVM SCALING (NORMALIZATION) PARAMETERS FROM FILE
*
* Description : This function loads svm scaling parameters from file.
*
* Arguments   : scale_params_file_name - filename ( full path ).
*               scale_params           - loaded params structure ( preallocated ).
*
* Returns     : MIG_OK                  on success.
*               MIG_ERROR_PARAM         if one of the input parameters is NULL.
*               MIG_ERROR_IO            if params file could not be opened for reading
*                                       or a line of text could not be read from the file.
*               MIG_ERROR_UNSUPPORTED   if there is a syntax error in the file.
*               MIG_ERROR_MEMORY        if there was a memory error.
*
* Notes : The maximum file line lenght that can be parsed is fixed by 
*         NORM_LINE_LEN  =  (10000+1). If the number of entries inside the file is
*         greater than there will be a truncation !
*
******************************************************************************
*/

int
mig_svm_scale_params_load ( const char *scale_params_file_name , mig_svm_scale_t *scale_params );


/*
******************************************************************************
*               WRITE SVM SCALING (NORMALIZATION) PARAMETERS TO FILE
*
* Description : This function write svm scaling parameters to file.
*
* Arguments   : scale_params_file_name - filename ( full path ).
*               scale_params           - params structure to dump.
*
* Returns     : MIG_OK                  on success.
*               MIG_ERROR_PARAM         if one of the input parameters is NULL.
*               MIG_ERROR_IO            if params file could not be opened for writing
*                                       or a line of text could not be written on the file.
*               MIG_ERROR_MEMORY        if there was a memory error.
*
* Notes : The maximum file line lenght that can be parsed is fixed by 
*         NORM_LINE_LEN  =  (10000+1). If the number of entries inside the file is
*         greater than there will be a truncation !
*
******************************************************************************
*/

int
mig_svm_scale_params_write ( const char *scale_params_file_name , mig_svm_scale_t *scale_params );


/*
******************************************************************************
*               FREE SVM NORMALIZATION PARAMS
*
* Description : This function frees svm normalization params structure.
*
* Arguments   : scale_params - normalization params structure to free.
*
* Notes : Only data internal to the structure is freed. The structure should be
*         freed explicitly using free.
* 
******************************************************************************
*/

void
mig_svm_scale_params_free ( mig_svm_scale_t *scale_params );


/*
******************************************************************************
*               PREDICT SVM LABELS FOR A VECTOR OF DATA
*
* Description : This function predicts ( assignes ) a label for a vector of features.
*
* Arguments   : svm - support vector machine structure.
*               x   - current example.
*
* Returns     : MIG_OK                  on success.
*               MIG_ERROR_UNSUPPORTED   if number of features in vector x is different from
*                                       number of features defined for current svm.
* 
* Notes : The type of kernel used for the label prediction is registered inside the svm
*         structure.
*         The final label assigned to x is stored inside x->label (integer).
*         The value of the final label assigned to x if read from svm->labels.
*
******************************************************************************
*/

int
mig_svm_predict ( const mig_svm_t *svm , mig_svm_example_t *x );

/*
 ****************************************************************************
 * Allocate scaling structure with feat_len length
 *
 ****************************************************************************
 */
mig_svm_scale_t*
mig_svm_scale_alloc ( int feat_len );

/*
 ****************************************************************************
 * Free scaling structure
 *
 ****************************************************************************
 */

void
mig_svm_scale_free ( mig_svm_scale_t* scales );

/*
 ****************************************************************************
 * zero scaling structure values
 *
 ****************************************************************************
 */

void
mig_svm_scale_zero ( mig_svm_scale_t* scales );




#ifdef __cplusplus
}
#endif
#endif /* __LIBMIG_SVM_H__ */
