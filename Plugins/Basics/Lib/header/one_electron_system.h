#pragma once
#include <vector>
#include <iostream>
#include <Eigen/Dense>
#include <unsupported/Eigen/CXX11/Tensor>

namespace QC
{
	class One_Electron_System
	{
	private:
		Eigen::MatrixXd overlapmatrix;
		Eigen::MatrixXd one_electron_integrals;
		double E0 = 0;

		double HF_energy;
		Eigen::MatrixXd P;
		Eigen::MatrixXd C;
		Eigen::VectorXd Orbitalenergys;

	public:
		void set_E0(double E0) { this->E0 = E0; }
		void set_overlapmatrix(Eigen::MatrixXd overlapmatrix) { this->overlapmatrix = overlapmatrix; }
		void set_one_electron_integrals(Eigen::MatrixXd one_electron_integrals) { this->one_electron_integrals = one_electron_integrals; }

		Eigen::MatrixXd get_density_matrix() { return P; }
		Eigen::MatrixXd get_C_matrix() { return C; }
		Eigen::VectorXd get_orbitalenergys() { return Orbitalenergys; }
		double get_orbitalenergy(int i) { return Orbitalenergys(i); }

		void compute();
	};
}