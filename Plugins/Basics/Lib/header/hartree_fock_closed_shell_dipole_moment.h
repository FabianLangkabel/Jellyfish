#pragma once
#include <vector>
#include <iostream>
#include <Eigen/Dense>
#include "../header/pointcharge.h"


namespace QC
{
	class Hartree_Fock_Closed_Shell_Dipole_Moment
	{
	private:
		Eigen::MatrixXd HF_Density_Matrix;
		Eigen::MatrixXd Overlapmatrix_x;
		Eigen::MatrixXd Overlapmatrix_y;
		Eigen::MatrixXd Overlapmatrix_z;
		Eigen::VectorXd Nuclear_Dipole_Moment;
		Eigen::VectorXd Electric_Dipole_Moment;
		Eigen::VectorXd Dipole_Moment;
		double Dipole_Moment_Absolute_Value;

	public:
		void set_nuclear_dipole_moment(Eigen::VectorXd Nuclear_Dipole_Moment) { this->Nuclear_Dipole_Moment = Nuclear_Dipole_Moment; }
		void set_HF_Density_Matrix(Eigen::MatrixXd HF_Density_Matrix) { this->HF_Density_Matrix = HF_Density_Matrix; }
		void set_overlapmatrix_x(Eigen::MatrixXd Overlapmatrix_x) { this->Overlapmatrix_x = Overlapmatrix_x; }
		void set_overlapmatrix_y(Eigen::MatrixXd Overlapmatrix_y) { this->Overlapmatrix_y = Overlapmatrix_y; }
		void set_overlapmatrix_z(Eigen::MatrixXd Overlapmatrix_z) { this->Overlapmatrix_z = Overlapmatrix_z; }
		Eigen::VectorXd get_electric_dipole_moment() { return Electric_Dipole_Moment; }
		Eigen::VectorXd get_dipole_moment() { return Dipole_Moment; }
		double get_dipole_moment_absolute_value() { return Dipole_Moment_Absolute_Value; }
		void compute();

	};
}