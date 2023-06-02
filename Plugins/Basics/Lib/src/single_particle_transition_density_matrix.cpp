#include "../header/single_particle_transition_density_matrix.h"

namespace QC
{

	void Single_Particle_Transition_Density_Matrix::compute()
	{
		//old Method
		/*
		{
			int occupied_spin_orbitals = 0;
			std::vector<bool> HF_alpha_string = this->configurations[0].alphastring;
			std::vector<bool> HF_beta_string = this->configurations[0].betastring;
			for (int i = 0; i < HF_alpha_string.size(); i++)
			{
				if (HF_alpha_string[i]) { occupied_spin_orbitals += 2; }
			}
			int unoccupied_spin_orbitals = 2 * HF_alpha_string.size() - occupied_spin_orbitals;
			int occupied_spartial_orbitals = occupied_spin_orbitals / 2;
			int unoccupied_spartial_orbitals = unoccupied_spin_orbitals / 2;

			OnePTDM = Eigen::MatrixXd::Zero(occupied_spartial_orbitals, unoccupied_spartial_orbitals);

			for (int occ = 0; occ < occupied_spartial_orbitals; occ++)
			{
				for (int virt = 0; virt < unoccupied_spartial_orbitals; virt++)
				{
					for (int conf = 0; conf < this->configurations.size(); conf++)
					{
						if (this->configurations[conf].alphastring[occ] == false && this->configurations[conf].alphastring[occupied_spartial_orbitals + virt] == true)
						{
							OnePTDM(occ, virt) += this->CI_Matrix(conf, this->FinalState) * (1 / sqrt(2));
						}
						else if (this->configurations[conf].betastring[occ] == false && this->configurations[conf].betastring[occupied_spartial_orbitals + virt] == true)
						{
							OnePTDM(occ, virt) -= this->CI_Matrix(conf, this->FinalState) * (1 / sqrt(2));
						}
					}
				}
			}

			std::cout << OnePTDM << std::endl << std::endl;
		}
		*/

		//new Method
		{
			int occupied_spin_orbitals = 0;
			std::string HF_String = this->configurations[0].print_configuration_string();
			std::vector<bool> HF_alpha_string = this->configurations[0].alphastring;
			std::vector<bool> HF_beta_string = this->configurations[0].betastring;
			for (int i = 0; i < HF_alpha_string.size(); i++)
			{
				if (HF_alpha_string[i]) { occupied_spin_orbitals += 2; }
			}
			int unoccupied_spin_orbitals = 2 * HF_alpha_string.size() - occupied_spin_orbitals;
			int occupied_spartial_orbitals = occupied_spin_orbitals / 2;
			int unoccupied_spartial_orbitals = unoccupied_spin_orbitals / 2;

			std::map<std::string, int> conf_map;
			for (int i = 0; i < this->configurations.size(); i++)
			{
				std::string conf = this->configurations[i].print_configuration_string();
				conf_map[conf] = i;
			}

			OnePTDM = Eigen::MatrixXd::Zero(occupied_spartial_orbitals, unoccupied_spartial_orbitals);

			for (int occ = 0; occ < occupied_spartial_orbitals; occ++)
			{
				for (int virt = 0; virt < unoccupied_spartial_orbitals; virt++)
				{
					std::string alphastring = HF_String;
					alphastring[2 * occ] = '0';
					alphastring[2 * (occupied_spartial_orbitals + virt)] = '1';

					std::string betastring = HF_String;
					betastring[2 * occ + 1] = '0';
					betastring[2 * (occupied_spartial_orbitals + virt) + 1] = '1';

					OnePTDM(occ, virt) = (1 / sqrt(2)) * this->CI_Matrix(conf_map[alphastring], this->FinalState) - (1 / sqrt(2)) * this->CI_Matrix(conf_map[betastring], this->FinalState);
				}
			}
			//std::cout << OnePTDM << std::endl;
		}
	}

}