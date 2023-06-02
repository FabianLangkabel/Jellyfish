#include "../header/configuration_interaction.h"
namespace QC
{

	/*
	* Creation of Configuration Strings
	*/

	void Configuration_Interaction::print_configuration(Configuration conf)
	{
		for (int j = 0; j < conf.alphastring.size(); j++)
		{
			if (conf.alphastring[j] == true) { std::cout << "1"; }
			else { std::cout << "0"; }
		}
		std::cout << " ";
		for (int j = 0; j < conf.betastring.size(); j++)
		{
			if (conf.betastring[j] == true) { std::cout << "1"; }
			else { std::cout << "0"; }
		}
		std::cout << std::endl;
	}

	void Configuration_Interaction::excite(int first_alphavirtuell_index, int first_betavirtuell_index, int lowest_alphaelectron_index, int lowest_betaelectron_index, int lowest_alphavirtuell_index, int lowest_betavirtuell_index, int excite_alpha, int excite_beta, Configuration configuration)
	{
		if (excite_alpha > 0)
		{
			for (int i = 0; i < lowest_alphaelectron_index; i++)
			{
				for (int a = first_alphavirtuell_index; a < lowest_alphavirtuell_index; a++)
				{
					Configuration conf = configuration;
					conf.alphastring[a] = true;
					conf.alphastring[i] = false;
					excite(first_alphavirtuell_index, first_betavirtuell_index, i, lowest_betaelectron_index, a, lowest_betavirtuell_index, excite_alpha - 1, excite_beta, conf);
				}
			}
		}
		if (excite_alpha == 0 && excite_beta > 0)
		{
			for (int i = 0; i < lowest_betaelectron_index; i++)
			{
				for (int a = first_betavirtuell_index; a < lowest_betavirtuell_index; a++)
				{
					Configuration conf = configuration;
					conf.betastring[a] = true;
					conf.betastring[i] = false;
					excite(first_alphavirtuell_index, first_betavirtuell_index, lowest_alphaelectron_index, i, lowest_alphavirtuell_index, a, excite_alpha, excite_beta - 1, conf);
				}
			}
		}
		if (excite_alpha == 0 && excite_beta == 0)
		{
			//print_configuration(configuration);
			this->configurations.push_back(configuration);
		}
	}

	void Configuration_Interaction::distribute_excitation(int alphaelectrons, int betaelectrons, int spartialorbitals, int excitation, Configuration hartreefockstate)
	{
		int max_excite_alpha = std::min({ excitation, alphaelectrons, spartialorbitals - alphaelectrons });
		int max_excite_beta = std::min({ excitation, betaelectrons, spartialorbitals - betaelectrons });

		for (int excite_alpha = 0; excite_alpha < max_excite_alpha + 1; excite_alpha++)
		{
			int excite_beta = excitation - excite_alpha;
			if (excite_beta < max_excite_beta + 1)
			{
				excite(alphaelectrons, betaelectrons, alphaelectrons, betaelectrons, spartialorbitals, spartialorbitals, excite_alpha, excite_beta, hartreefockstate);
			}
		}
	}

	/*
	* Evaluate Matrix Elements
	*/
	std::vector<int> Configuration_Interaction::get_occupied_orbitals(Configuration configuration1, Configuration configuration2)
	{
		std::vector<int> occupied;
		for (int i = 0; i < configuration1.alphastring.size(); i++)
		{
			if (configuration1.alphastring[i] && configuration2.alphastring[i]) { occupied.push_back(2 * i); }
			if (configuration1.betastring[i] && configuration2.betastring[i]) { occupied.push_back(2 * i + 1); }
		}
		return occupied;
	}

	int Configuration_Interaction::get_single_sign(Configuration conf1, int hole, int particle)
	{
		int sign = 1;
		int start, end;
		if (hole < particle) { start = hole; end = particle; }
		else { start = particle; end = hole;}
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

	std::tuple<int, int, int> Configuration_Interaction::get_single_set_params(Configuration configuration1, Configuration configuration2)
	{
		int particle, hole, sign;
		
		std::vector<int> holes_on_alpha_string, holes_on_beta_string, particles_on_alpha_string, particles_on_beta_string;

		for (int i = 0; i < configuration1.alphastring.size(); i++)
		{
			if (configuration1.alphastring[i] == true && configuration2.alphastring[i] == false) { holes_on_alpha_string.push_back(i); }
			if (configuration1.alphastring[i] == false && configuration2.alphastring[i] == true) { particles_on_alpha_string.push_back(i); }
			if (configuration1.betastring[i] == true && configuration2.betastring[i] == false) { holes_on_beta_string.push_back(i);}
			if (configuration1.betastring[i] == false && configuration2.betastring[i] == true) { particles_on_beta_string.push_back(i); }
		}

		if(holes_on_alpha_string.size() == 1)
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

	std::tuple<int, int, int, int, int> Configuration_Interaction::get_double_set_params(Configuration configuration1, Configuration configuration2)
	{
		int particle1, particle2, hole1, hole2, sign;

		std::vector<int> holes_on_alpha_string, holes_on_beta_string, particles_on_alpha_string, particles_on_beta_string;

		for (int i = 0; i < configuration1.alphastring.size(); i++)
		{
			if (configuration1.alphastring[i] == true && configuration2.alphastring[i] == false) { holes_on_alpha_string.push_back(i); }
			if (configuration1.alphastring[i] == false && configuration2.alphastring[i] == true) { particles_on_alpha_string.push_back(i); }
			if (configuration1.betastring[i] == true && configuration2.betastring[i] == false) { holes_on_beta_string.push_back(i); }
			if (configuration1.betastring[i] == false && configuration2.betastring[i] == true) { particles_on_beta_string.push_back(i); }
		}

		if (holes_on_alpha_string.size() == 2)
		{
			hole1 = holes_on_alpha_string[0] * 2;
			particle1 = particles_on_alpha_string[0] * 2;
			hole2 = holes_on_alpha_string[1] * 2;
			particle2 = particles_on_alpha_string[1] * 2;
			sign = get_single_sign(configuration1, hole1, particle1) * get_single_sign(configuration2, hole2, particle2);
		}
		else if (holes_on_alpha_string.size() == 1)
		{
			hole1 = holes_on_alpha_string[0] * 2;
			particle1 = particles_on_alpha_string[0] * 2;
			hole2 = holes_on_beta_string[0] * 2 + 1;
			particle2 = particles_on_beta_string[0] * 2 + 1;
			sign = get_single_sign(configuration1, hole1, particle1) * get_single_sign(configuration2, hole2, particle2);
		}
		else
		{
			hole1 = holes_on_beta_string[0] * 2 + 1;
			particle1 = particles_on_beta_string[0] * 2 + 1;
			hole2 = holes_on_beta_string[1] * 2 + 1;
			particle2 = particles_on_beta_string[1] * 2 + 1;
			sign = get_single_sign(configuration1, hole1, particle1) * get_single_sign(configuration2, hole2, particle2);
		}

		return std::make_tuple(particle1, particle2, hole1, hole2, sign);
	}

	double Configuration_Interaction::evaluate_states(Configuration configuration1, Configuration configuration2)
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
		double en = 0;
		if (diff == 0)
		{
			std::vector<int> occupied = get_occupied_orbitals(configuration1, configuration2);
			for (int i = 0; i < occupied.size(); i++) {
				en += this->one_electron_integrals_spinorbitals(occupied[i], occupied[i]);
			}
			for (int i = 0; i < occupied.size(); i++) {
				for (int j = 0; j < i; j++) {
					en += this->two_electron_integrals_spinorbitals(occupied[i], occupied[j], occupied[i], occupied[j]);
				}
			}
		}
		else if (diff == 1)
		{
			std::vector<int> occupied = get_occupied_orbitals(configuration1, configuration2);
			int particle, hole, sign;
			std::tie(particle, hole, sign) = get_single_set_params(configuration1, configuration2);
			
			en += this->one_electron_integrals_spinorbitals(hole, particle);
			for (int i = 0; i < occupied.size(); i++) {
				en += this->two_electron_integrals_spinorbitals(hole, occupied[i], particle, occupied[i]);
			}
			en = en * sign;
		}
		else if (diff == 2)
		{
			int particle1, particle2, hole1, hole2, sign;
			std::tie(particle1, particle2, hole1, hole2, sign) = get_double_set_params(configuration1, configuration2);

			en = this->two_electron_integrals_spinorbitals(hole1, hole2, particle1, particle2);
			en = en * sign;
		}
		else
		{
			en = 0;
		}
		return en;
	}

	void Configuration_Interaction::compute()
	{
		this->configurations.clear();
		this->configuration_strings.clear();

		
		int alphaelectrons = this->electrons / 2;
		int betaelectrons = this->electrons / 2;
		int spartialorbitals = this->one_electron_integrals_spinorbitals.rows() / 2;

		//Build HF-vector and add as first configuration
		std::vector<bool> alphastring, betastring;
		for (int i = 0; i < alphaelectrons; i++) {
			alphastring.push_back(true);
		}
		for (int i = alphaelectrons; i < spartialorbitals; i++) {
			alphastring.push_back(false);
		}

		for (int i = 0; i < betaelectrons; i++) {
			betastring.push_back(true);
		}
		for (int i = betaelectrons; i < spartialorbitals; i++) {
			betastring.push_back(false);
		}

		Configuration hartreefockstate;
		hartreefockstate.alphastring = alphastring;
		hartreefockstate.betastring = betastring;
		this->configurations.push_back(hartreefockstate);

		
		for (int excitation = 1; excitation < this->excitation_level + 1; excitation++)
		{
			distribute_excitation(alphaelectrons, betaelectrons, spartialorbitals, excitation, hartreefockstate);
		}

		for (int i = 0; i < this->configurations.size(); i++)
		{
			std::string configuration_string;
			Configuration conf = this->configurations[i];
			for (int j = 0; j < conf.alphastring.size(); j++)
			{
				if (conf.alphastring[j]) { configuration_string += "1"; }
				else { configuration_string += "0"; }
				if (conf.betastring[j]) { configuration_string += "1"; }
				else { configuration_string += "0"; }
				//configuration_string += " ";
			}
			this->configuration_strings.push_back(configuration_string);
		}

		Eigen::MatrixXd Hamilton_Matrix(this->configurations.size(), this->configurations.size());
		for (int i = 0; i < this->configurations.size(); i++) {
			for (int j = 0; j < this->configurations.size(); j++) {
				Hamilton_Matrix(i, j) = evaluate_states(this->configurations[i], this->configurations[j]);
			}
		}
		//std::cout << Hamilton_Matrix;


		Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> es;
		es.compute(Hamilton_Matrix);
		this->ci_matrix = es.eigenvectors();
		this->ci_values = es.eigenvalues();

		//Hier wird jetzt auf jeden Eigenwert noch E0 raufgerechnet... (Nochmal drüber nachdenken !!!)
		for (int i = 0; i < this->ci_values.rows(); i++) { this->ci_values[i] += this->E0; }
	}

	std::vector<std::string> Configuration_Interaction::get_all_ci_strings_from_excitation_level(int spinorbitals, int electrons, int excitation_level)
	{
		this->configurations.clear();
		this->configuration_strings.clear();

		int alphaelectrons = electrons / 2;
		int betaelectrons = electrons / 2;
		int spartialorbitals = spinorbitals / 2;

		//Build HF-vector and add as first configuration
		std::vector<bool> alphastring, betastring;
		for (int i = 0; i < alphaelectrons; i++) {
			alphastring.push_back(true);
		}
		for (int i = alphaelectrons; i < spartialorbitals; i++) {
			alphastring.push_back(false);
		}

		for (int i = 0; i < betaelectrons; i++) {
			betastring.push_back(true);
		}
		for (int i = betaelectrons; i < spartialorbitals; i++) {
			betastring.push_back(false);
		}

		Configuration hartreefockstate;
		hartreefockstate.alphastring = alphastring;
		hartreefockstate.betastring = betastring;
		this->configurations.push_back(hartreefockstate);


		for (int excitation = 1; excitation < excitation_level + 1; excitation++)
		{
			distribute_excitation(alphaelectrons, betaelectrons, spartialorbitals, excitation, hartreefockstate);
		}

		for (int i = 0; i < this->configurations.size(); i++)
		{
			std::string configuration_string;
			Configuration conf = this->configurations[i];
			for (int j = 0; j < conf.alphastring.size(); j++)
			{
				if (conf.alphastring[j]) { configuration_string += "1"; }
				else { configuration_string += "0"; }
				if (conf.betastring[j]) { configuration_string += "1"; }
				else { configuration_string += "0"; }
			}
			this->configuration_strings.push_back(configuration_string);
		}

		return this->configuration_strings;
	}
}