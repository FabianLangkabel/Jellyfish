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
#include "../header/pointcharge.h"

namespace QC
{
	
	class Configuration_Interaction_Dipole_Moment
	{
	private:
		Eigen::MatrixXd one_electron_integrals_spinorbitals_x;
		Eigen::MatrixXd one_electron_integrals_spinorbitals_y;
		Eigen::MatrixXd one_electron_integrals_spinorbitals_z;

		std::vector<Configuration> configurations;
		std::vector<std::string> configuration_strings;

		Eigen::MatrixXd CI_Matrix;
		std::vector<Eigen::MatrixXd> Dipole_Matrices;
		std::vector<QC::Pointcharge> Pointcharges;
		Eigen::Vector3d center_of_mass;

		std::vector<int> get_occupied_orbitals(Configuration configuration1, Configuration configuration2);
		int get_single_sign(Configuration conf1, int hole, int particle);
		std::tuple<int, int, int> get_single_set_params(Configuration configuration1, Configuration configuration2);
		Eigen::VectorXd evaluate_states_dipole_moment(Configuration configuration1, Configuration configuration2);

	public:
		void set_one_electron_integrals_spinorbitals_x(Eigen::MatrixXd one_electron_integrals_spinorbitals_x) { this->one_electron_integrals_spinorbitals_x = one_electron_integrals_spinorbitals_x; }
		void set_one_electron_integrals_spinorbitals_y(Eigen::MatrixXd one_electron_integrals_spinorbitals_y) { this->one_electron_integrals_spinorbitals_y = one_electron_integrals_spinorbitals_y; }
		void set_one_electron_integrals_spinorbitals_z(Eigen::MatrixXd one_electron_integrals_spinorbitals_z) { this->one_electron_integrals_spinorbitals_z = one_electron_integrals_spinorbitals_z; }
		void set_ci_matrix(Eigen::MatrixXd CI_Matrix) { this->CI_Matrix = CI_Matrix; }
		void set_configuration(std::vector<Configuration> configurations) { this->configurations = configurations; }
		void set_pointcharges(std::vector<QC::Pointcharge> Pointcharges) { this->Pointcharges = Pointcharges; }

		Eigen::MatrixXd get_dipole_matrix_x() { return Dipole_Matrices[0]; }
		Eigen::MatrixXd get_dipole_matrix_y() { return Dipole_Matrices[1]; }
		Eigen::MatrixXd get_dipole_matrix_z() { return Dipole_Matrices[2]; }
		std::vector<Eigen::MatrixXd> get_dipole_matrices() { return Dipole_Matrices; }
		
		void compute_electronic_dipole_moment();
		void compute();
	};
}