#pragma once
#include <vector>
#include <iostream>
#include <Eigen/Dense>
#include <unsupported/Eigen/CXX11/Tensor>

namespace QC
{
	class Transformation_Integrals_Basisfunction_To_HFOrbitals
	{
	private:
		Eigen::MatrixXd one_electron_integrals_basisfunctions;
		Eigen::Tensor<double, 4> two_electron_integrals_basisfunctions;

		Eigen::MatrixXd one_electron_integrals_hforbitals;
		Eigen::Tensor<double, 4> two_electron_integrals_hforbitals;

		Eigen::MatrixXd HF_C;

	public:
		void set_one_electron_integrals_basisfunction(Eigen::MatrixXd one_electron_integrals_basisfunction)
		{
			this->one_electron_integrals_basisfunctions = one_electron_integrals_basisfunction;
		}
		void set_two_electron_integrals_basisfunction(Eigen::Tensor<double, 4> two_electron_integrals_basisfunction)
		{
			this->two_electron_integrals_basisfunctions = two_electron_integrals_basisfunction;
		}
		void set_hf_cmatrix(Eigen::MatrixXd hf_cmatrix)
		{
			this->HF_C = hf_cmatrix;
		}

		void set_one_electron_integrals_hforbitals(Eigen::MatrixXd one_electron_integrals_hforbitals) { this->one_electron_integrals_hforbitals = one_electron_integrals_hforbitals; }
		void set_two_electron_integrals_hforbitals(Eigen::Tensor<double, 4> two_electron_integrals_hforbitals) { this->two_electron_integrals_hforbitals = two_electron_integrals_hforbitals; }

		Eigen::MatrixXd get_one_electron_integrals_hforbitals() { return one_electron_integrals_hforbitals; }
		Eigen::Tensor<double, 4> get_two_electron_integrals_hforbitals() { return two_electron_integrals_hforbitals; }

		void compute();
		void compute_one_electron_integrals();
		void compute_two_electron_integrals();
	};
}