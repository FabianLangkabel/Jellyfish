#pragma once
#include <Eigen/Dense>
#include "gaussian_basisset.h"
#include "pointcharge.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>

extern "C" {
#include <cint.h>
	int cint1e_ovlp_sph(double* buf, int* shls,
		int* atm, int natm, int* bas, int nbas, double* env);
	int cint1e_ovlp_cart(double* buf, int* shls,
		int* atm, int natm, int* bas, int nbas, double* env);

	int cint1e_kin_sph(double* buf, int* shls,
		int* atm, int natm, int* bas, int nbas, double* env);
	int cint1e_kin_cart(double* buf, int* shls,
		int* atm, int natm, int* bas, int nbas, double* env);

	int cint1e_r_sph(double* buf, int* shls,
		int* atm, int natm, int* bas, int nbas, double* env);
	int cint1e_r_cart(double* buf, int* shls,
		int* atm, int natm, int* bas, int nbas, double* env);

	int cint1e_rr_sph(double* buf, int* shls,
		int* atm, int natm, int* bas, int nbas, double* env);
	int cint1e_rr_cart(double* buf, int* shls,
		int* atm, int natm, int* bas, int nbas, double* env);

	int cint1e_nuc_sph(double* buf, int* shls,
		int* atm, int natm, int* bas, int nbas, double* env);
	int cint1e_nuc_cart(double* buf, int* shls,
		int* atm, int natm, int* bas, int nbas, double* env);


}


namespace QC
{
	class Integrator_libint_interface_1e
	{
	private:
		Gaussian_Basisset Basisset;
		Eigen::MatrixXd Integrals;
		std::vector<Eigen::MatrixXd> multiple_Integrals;
		std::vector<Pointcharge> Pointcharges;
		int Integral_Type;


	public:
		void set_basisset(Gaussian_Basisset Basisset)
		{
			this->Basisset = Basisset;
		}
		void set_pointcharges(std::vector<Pointcharge> Pointcharges)
		{
			this->Pointcharges = Pointcharges;
		}
		void set_integraltype(int integraltype)
		{
			/* Available Integrals
			* 0 = Overlap <i|j>
			* 1 = Kinetic <i|K|j>
			* 2 = <i|X|j>
			* 3 = <i|Y|j>
			* 4 = <i|Z|j>
			* 5 = <i|Nuc|j>

			multiple Integrals
			* 100 = <i|r|j> 
			* 101 = <i|r^2|j> 
			*/
			this->Integral_Type = integraltype;
		}
		void set_Integrals(Eigen::MatrixXd Integrals) { this->Integrals = Integrals; }
		Eigen::MatrixXd get_Integrals() { return Integrals; }
		std::vector<Eigen::MatrixXd> get_multipleIntegrals() { return multiple_Integrals; }
		void compute();
	};
}