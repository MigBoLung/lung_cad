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



#include "libmigwhitening.h"
#include "eigenwhitener.h"


int mig_whitening_compute( EigenWhitener* whitener, float** feat_mat, int nfeat, int nfeatvec )
{
	return whitener->compute( const_cast < const float **const >(feat_mat), nfeat, nfeatvec);
}

int mig_whitening_apply( EigenWhitener* whitener, float* in_vec, float* out_vec )
{
	return whitener->apply( in_vec, out_vec );
}

int mig_whitening_load( EigenWhitener* whitener, char* fname )
{
	return whitener->load( fname ) ;
}

int mig_whitening_save ( EigenWhitener* whitener, char* fname )
{
	return whitener->save( fname );
}
