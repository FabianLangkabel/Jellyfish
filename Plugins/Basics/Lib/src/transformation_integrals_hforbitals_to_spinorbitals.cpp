#include "../header/transformation_integrals_hforbitals_to_spinorbitals.h"
namespace QC
{
	void Transformation_Integrals_HFOrbitals_To_Spinorbitals::compute_one_electron_integrals()
	{
		int number_basisfunctions = this->one_electron_integrals_hforbitals.rows();

		this->one_electron_integrals_spinorbitals = Eigen::MatrixXd::Zero(number_basisfunctions * 2, number_basisfunctions * 2);
		for (int p = 0; p < number_basisfunctions * 2; p++) {
			for (int q = 0; q < number_basisfunctions * 2; q++) {
				int p_index = p / 2;
				int q_index = q / 2;
				this->one_electron_integrals_spinorbitals(p, q) = this->one_electron_integrals_hforbitals(p_index, q_index) * (p % 2 == q % 2);
			}
		}
	}
	void Transformation_Integrals_HFOrbitals_To_Spinorbitals::compute_two_electron_integrals()
	{

		int number_basisfunctions = this->two_electron_integrals_hforbitals.dimensions()[0];

		this->two_electron_integrals_spinorbitals = Eigen::Tensor<double, 4>(number_basisfunctions * 2, number_basisfunctions * 2, number_basisfunctions * 2, number_basisfunctions * 2);
		for (int p = 0; p < number_basisfunctions * 2; p++) {
			for (int q = 0; q < number_basisfunctions * 2; q++) {
				for (int r = 0; r < number_basisfunctions * 2; r++) {
					for (int s = 0; s < number_basisfunctions * 2; s++) {
						int p_index = p / 2;
						int q_index = q / 2;
						int r_index = r / 2;
						int s_index = s / 2;
						double val1 = this->two_electron_integrals_hforbitals(p_index, r_index, q_index, s_index) * (p % 2 == r % 2) * (q % 2 == s % 2);
						double val2 = this->two_electron_integrals_hforbitals(p_index, s_index, q_index, r_index) * (p % 2 == s % 2) * (q % 2 == r % 2);
						this->two_electron_integrals_spinorbitals(p, q, r, s) = val1 - val2;
					}
				}
			}
		}
	}
	void Transformation_Integrals_HFOrbitals_To_Spinorbitals::compute()
	{
		compute_one_electron_integrals();
		compute_two_electron_integrals();
	}
}