#include "../header/configuration_interaction_one_electron_operator.h"

namespace QC
{
	
	std::vector<int> Configuration_Interaction_One_Electron_Operator::get_occupied_orbitals(Configuration configuration1, Configuration configuration2)
	{
		std::vector<int> occupied;
		for (int i = 0; i < configuration1.alphastring.size(); i++)
		{
			if (configuration1.alphastring[i] && configuration2.alphastring[i]) { occupied.push_back(2 * i); }
			if (configuration1.betastring[i] && configuration2.betastring[i]) { occupied.push_back(2 * i + 1); }
		}
		return occupied;
	}

	int Configuration_Interaction_One_Electron_Operator::get_single_sign(Configuration conf1, int hole, int particle)
	{
		int sign = 1;
		int start, end;
		if (hole < particle) { start = hole; end = particle; }
		else { start = particle; end = hole; }
		for (int i = start + 1; i < end; i++)
		{
			int pos = i / 2;
			int spin = i % 2;
			if (spin == 0)
			{
				if (conf1.alphastring[pos] == true) { sign = -1 * sign; }
			}
			else
			{
				if (conf1.betastring[pos] == true) { sign = -1 * sign; }
			}
		}
		return sign;
	}

	std::tuple<int, int, int> Configuration_Interaction_One_Electron_Operator::get_single_set_params(Configuration configuration1, Configuration configuration2)
	{
		int particle, hole, sign;

		std::vector<int> holes_on_alpha_string, holes_on_beta_string, particles_on_alpha_string, particles_on_beta_string;

		for (int i = 0; i < configuration1.alphastring.size(); i++)
		{
			if (configuration1.alphastring[i] == true && configuration2.alphastring[i] == false) { holes_on_alpha_string.push_back(i); }
			if (configuration1.alphastring[i] == false && configuration2.alphastring[i] == true) { particles_on_alpha_string.push_back(i); }
			if (configuration1.betastring[i] == true && configuration2.betastring[i] == false) { holes_on_beta_string.push_back(i); }
			if (configuration1.betastring[i] == false && configuration2.betastring[i] == true) { particles_on_beta_string.push_back(i); }
		}

		if (holes_on_alpha_string.size() == 1)
		{
			hole = holes_on_alpha_string[0] * 2;
			particle = particles_on_alpha_string[0] * 2;
			sign = get_single_sign(configuration1, hole, particle);
		}
		else
		{
			hole = holes_on_beta_string[0] * 2 + 1;
			particle = particles_on_beta_string[0] * 2 + 1;
			sign = get_single_sign(configuration1, hole, particle);
		}

		return std::make_tuple(particle, hole, sign);
	}

	double Configuration_Interaction_One_Electron_Operator::evaluate_states_one_electron_operator(Configuration configuration1, Configuration configuration2)
	{
		//Calculate amount diff Orbs
		int diff = 0;
		for (int i = 0; i < configuration1.alphastring.size(); i++)
		{
			if (configuration1.alphastring[i] != configuration2.alphastring[i]) { diff++; }
			if (configuration1.betastring[i] != configuration2.betastring[i]) { diff++; }
		}
		diff = diff / 2;

		//Slater-Condon-Rules
		double en;
		en = 0;
		if (diff == 0)
		{
			std::vector<int> occupied = get_occupied_orbitals(configuration1, configuration2);
			for (int i = 0; i < occupied.size(); i++) {
				en += this->one_electron_integrals_spinorbitals(occupied[i], occupied[i]);
			}
		}
		else if (diff == 1)
		{
			int particle, hole, sign;
			std::tie(particle, hole, sign) = get_single_set_params(configuration1, configuration2);

			en += this->one_electron_integrals_spinorbitals(hole, particle);
			en = en * sign;
		}
		else
		{
			en = 0;
		}
		return en;
	}

	void Configuration_Interaction_One_Electron_Operator::compute()
	{
		
		Eigen::MatrixXcd Slater_Matrix = Eigen::MatrixXcd::Zero(this->configurations.size(), this->configurations.size());
		
		for (int i = 0; i < this->configurations.size(); i++) {
			for (int j = 0; j < this->configurations.size(); j++) {
				Slater_Matrix(i, j) = evaluate_states_one_electron_operator(this->configurations[i], this->configurations[j]);
				}
			}

		this->One_Electron_Operator_Matrix = this->CI_Matrix.transpose() * Slater_Matrix * this->CI_Matrix;

	}

}