#include "eigenwhitener.h"

#include <iostream>
#include <fstream>
#include <vector>

#include <Eigen/Dense>
#include <Eigen/Core>

using namespace std;
using namespace Eigen;

const float _Eps = 1e-5f;

	EigenWhitener::EigenWhitener()
	{
	}
	;
	
	int EigenWhitener::compute( const float** const feat_mat, const int nfeat, const int nfeatvec )
	{
		//AAAAAAAAA CHECK IDX ORDERS!!!


		//obtain covariance matrix
			
		MatrixXf covmat(nfeat, nfeat);
		covmat.setZero();
		///////////////////////
		//TODO:now compute covmat

		//compute means
		std::vector<double> means(nfeat); //double to avoid roundoff
		for ( int i = 0; i != nfeatvec; i++ )
		{
			for (int j = 0; j != nfeat; j++ )
			{
				means[j] += feat_mat[i][j];
			}
		}

		for (int j = 0; j != nfeat; j++)
		{
			means[j] /= nfeatvec;
		}

		//now compute covariances (upper part, then we'll copy to the lower part)
		
		for ( int ivec = 0; ivec != nfeatvec; ivec++ )
		{
			for (int i = 0; i != nfeat; i++ )
			{
				for (int j = i; j != nfeat; j++ )
				{
					covmat(i,j) +=  static_cast<float>(((feat_mat[ivec][i] - means[i]) * (feat_mat[ivec][j] - means[j])));
				}
			}
		}
		//copy to lower
		for (int i = 0; i != nfeat; i++ )
		{
			for (int j = i + 1; j != nfeat; j++ )
			{
				covmat(j,i) = covmat(i,j);
			}
		}

		//divide per nfeatvec
		covmat /= static_cast<float>(nfeatvec);
		
		///////////////////////////////////////////////////////////

		//then get eigenvalues and eigenvectors

		SelfAdjointEigenSolver<MatrixXf> eigensolver(covmat);
		inv_sqrt_eigenvalues =  eigensolver.eigenvalues();
		
		// compute inverse square root, if <= 0 just trunk to 0
		for (int i = 0; i != nfeat; i++)
		{
			if (inv_sqrt_eigenvalues(i) > 0 + _Eps)
			{
				
				inv_sqrt_eigenvalues(i) = static_cast<float> (sqrt(1. / static_cast<double>(inv_sqrt_eigenvalues(i))));
			}
			else
			{
				inv_sqrt_eigenvalues(i) = 0.f;
			}
		}

		eigenvectors = eigensolver.eigenvectors();


		feat_num = nfeat;
		return 0;
	}
	;

	int EigenWhitener::load ( const char* const fname)
	{
		ifstream fin;
		fin.open(fname);
		fin.exceptions(ios::badbit);

		try
		{	
			//read number of elements and resize matrices
			int nfeat;
			fin >> nfeat;
			feat_num = nfeat;
			inv_sqrt_eigenvalues.resize(nfeat,1);
			eigenvectors.resize(nfeat,nfeat);
			
			//read eigenvalues
			for (int i = 0; i != nfeat; ++i)
			{
				fin >> inv_sqrt_eigenvalues(i);
			}
			
			//read eigenvectors
			for (int i = 0; i != nfeat; ++i)
			{
				for (int j = 0; j != nfeat; ++j)
				{
					fin >> eigenvectors(i,j);
				}
			}


		}
		catch (ifstream::failure e)
		{
			return 1;
		}
		return 0;
	}
	;

	int EigenWhitener::save ( const char* const fname)
	{
		ofstream fout;

		fout.open(fname);
		//vectors are vertical
		int nfeat = static_cast<int> (inv_sqrt_eigenvalues.rows());
		//write number of features
		fout << nfeat << endl << endl;
		//write eigenvalues
		for (int i = 0; i != nfeat; ++i)
		{
			fout << inv_sqrt_eigenvalues(i) << " ";
		}
		fout << endl << endl;

		//write eigenvectors matrix
		for (int i = 0; i != nfeat; ++i)
		{
			for (int j = 0; j != nfeat; ++j)
			{
				fout << eigenvectors(i,j) << " ";
			}
			fout << endl;
		}
		fout.close();
		if (fout.fail())
			return 1;
		else
			return 0;
	}
	;

	int EigenWhitener::apply ( const float* const in_vec, const float* out_vec)
	{
		//map to eigen data structures
		Map<VectorXf> in_vec_map(const_cast<float*> (in_vec), feat_num);
		Map<VectorXf> out_vec_map(const_cast<float*> (out_vec), feat_num);
		
		VectorXf temp = eigenvectors.transpose() * in_vec_map;

		temp = inv_sqrt_eigenvalues.array() * temp.array();
		out_vec_map = temp;

		return 0;
	}
	; 
