#include "../header/hartree_fock_closed_shell.h"
namespace QC
{
	void Hartree_Fock_Closed_Shell::compute()
	{
		int number_basisfunctions = this->get_overlapmatrix().rows();
		int occupied = this->get_electrons() / 2;

		// Diagonalize S and Obtain tranformation matrix X
		Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> es(this->get_overlapmatrix());
		Eigen::MatrixXd U = es.eigenvectors();
		Eigen::MatrixXd S_diag_powerminusonehalf = Eigen::MatrixXd::Zero(number_basisfunctions, number_basisfunctions);
		for (int i = 0; i < number_basisfunctions; i++) {
			S_diag_powerminusonehalf(i, i) = 1 / sqrt(es.eigenvalues()[i]);
		}
		Eigen::MatrixXd X = U * S_diag_powerminusonehalf * U.adjoint();

		// Guess density Matrix P
		this->P = Eigen::MatrixXd::Zero(number_basisfunctions, number_basisfunctions);
		for (int i = 0; i < number_basisfunctions; i++) {
			this->P(i, i) = 1;
		}

		bool notconverged = true;
		this->iteration = 1;
		Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> es2;

		while (notconverged && this->iteration < this->max_iter_steps + 1) {
			Eigen::MatrixXd G = Eigen::MatrixXd::Zero(number_basisfunctions, number_basisfunctions);
			for (int i = 0; i < number_basisfunctions; i++) {
				for (int j = 0; j < number_basisfunctions; j++) {
					for (int k = 0; k < number_basisfunctions; k++) {
						for (int l = 0; l < number_basisfunctions; l++) {
							double sum1 = this->two_electron_integrals(i, j, l, k);
							double sum2 = this->two_electron_integrals(i, k, l, j);
							G(i, j) += P(k, l) * (sum1 - 0.5 * sum2);
						}
					}
				}
			}

			Eigen::MatrixXd F = this->one_electron_integrals + G;
			Eigen::MatrixXd F_prime = X.adjoint() * F * X;

			es2.compute(F_prime);
			Eigen::MatrixXd C_prime = es2.eigenvectors();
			this->Orbitalenergys = es2.eigenvalues();

			this->C = X * C_prime;

			Eigen::MatrixXd P_new = Eigen::MatrixXd::Zero(number_basisfunctions, number_basisfunctions);
			for (int i = 0; i < number_basisfunctions; i++) {
				for (int j = 0; j < number_basisfunctions; j++) {
					P_new(i, j) = 0;
					for (int k = 0; k < occupied; k++) {
						P_new(i, j) = P_new(i, j) + 2 * C(i, k) * C(j, k);
					}
				}
			}

			double energy = 0;
			Eigen::MatrixXd HandF = this->one_electron_integrals + F;
			for (int i = 0; i < number_basisfunctions; i++) {
				for (int j = 0; j < number_basisfunctions; j++) {
					energy += P(j, i) * HandF(i, j);
				}
			}
			energy = energy / 2 + E0;

			std::cout << "HF Iteration Step " << this->iteration << "    E_HF= " << energy << std::endl;
			if (abs(energy - this->HF_energy) < this->energy_threshold && this->iteration > 1) {
				notconverged = false;
				std::cout << std::endl << "Hartree Fock Converged after " << this->iteration << " steps" << std::endl << "E_HF = " << energy << std::endl;
			}
			this->P = P_new;
			this->HF_energy = energy;
			this->iteration++;
		}
	}
}