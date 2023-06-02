#pragma once
#define NOMINMAX
#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
#include <map>

#include <Eigen/Dense>
#include <unsupported/Eigen/CXX11/Tensor>
#include "../header/configurations.h"

namespace QC
{
	
	class Single_Particle_Transition_Density_Matrix
	{
	private:
		std::vector<Configuration> configurations;
		Eigen::MatrixXd CI_Matrix;
		int InitialState = 0;
		int FinalState = 1;
		Eigen::MatrixXd OnePTDM;


	public:
		void set_ci_matrix(Eigen::MatrixXd CI_Matrix) { this->CI_Matrix = CI_Matrix; }
		void set_configurations(std::vector<Configuration> configurations) { this->configurations = configurations; }
		void set_InitialState(int InitialState) { this->InitialState = InitialState; }
		void set_FinalState(int FinalState) { this->FinalState = FinalState; }
		Eigen::MatrixXd get_OnePTDM() { return this->OnePTDM; }
		
		void compute();
	};
}