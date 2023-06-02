#pragma once
#include <vector>
#include <iostream>
#include <complex>
#include <cmath>
#include <Eigen/Dense>
#include <unsupported/Eigen/MatrixFunctions>
#include "td_hamiltonian.h"
#include "td_hamiltonian_molecular.h"

namespace QC
{
	class Propagator_Matrixexponential
	{
	private:
		bool timeindependent = false;
		std::vector<Eigen::VectorXcd> initialwavefunctions;
		std::vector<std::vector<Eigen::VectorXcd>> timedependtwavefunctions;
		double stepsize;
		int steps;

		std::vector<TD_Hamiltonian*> Hamiltonian;

		Eigen::MatrixXcd get_Hamiltonian_at_time(double time);

	public:
		void add_initialwavefunction(Eigen::VectorXcd initialwavefunction)
		{
			this->initialwavefunctions.push_back(initialwavefunction);
		}

		void add_initialwavefunction(Eigen::VectorXd initialwavefunction)
		{
			this->initialwavefunctions.push_back(initialwavefunction);
		}
		void set_stepsize(double stepsize) { this->stepsize = stepsize; }
		void set_steps(int steps) { this->steps = steps; }

		void add_Hamiltonian(TD_Hamiltonian *Hamiltonian) { this->Hamiltonian.push_back(Hamiltonian); }
		void compute();
	};
}