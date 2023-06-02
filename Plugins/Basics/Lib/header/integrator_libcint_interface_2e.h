#pragma once
#include <Eigen/Dense>
#include <unsupported/Eigen/CXX11/Tensor>
#include "gaussian_basisset.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

extern "C" {
#include <cint.h>
}


namespace QC
{
	class Integrator_libint_interface_2e
	{
	private:
		Gaussian_Basisset Basisset;
		Eigen::Tensor<double, 4> Integrals;
		int Integral_Type = 0;


	public:
		void set_basisset(Gaussian_Basisset Basisset)
		{
			this->Basisset = Basisset;
		}
		void set_integraltype(int integraltype)
		{
			/* Available Integrals
			* 0 = ERI <ij|kl>
			*/
			this->Integral_Type = integraltype;
		}
		void set_Integrals(Eigen::Tensor<double, 4> Integrals) { this->Integrals = Integrals; }
		Eigen::Tensor<double, 4> get_Integrals() { return Integrals; }
		void compute();
	};
}