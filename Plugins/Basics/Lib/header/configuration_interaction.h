#pragma once
#define NOMINMAX
#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
#include <Eigen/Dense>
#include <unsupported/Eigen/CXX11/Tensor>
#include "configurations.h"

namespace QC
{

	class Configuration_Interaction
	{
	private:
		double E0 = 0;
		Eigen::MatrixXd one_electron_integrals_spinorbitals;
		Eigen::Tensor<double, 4> two_electron_integrals_spinorbitals;
		int electrons;
		int excitation_level = 0;

		std::vector<std::string> configuration_strings;
		Eigen::MatrixXd ci_matrix;
		Eigen::VectorXd ci_values;

		void distribute_excitation(int alphaelectrons, int betaelectrons, int spartialorbitals, int excitation, Configuration hartreefockstate);
		void excite(int first_alphavirtuell_index, int first_betavirtuell_index, int lowest_alphaelectron_index, int lowest_betaelectron_index, int lowest_alphavirtuell_index, int lowest_betavirtuell_index, int excite_alpha, int excite_beta, Configuration configuration);
		void print_configuration(Configuration conf);
		double evaluate_states(Configuration configuration1, Configuration configuration2);
		std::vector<int> get_occupied_orbitals(Configuration configuration1, Configuration configuration2);
		std::tuple<int, int, int> get_single_set_params(Configuration configuration1, Configuration configuration2);
		std::tuple<int, int, int, int, int> get_double_set_params(Configuration configuration1, Configuration configuration2);
		int get_single_sign(Configuration conf1, int hole, int particle);


	public:
		std::vector<Configuration> configurations;
		void set_one_electron_integrals_spinorbitals(Eigen::MatrixXd one_electron_integrals_spinorbitals)
		{
			this->one_electron_integrals_spinorbitals = one_electron_integrals_spinorbitals;
		}
		void set_two_electron_integrals_spinorbitals(Eigen::Tensor<double, 4> two_electron_integrals_spinorbitals)
		{
			this->two_electron_integrals_spinorbitals = two_electron_integrals_spinorbitals;
		}
		void set_electrons(int electrons)
		{
			this->electrons = electrons;
		}
		void set_excitation_level(int excitation_level)
		{
			this->excitation_level = excitation_level;
		}
		void set_E0(double E0)
		{
			this->E0 = E0;
		}
		void set_configuration_strings(std::vector<std::string> configuration_strings){ this->configuration_strings = configuration_strings;}
		void set_ci_matrix(Eigen::MatrixXd ci_matrix) { this->ci_matrix = ci_matrix; }
		void set_eigenenergys(Eigen::VectorXd ci_values) { this->ci_values = ci_values; }

		int get_excitation_level() { return excitation_level; }
		std::vector<std::string> get_configuration_strings() { return configuration_strings; }

		std::string get_configuration_string(int configuration) 
		{ 
			return configuration_strings[configuration];
		}
		Eigen::MatrixXd get_ci_matrix() { return ci_matrix; }
		Eigen::VectorXd get_ci_eigenvector(int index) { return ci_matrix.col(index); }
		Eigen::VectorXd get_eigenenergys() { return ci_values; }
		double get_eigenenergy(int index) { return ci_values(index); }

		void compute();

		std::vector<std::string> get_all_ci_strings_from_excitation_level(int spinorbitals, int electrons, int excitation_level);
	};
}