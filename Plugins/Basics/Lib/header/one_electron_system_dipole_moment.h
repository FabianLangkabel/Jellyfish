#pragma once
#include <vector>
#include <iostream>
#include <Eigen/Dense>
#include <unsupported/Eigen/CXX11/Tensor>

namespace QC
{

	class One_Electron_System_Dipole_Moment
	{
	private:
		Eigen::MatrixXd position_integrals_x;
		Eigen::MatrixXd position_integrals_y;
		Eigen::MatrixXd position_integrals_z;
		Eigen::MatrixXd HF_C_Matrix;

		Eigen::VectorXd Dipole_Moment_Nuclear;
		std::vector<Eigen::MatrixXd> Electronic_Dipole_Matrices;
		std::vector<Eigen::MatrixXd> Dipole_Matrices;
	
	public:
		void set_nuclear_dipole_moment(Eigen::VectorXd Dipole_Moment_Nuclear) { this->Dipole_Moment_Nuclear = Dipole_Moment_Nuclear; }
		void set_position_integrals_x(Eigen::MatrixXd position_integrals_x) { this->position_integrals_x = position_integrals_x; }
		void set_position_integrals_y(Eigen::MatrixXd position_integrals_y) { this->position_integrals_y = position_integrals_y; }
		void set_position_integrals_z(Eigen::MatrixXd position_integrals_z) { this->position_integrals_z = position_integrals_z; }
		void set_HF_C_matrix(Eigen::MatrixXd HF_C_Matrix) { this->HF_C_Matrix = HF_C_Matrix; }

		Eigen::MatrixXd get_electronic_dipole_matrix_x() { return Electronic_Dipole_Matrices[0]; }
		Eigen::MatrixXd get_electronic_dipole_matrix_y() { return Electronic_Dipole_Matrices[1]; }
		Eigen::MatrixXd get_electronic_dipole_matrix_z() { return Electronic_Dipole_Matrices[2]; }
		std::vector<Eigen::MatrixXd> get_electronic_dipole_matrices() { return Electronic_Dipole_Matrices; }

		Eigen::MatrixXd get_dipole_matrix_x() { return Dipole_Matrices[0]; }
		Eigen::MatrixXd get_dipole_matrix_y() { return Dipole_Matrices[1]; }
		Eigen::MatrixXd get_dipole_matrix_z() { return Dipole_Matrices[2]; }
		std::vector<Eigen::MatrixXd> get_dipole_matrices() { return Dipole_Matrices; }

		void compute_electronic_dipole_moment();
		void compute();
	};
}