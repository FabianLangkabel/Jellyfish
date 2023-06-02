#include "../header/configuration_interaction_dipole_moment.h"
#include "time.h"

namespace QC
{
	
	std::vector<int> Configuration_Interaction_Dipole_Moment::get_occupied_orbitals(Configuration configuration1, Configuration configuration2)
	{
		std::vector<int> occupied;
		for (int i = 0; i < configuration1.alphastring.size(); i++)
		{
			if (configuration1.alphastring[i] && configuration2.alphastring[i]) { occupied.push_back(2 * i); }
			if (configuration1.betastring[i] && configuration2.betastring[i]) { occupied.push_back(2 * i + 1); }
		}
		return occupied;
	}

	int Configuration_Interaction_Dipole_Moment::get_single_sign(Configuration conf1, int hole, int particle)
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

	std::tuple<int, int, int> Configuration_Interaction_Dipole_Moment::get_single_set_params(Configuration configuration1, Configuration configuration2)
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

	Eigen::VectorXd Configuration_Interaction_Dipole_Moment::evaluate_states_dipole_moment(Configuration configuration1, Configuration configuration2)
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
		Eigen::VectorXd en(3);
		en(0) = 0;
		en(1) = 0;
		en(2) = 0;
		if (diff == 0)
		{
			std::vector<int> occupied = get_occupied_orbitals(configuration1, configuration2);
			for (int i = 0; i < occupied.size(); i++) {
				en(0) -= (this->one_electron_integrals_spinorbitals_x(occupied[i], occupied[i]) - center_of_mass(0));
				en(1) -= (this->one_electron_integrals_spinorbitals_y(occupied[i], occupied[i]) - center_of_mass(1));
				en(2) -= (this->one_electron_integrals_spinorbitals_z(occupied[i], occupied[i]) - center_of_mass(2));
			}
		}
		else if (diff == 1)
		{
			std::vector<int> occupied = get_occupied_orbitals(configuration1, configuration2);
			int particle, hole, sign;
			std::tie(particle, hole, sign) = get_single_set_params(configuration1, configuration2);

			en(0) -= this->one_electron_integrals_spinorbitals_x(hole, particle);
			en(1) -= this->one_electron_integrals_spinorbitals_y(hole, particle);
			en(2) -= this->one_electron_integrals_spinorbitals_z(hole, particle);
			en = en * sign;
		}
		else
		{
			en(0) = 0;
			en(1) = 0;
			en(2) = 0;
		}
		return en;
	}

	void Configuration_Interaction_Dipole_Moment::compute_electronic_dipole_moment()
	{

		std::vector<Eigen::MatrixXd> Slater_Matrices;
		
		for (int k = 0; k < 3; k++) {
			Slater_Matrices.push_back(Eigen::MatrixXd::Zero(this->configurations.size(), this->configurations.size()));
		}

		Eigen::VectorXd Slater_Matrices_Vector;
		for (int i = 0; i < configurations.size(); i++) {
			for (int j = 0; j < configurations.size(); j++) {
				Slater_Matrices_Vector = evaluate_states_dipole_moment(this->configurations[i], this->configurations[j]);
				for (int k = 0; k < 3; k++) {
					Slater_Matrices[k](i, j) = Slater_Matrices_Vector(k);
				}
			}
		}

		for (int k = 0; k < 3; k++) {
			this->Dipole_Matrices.push_back(this->CI_Matrix.transpose() * Slater_Matrices[k] * this->CI_Matrix);
		}

	}

	void Configuration_Interaction_Dipole_Moment::compute()
	{
		//Calculate Center of Mass
		center_of_mass = Eigen::Vector3d::Zero();
		if (Pointcharges.size() > 0)
		{
			double total_mass = 0;
			for (int i = 0; i < Pointcharges.size(); i++)
			{
				center_of_mass += Pointcharges[i].get_mass() * Pointcharges[i].get_center_as_vec();
				total_mass += Pointcharges[i].get_mass();
			}
			center_of_mass = center_of_mass / total_mass;
		}

		//Calculate Electronic Dipole-Moment
		compute_electronic_dipole_moment();


		//Add Nuclear Dipole Moment
		if (Pointcharges.size() > 0)
		{
			Eigen::Vector3d nuclear_dipole = Eigen::Vector3d::Zero();
			for (int i = 0; i < Pointcharges.size(); i++)
			{
				nuclear_dipole += Pointcharges[i].get_charge() * (Pointcharges[i].get_center_as_vec() - center_of_mass);

			}
			for (int k = 0; k < 3; k++) {
				for (int i = 0; i < this->Dipole_Matrices[k].rows(); i++)
				{
					this->Dipole_Matrices[k](i, i) += nuclear_dipole(k);
				}
			}
		}

		/*
		std::vector<Eigen::MatrixXd> Nuclear_Dipole_Matrices;
		for (int k = 0; k < 3; k++) {
			Nuclear_Dipole_Matrices.push_back(Eigen::MatrixXd::Zero(this->configurations.size(), this->configurations.size()));
			for (int i = 0; i < this->configurations.size(); i++) {
				Nuclear_Dipole_Matrices[k](i, i) = this->Dipole_Moment_Nuclear[k];
			}
			this->Dipole_Matrices.push_back(this->Electronic_Dipole_Matrices[k] + Nuclear_Dipole_Matrices[k]);
		}
		*/

	}

}