#include "../header/propagator_matrixexponential.h"
namespace QC
{
	Eigen::MatrixXcd Propagator_Matrixexponential::get_Hamiltonian_at_time(double time)
	{
		this->Hamiltonian[0]->set_time(time);
		this->Hamiltonian[0]->compute();
		Eigen::MatrixXcd Hamiltonian_total = this->Hamiltonian[0]->get_Hamiltonian();
		for (int i = 1; i < this->Hamiltonian.size(); i++)
		{
			this->Hamiltonian[i]->set_time(time);
			this->Hamiltonian[i]->compute();
			Hamiltonian_total += this->Hamiltonian[i]->get_Hamiltonian();
		}
		return Hamiltonian_total;
	}

	void Propagator_Matrixexponential::compute()
	{
		for (int i = 0; i < this->initialwavefunctions.size(); i++)
		{
			std::vector<Eigen::VectorXcd> wf;
			wf.push_back(this->initialwavefunctions[i]);
			this->timedependtwavefunctions.push_back(wf);
		}

		double time = 0;
		for (int step = 0; step < this->steps; step++)
		{
			Eigen::MatrixXcd Exponent = std::complex<double>(0,-1) * get_Hamiltonian_at_time(time) * this->stepsize;
			Eigen::MatrixXcd Hamiltonian_Exponent = Exponent.exp();
			for (int i = 0; i < this->timedependtwavefunctions.size(); i++)
			{
				Eigen::VectorXcd wf_at_time = Hamiltonian_Exponent * this->timedependtwavefunctions[i][step];
				timedependtwavefunctions[i].push_back(wf_at_time);
			}

			time += this->stepsize;
		}
	}
}