#pragma once

#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
#include <Eigen/Dense>
#include <unsupported/Eigen/CXX11/Tensor>

namespace QC
{

	class CIS
	{
	private:
		double E0 = 0;
		Eigen::Tensor<double, 4> two_electron_integrals_spinorbitals;
		int electrons;

		Eigen::MatrixXd ci_matrix;
		Eigen::VectorXd ci_values;
		Eigen::MatrixXd HF_Orbitalenergys;


	public:
		void set_HF_Orbitalenergys(Eigen::MatrixXd HF_Orbitalenergys)
		{
			this->HF_Orbitalenergys = HF_Orbitalenergys;
		}
		void set_two_electron_integrals_spinorbitals(Eigen::Tensor<double, 4> two_electron_integrals_spinorbitals)
		{
			this->two_electron_integrals_spinorbitals = two_electron_integrals_spinorbitals;
		}
		void set_electrons(int electrons)
		{
			this->electrons = electrons;
		}
		void set_E0(double E0)
		{
			this->E0 = E0;
		}

		Eigen::MatrixXd get_ci_matrix() { return ci_matrix; }
		Eigen::VectorXd get_ci_eigenvector(int index) { return ci_matrix.col(index); }
		Eigen::VectorXd get_eigenenergys() { return ci_values; }

		//ACHTUNG ACHTUNG TEMPOR�R WIRD DIREKT E0 RAUFGERECHNET
		double get_eigenenergy(int index) { return ci_values(index) + E0; }

		Eigen::VectorXd get_statevector_after_transformation(int index) {
			Eigen::VectorXd vec = Eigen::VectorXd::Zero(get_ci_eigenvector(index).rows());
			vec[index] = 1;
			return vec;
		}

		void compute();
	};
}