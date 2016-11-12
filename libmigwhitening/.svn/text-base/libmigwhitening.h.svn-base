/*
******************************************************************************
*
* Author      : Gianluca Ferri
* Filename    : libmigwhitening.h
* Created     : 2010/08/20
* Description : C INTERFACE FOR WHITENING HANDLING
*
*
******************************************************************************
*/




#ifndef __LIBMIG_WHITENING_H__
#define __LIBMIG_WHITENING_H__

#include "mig_config.h"
#include "mig_defs.h"
#include "mig_data_types.h"
#include "mig_error_codes.h"

/* FOR C++->C WRAPPING */
#ifdef __cplusplus
	#include "eigenwhitener.h"	
#else
	typedef struct EigenWhitener EigenWhitener;
#endif

MIG_C_LINKAGE_START

int mig_whitening_compute( EigenWhitener* whitener, float** feat_mat, int nfeat, int nfeatvec );

int mig_whitening_apply( EigenWhitener* whitener, float* in_vec, float* out_vec );

int mig_whitening_load( EigenWhitener* whitener, char* fname );


int mig_whitening_save ( EigenWhitener* whitener, char* fname );


MIG_C_LINKAGE_END

#endif /* __LIBMIG_WHITENING_H__ */