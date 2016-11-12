/*********************************************************/
/* svmsavemodel.c                                        */
/* Author: Gianluca Ferri  ferri_gianluca@virgilio.it    */ 
/* (stealing almost all of the code from svmpredict.c)   */
/*                                                       */
/* Created on Oct 26, 2007                               */
/*********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "svm.h"

#include "mex.h"
#include "svm_model_matlab.h"

#if MX_API_VER < 0x07030000
typedef int mwIndex;
#endif 

#define CMD_LEN 2048

/* Until we get something more portable and effective */
#if !defined(MAX_PATH)
	#define MAX_PATH 1024
#endif
#define FNAME_LEN MAX_PATH


static void fake_answer(mxArray *plhs[])
{
	plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
	*mxGetPr(plhs[0]) = -1;
}

void exit_with_help()
{
	mexPrintf(
		"Usage: [status] = svmsavemodel(model, filename)\n"
	);
}

void mexFunction( int nlhs, mxArray *plhs[],
		 int nrhs, const mxArray *prhs[] )
{
	//int prob_estimate_flag = 0;
	struct svm_model *model;

	int status = 0;
	char model_fname[FNAME_LEN];
	int fname_length = 0;

	if(nrhs != 2)
	{
		exit_with_help();
		fake_answer(plhs);
		return;
	}

	if(mxIsStruct(prhs[0]))
	{
		const char *error_msg;
		
		//load svm model from matrix
		model = (struct svm_model *) malloc(sizeof(struct svm_model));
		error_msg = matlab_matrix_to_model(model, prhs[0]);
		if(error_msg)
		{
			mexPrintf("Error: can't read model: %s\n", error_msg);
			svm_destroy_model(model);
			fake_answer(plhs);
			return;
		}
		
		//get file name from parameters
		//add checks!!
		
		fname_length = mxGetN(prhs[1]);
		if(fname_length <= 0)
		{
			mexPrintf("Error: zero-length filename");
			svm_destroy_model(model);
			fake_answer(plhs);
			return;
		}

		status = mxGetString(prhs[1], model_fname,  fname_length + 1);
		mexPrintf("%d\n", status);
		if (status)
		{
			mexPrintf("Error: second argument must be a String");
			svm_destroy_model(model);
			fake_answer(plhs);
			return;
		}

		//save to model file
		status = svm_save_model(model_fname, model);
		
		// destroy model
		svm_destroy_model(model);

		plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
		*mxGetPr(plhs[0]) = status;
	}
	else
	{
		mexPrintf("model file should be a struct array\n");
		fake_answer(plhs);
	}

	return;
}
