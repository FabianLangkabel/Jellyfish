#pragma once

#include <Eigen/Dense>
#include "gaussian_basisset.h"
#include "operator_function.h"
#include <tuple>
#include <thread>

#include <array>

namespace QC
{
	class Integrator_Operator_Function_Numerical
	{
	private:
		QC::Operator_Function* Operator_Function;
		Gaussian_Basisset Basisset;

		double range;
		double steps;

		double operator_function(double x, double y, double z);

		Eigen::MatrixXd Integrals;

		double stepsize;
		int basisset_size;


	public:
		void set_operator_function(QC::Operator_Function* Operator_Function) { this->Operator_Function = Operator_Function; }
		void set_basisset(Gaussian_Basisset Basisset) { this->Basisset = Basisset; }
		void set_range(double range) { this->range = range; }
		void set_steps(int steps) { this->steps = steps; }

		Eigen::MatrixXd get_Integrals(){ return Integrals; }

		void compute();

		void computeCase0(int id);
		void computeCase1(int id);
		/*
		double semianalyticalorder = 3;
		double capstrength = 10;
		double caporder = 1;
		double R = 10;
		double dR = 1;
		Eigen::MatrixXd get_semianalyiticalIntegrals() { return Integrals; }
		double getsemianalyticalOverlap(double exponent, double R);
		double getErrorfct(double R);
		*/
	};
}