#pragma once
#include <Eigen/Dense>
#include "gaussian_basisset.h"
#include "potential_gaussian.h"
#include <tuple>


namespace QC
{
	class Integrator_Gaussian_Potential_Gaussian
	{
	private:
		Gaussian_Basisset Basisset;
		Eigen::MatrixXd Integrals;
		std::vector<Potential_Gaussian> Gaussian_Potentials;

		inline int fact(int n);
		inline int fact2(int n);
		int binomial(int a, int b);
		double binomial_prefactor(int s, int ia, int ib, double xpa, double xpb);
		double norm(double exponent, int l, int m, int n);
		double overlap_1D(int a1, int a2, double exponent1, double exponent2, double potential_exponent, double center1, double center2, double center_product);
		std::tuple<std::vector<double>, double> gaussprod(std::vector<double> potential_center, double potential_exponent, std::vector<double> basisfunc1_center, double basisfunc1_exponent, std::vector<double> basisfunc2_center, double basisfunc2_exponent);
		double overlap(
			std::vector<double> potential_center, double potential_exponent,
			std::vector<double> basisfunc1_center, double basisfunc1_exponent, int basisfunc1_angular_moment_x, int basisfunc1_angular_moment_y, int basisfunc1_angular_moment_z,
			std::vector<double> basisfunc2_center, double basisfunc2_exponent, int basisfunc2_angular_moment_x, int basisfunc2_angular_moment_y, int basisfunc2_angular_moment_z);
		double potentialoverlap(std::tuple<Gaussian_Basisfunction, int> basisfunction1, std::tuple<Gaussian_Basisfunction, int> basisfunction2, std::vector<Potential_Gaussian> Gaussian_Potentials);


	public:
		void set_basisset(Gaussian_Basisset Basisset)
		{
			this->Basisset = Basisset;
		}
		void set_Integrals(Eigen::MatrixXd Integrals) { this->Integrals = Integrals; }
		Eigen::MatrixXd get_Integrals() { return Integrals; }
		void add_potential_gaussian(Potential_Gaussian potential_gaussian)
		{
			this->Gaussian_Potentials.push_back(potential_gaussian);
		}
		void compute();
	};
}