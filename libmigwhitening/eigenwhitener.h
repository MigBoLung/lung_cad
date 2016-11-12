#ifndef __EIGENWHITENER_H__
#define __EIGENWHITENER_H__



//implementation of whitening using Eigen library


#include <vector>

#include <Eigen/Dense>


class EigenWhitener
{
public:
	EigenWhitener();

	//ok default constructors and assignments for now (no new and deletes)
	//EigenWhitener( const EigenWhitener& );

	//EigenWhitener& operator=(EigenWhitener& );
	
	/**
	 ************************************************
	 *  compute eigenvectors and eigenvalues from feature matriz
	 ************************************************
	 */
	int compute( const float** const feat_mat, const int nfeat, const int nfeatvec );

	int load ( const char* const fname );

	int save ( const char* const fname );

	int apply ( const float* const in_vec, const float* out_vec);
	

private:
	int feat_num;
	Eigen::MatrixXf inv_sqrt_eigenvalues;
	Eigen::MatrixXf eigenvectors;
};

#endif /* __EIGENWHITENER_H__*/