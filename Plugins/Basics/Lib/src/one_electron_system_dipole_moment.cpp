#include "../header/one_electron_system_dipole_moment.h"
namespace QC
{

	void One_Electron_System_Dipole_Moment::compute_electronic_dipole_moment()
	{
		this->Electronic_Dipole_Matrices.push_back(this->HF_C_Matrix.transpose() * this->position_integrals_x * this->HF_C_Matrix);
		this->Electronic_Dipole_Matrices.push_back(this->HF_C_Matrix.transpose() * this->position_integrals_y * this->HF_C_Matrix);
		this->Electronic_Dipole_Matrices.push_back(this->HF_C_Matrix.transpose() * this->position_integrals_z * this->HF_C_Matrix);
	}

	void One_Electron_System_Dipole_Moment::compute()
	{
		compute_electronic_dipole_moment();
		std::vector<Eigen::MatrixXd> Nuclear_Dipole_Matrices;
		for (int k = 0; k < 3; k++) {
			Nuclear_Dipole_Matrices.push_back(Eigen::MatrixXd::Zero(this->HF_C_Matrix.rows(), this->HF_C_Matrix.rows()));
			for (int i = 0; i < this->HF_C_Matrix.rows(); i++) {
				Nuclear_Dipole_Matrices[k](i, i) = this->Dipole_Moment_Nuclear(k);
			}
			this->Dipole_Matrices.push_back(this->Electronic_Dipole_Matrices[k] + Nuclear_Dipole_Matrices[k]);
		}
	}

}