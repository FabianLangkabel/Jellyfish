#include "../header/one_electron_system.h"
namespace QC
{
	void One_Electron_System::compute()
	{
		int number_basisfunctions = this->overlapmatrix.rows();
		
		// Diagonalize S and Obtain tranformation matrix X
		Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> es(this->overlapmatrix);
		Eigen::MatrixXd U = es.eigenvectors();
		Eigen::MatrixXd S_diag_powerminusonehalf = Eigen::MatrixXd::Zero(number_basisfunctions, number_basisfunctions);
		for (int i = 0; i < number_basisfunctions; i++) {
			S_diag_powerminusonehalf(i, i) = 1 / sqrt(es.eigenvalues()[i]);
		}
		Eigen::MatrixXd X = U * S_diag_powerminusonehalf * U.adjoint();

		//Diagonalize H Matrix
		Eigen::MatrixXd H_prime = X.adjoint() * this->one_electron_integrals * X;

		Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> es2(H_prime);
		Eigen::MatrixXd C_prime = es2.eigenvectors();
		this->Orbitalenergys = Eigen::VectorXd::Zero(number_basisfunctions);
		for (int i = 0; i < number_basisfunctions; i++) {
			this->Orbitalenergys(i) = es2.eigenvalues()[i] + this->E0;
		}

		this->C = X * C_prime;
		this->P = C * C.transpose();
	}
}