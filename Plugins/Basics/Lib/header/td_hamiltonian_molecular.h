#pragma once
#include <vector>
#include <Eigen/Dense>
#include "td_hamiltonian.h"

namespace QC
{
	class TD_Hamiltonian_Molecular : public TD_Hamiltonian
	{
	private:
		Eigen::VectorXd CI_eigenenergys;

	public:
		void set_CI_eigenenergys(Eigen::VectorXd CI_eigenenergys){ this->CI_eigenenergys = CI_eigenenergys; }
		void compute();
	};
}