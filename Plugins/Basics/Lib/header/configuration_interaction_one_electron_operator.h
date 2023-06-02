#pragma once
#define NOMINMAX
#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
#include <Eigen/Dense>
#include <unsupported/Eigen/CXX11/Tensor>
#include "../header/configuration_interaction.h"
#include "../header/configurations.h"

namespace QC
{
	
	class Configuration_Interaction_One_Electron_Operator
	{
	private:
		Eigen::MatrixXd one_electron_integrals_spinorbitals;

		std::vector<Configuration> configurations;
		std::vector<std::string> configuration_strings;

		Eigen::MatrixXd CI_Matrix;

		Eigen::MatrixXcd One_Electron_Operator_Matrix;

		std::vector<int> get_occupied_orbitals(Configuration configuration1, Configuration configuration2);
		int get_single_sign(Configuration conf1, int hole, int particle);
		std::tuple<int, int, int> get_single_set_params(Configuration configuration1, Configuration configuration2);
		double evaluate_states_one_electron_operator(Configuration configuration1, Configuration configuration2);

	public:
		void set_one_electron_integrals_spinorbitals(Eigen::MatrixXd one_electron_integrals_spinorbitals) { this->one_electron_integrals_spinorbitals = one_electron_integrals_spinorbitals; }
		void set_ci_matrix(Eigen::MatrixXd CI_Matrix) { this->CI_Matrix = CI_Matrix; }
		void set_configuration(std::vector<Configuration> configurations) { this->configurations = configurations; }

		Eigen::MatrixXcd get_one_electron_operator_matrix() { return One_Electron_Operator_Matrix; }
		
		void compute();
	};
}