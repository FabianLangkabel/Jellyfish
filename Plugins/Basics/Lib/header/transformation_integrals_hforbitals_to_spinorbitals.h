#pragma once
#include <vector>
#include <iostream>
#include <Eigen/Dense>
#include <unsupported/Eigen/CXX11/Tensor>

namespace QC
{
	class Transformation_Integrals_HFOrbitals_To_Spinorbitals
	{
	private:
		Eigen::MatrixXd one_electron_integrals_hforbitals;
		Eigen::Tensor<double, 4> two_electron_integrals_hforbitals;

		Eigen::MatrixXd one_electron_integrals_spinorbitals;
		Eigen::Tensor<double, 4> two_electron_integrals_spinorbitals;


	public:
		void set_one_electron_integrals_hforbitals(Eigen::MatrixXd one_electron_integrals_hforbitals)
		{
			this->one_electron_integrals_hforbitals = one_electron_integrals_hforbitals;
		}
		void set_two_electron_integrals_hforbitals(Eigen::Tensor<double, 4> two_electron_integrals_hforbitals)
		{
			this->two_electron_integrals_hforbitals = two_electron_integrals_hforbitals;
		}

		void set_one_electron_integrals_spinorbitals(Eigen::MatrixXd one_electron_integrals_spinorbitals) { this->one_electron_integrals_spinorbitals = one_electron_integrals_spinorbitals; }
		void set_two_electron_integrals_spinorbitals(Eigen::Tensor<double, 4> two_electron_integrals_spinorbitals) { this->two_electron_integrals_spinorbitals = two_electron_integrals_spinorbitals; }

		Eigen::MatrixXd get_one_electron_integrals_spinorbitals() { return one_electron_integrals_spinorbitals; }
		Eigen::Tensor<double, 4> get_two_electron_integrals_spinorbitals() { return two_electron_integrals_spinorbitals; }

		void compute();
		void compute_one_electron_integrals();
		void compute_two_electron_integrals();
	};
}