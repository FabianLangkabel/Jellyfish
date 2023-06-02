#include "../header/transformation_integrals_basisfunction_to_hforbitals.h"
namespace QC
{
	void Transformation_Integrals_Basisfunction_To_HFOrbitals::compute_one_electron_integrals()
	{
		this->one_electron_integrals_hforbitals = HF_C.transpose() * this->one_electron_integrals_basisfunctions * this->HF_C;
	}
	void Transformation_Integrals_Basisfunction_To_HFOrbitals::compute_two_electron_integrals()
	{
		int number_basisfunctions = this->HF_C.rows();

		Eigen::Tensor<double, 4> two_e_integrals2(number_basisfunctions, number_basisfunctions, number_basisfunctions, number_basisfunctions);
		for (int i = 0; i < number_basisfunctions; i++) {
			for (int j = 0; j < number_basisfunctions; j++) {
				for (int k2 = 0; k2 < number_basisfunctions; k2++) {
					for (int l = 0; l < number_basisfunctions; l++) {
						double k_value = 0;
						for (int k = 0; k < number_basisfunctions; k++) {
							k_value += this->HF_C(k, k2) * this->two_electron_integrals_basisfunctions(i, j, k, l);
						}
						two_e_integrals2(i, j, k2, l) = k_value;
					}
				}
			}
		}

		Eigen::Tensor<double, 4> two_e_integrals3(number_basisfunctions, number_basisfunctions, number_basisfunctions, number_basisfunctions);
		for (int i2 = 0; i2 < number_basisfunctions; i2++) {
			for (int j = 0; j < number_basisfunctions; j++) {
				for (int k2 = 0; k2 < number_basisfunctions; k2++) {
					for (int l = 0; l < number_basisfunctions; l++) {
						double i_value = 0;
						for (int i = 0; i < number_basisfunctions; i++) {
							i_value += this->HF_C(i, i2) * two_e_integrals2(i, j, k2, l);
						}
						two_e_integrals3(i2, j, k2, l) = i_value;
					}
				}
			}
		}


		Eigen::Tensor<double, 4> two_e_integrals4(number_basisfunctions, number_basisfunctions, number_basisfunctions, number_basisfunctions);
		for (int i2 = 0; i2 < number_basisfunctions; i2++) {
			for (int j = 0; j < number_basisfunctions; j++) {
				for (int k2 = 0; k2 < number_basisfunctions; k2++) {
					for (int l2 = 0; l2 < number_basisfunctions; l2++) {
						double l_value = 0;
						for (int l = 0; l < number_basisfunctions; l++) {
							l_value += this->HF_C(l, l2) * two_e_integrals3(i2, j, k2, l);
						}
						two_e_integrals4(i2, j, k2, l2) = l_value;
					}
				}
			}
		}

		this->two_electron_integrals_hforbitals = Eigen::Tensor<double, 4>(number_basisfunctions, number_basisfunctions, number_basisfunctions, number_basisfunctions);
		for (int i2 = 0; i2 < number_basisfunctions; i2++) {
			for (int j2 = 0; j2 < number_basisfunctions; j2++) {
				for (int k2 = 0; k2 < number_basisfunctions; k2++) {
					for (int l2 = 0; l2 < number_basisfunctions; l2++) {
						double j_value = 0;
						for (int j = 0; j < number_basisfunctions; j++) {
							j_value += this->HF_C(j, j2) * two_e_integrals4(i2, j, k2, l2);
						}
						this->two_electron_integrals_hforbitals(i2, j2, k2, l2) = j_value;
					}
				}
			}
		}
	}
	void Transformation_Integrals_Basisfunction_To_HFOrbitals::compute()
	{
		compute_one_electron_integrals();
		compute_two_electron_integrals();
	}
}