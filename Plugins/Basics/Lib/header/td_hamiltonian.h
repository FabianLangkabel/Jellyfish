#pragma once
#include <vector>
#include <Eigen/Dense>

namespace QC
{
	class TD_Hamiltonian
	{
	private:
		double time = 0;
		Eigen::MatrixXcd Hamiltonian;

	public:
		void set_Hamiltonian(Eigen::MatrixXcd Hamiltonian) { this->Hamiltonian = Hamiltonian; }
		double get_time() { return this->time; }

		Eigen::MatrixXcd get_Hamiltonian() { return this->Hamiltonian; }
		void set_time(double time) { this->time = time; }
		virtual void compute() { ; }
	};
}