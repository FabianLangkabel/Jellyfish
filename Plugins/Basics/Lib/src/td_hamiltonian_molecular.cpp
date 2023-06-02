#include "../header/td_hamiltonian_molecular.h"
namespace QC
{
	void TD_Hamiltonian_Molecular::compute()
	{
		int size = this->CI_eigenenergys.rows();
		Eigen::MatrixXcd Hamiltonian = Eigen::MatrixXcd::Zero(size, size);
		for (int i = 0; i < size; i++) {
			Hamiltonian(i, i) = this->CI_eigenenergys(i);
		}
		this->set_Hamiltonian(Hamiltonian);
	}
}