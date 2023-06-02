#pragma once
#include <vector>
#include <iostream>
#include <Eigen/Dense>
#include <unsupported/Eigen/CXX11/Tensor>

namespace QC
{
	class Hartree_Fock_Closed_Shell
	{
	private:
		Eigen::MatrixXd overlapmatrix;
		Eigen::MatrixXd one_electron_integrals;
		Eigen::Tensor<double, 4> two_electron_integrals;
		int electrons;
		int max_iter_steps = 10000;
		double energy_threshold = 0.0000001;
		double E0 = 0;

		double HF_energy;
		int iteration;
		Eigen::MatrixXd P;
		Eigen::MatrixXd C;
		Eigen::VectorXd Orbitalenergys;


	public:
		void set_E0(double E0)
		{
			this->E0 = E0;
		}
		void set_overlapmatrix(Eigen::MatrixXd overlapmatrix)
		{
			this->overlapmatrix = overlapmatrix;
		}
		void set_one_electron_integrals(Eigen::MatrixXd one_electron_integrals)
		{
			this->one_electron_integrals = one_electron_integrals;
		}
		void set_two_electron_integrals(Eigen::Tensor<double, 4> two_electron_integrals)
		{
			this->two_electron_integrals = two_electron_integrals;
		}
		void set_electrons(int electrons)
		{
			this->electrons = electrons;
		}
		void set_max_iteration_steps(int max_iteration_steps)
		{
			this->max_iter_steps = max_iteration_steps;
		}
		void set_energy_threshold(double energy_threshold)
		{
			this->energy_threshold = energy_threshold;
		}
		void set_C_matrix(Eigen::MatrixXd C){ this->C = C;}
		void set_densitymatrix(Eigen::MatrixXd P) { this->P = P; }
		void set_groundstate_Energy(double HF_energy) { this->HF_energy = HF_energy; }
		void set_orbitalenergys(Eigen::VectorXd Orbitalenergys) { this->Orbitalenergys = Orbitalenergys; }

		Eigen::MatrixXd get_overlapmatrix() { return overlapmatrix; }
		Eigen::MatrixXd get_one_electron_integrals() { return one_electron_integrals; }
		Eigen::Tensor<double, 4> get_two_electron_integrals() { return two_electron_integrals; }
		int get_electrons() { return electrons; }
		int get_max_iteration_steps() { return max_iter_steps; }
		double get_energy_threshold() { return energy_threshold; }
		double get_groundstate_Energy() { return HF_energy; }
		int get_iteration() { return iteration; }
		Eigen::MatrixXd get_densitymatrix() { return P; }
		Eigen::MatrixXd get_C_matrix() { return C; }
		Eigen::VectorXd get_orbitalenergys() { return Orbitalenergys; }

		void compute();
	};
}