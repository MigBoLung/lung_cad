//THIS IS A TEMPLATE FOR A FULL FLEDGED WHITENING ALGORITHM USING Eigen


#include <iostream>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

int main()
{
	//data is a matrix containing all feat in rows
	//nfeat is the number of feat per reg
	MatrixXf data(nregs,nfeat);
	MatrixXf covmat(nfeat,nfeat);
	func_calc_cov (data, covmat);
	
	//these funcs obtain eigenvalues and eigenvectors
	SelfAdjointEigenSolver<Matrix2f> eigensolver(covmat);
	eigensolver.eigenvalues();
	eigensolver.eigenvectors();
	
	//now for each reg apply transform
	
	
	//best way is to create a serializable object
	//responsible for creating, loading, saving and maintaing
	// whitening data,
	// and applying transform
	
	//lambda^-1/2 Mt X
	
}
