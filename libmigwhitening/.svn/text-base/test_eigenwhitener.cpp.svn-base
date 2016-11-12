#include "eigenwhitener.h"
#include <iostream>
int main()
{
	//create data matrix as float[][]
	
	float data_mat[10][3] = {
							{1.0f, 3.0f, 5.0f},
							{2.0f, 5.0f, 2.0f},
							{3.0f, 4.0f, 2.0f},
							{3.0f, 5.0f, 1.0f},
						   {10.0f, 3.0f, 5.0f},
							{1.0f, 1.0f, 5.0f},
							{2.0f, 4.0f, 2.0f},
							{3.0f, 4.2f, 2.1f},
							{3.4f, 5.1f, 1.4f},
							{3.2f, 3.9f, 5.2f}
							};


	float** feat_mat = new float*[10];
	
	for (int i= 0; i!= 10; ++i)
	{
		feat_mat[i] = new float[3];
		for (int j = 0; j!=3; ++j)
		{
			feat_mat[i][j] = data_mat[i][j];
		}
	}
							
	
	float in_vec[] = {1,2,3};
	float out_vec[] = {0,0,0};

	/***********************************************
	   outside computed covariance matrix is:
		6.5493    0.0787    0.8542
		0.0787    1.5484   -1.6704
		0.8542   -1.6704    3.0179


		eigenvectors matrix is:
		-0.0882   -0.2423   -0.9662
		0.8253   -0.5609    0.0653
		0.5578    0.7916   -0.2494

		eigenvalues matrix is:
		0.4111	0			0
        0		3.9400		0
        0		0			6.7645

		inv(sqrt(eigenvaluesmatrix is):
		1.5596	0			0
         0		0.5038		0
         0		0			0.3845

		 
		 whitening applied on [1,2,3] gives:
		 -0.1376	-0.1220		-0.3715

	************************************************/

	EigenWhitener whitener;
	whitener.compute(const_cast < const float **const >(feat_mat),3,10);
	whitener.apply(in_vec, out_vec);
	std::cout << "expected: 5.0465 0.5093 -0.6090\n";
	std::cout << "obtained: " << out_vec[0] << " " << out_vec[1] << " " << out_vec[2];
}