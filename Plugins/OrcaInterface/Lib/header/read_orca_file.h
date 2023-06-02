#pragma once

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iterator>

#include "pointcharge.h"
#include "gaussian_basisset.h"
#include "configurations.h"

#include <Eigen/Dense>
#include <unsupported/Eigen/CXX11/Tensor>


namespace QC
{

	class ReadOrcaFile
	{
	private:
		std::string OrcaFilePath = "";
		std::vector<std::string> AllLinesOfFile;
		int electrons;
		std::vector<QC::Pointcharge> pointcharges;
		QC::Gaussian_Basisset basisset;
		double scf_energy;
		Eigen::VectorXd orbital_energys;
		Eigen::MatrixXd SCF_MO_Matrix;
		std::vector<QC::Configuration> CI_strings;
		Eigen::MatrixXd CI_Matrix;
		Eigen::VectorXd CI_Energys;

	public:
		void set_OrcaFilePath(std::string OrcaFilePath) {this->OrcaFilePath = OrcaFilePath;}
		void ReadFile();

		bool file_has_coordinates_section() { return has_coordinates_section; }
		bool file_has_basisset_section() { return has_basisset_section; }
		bool file_has_SCF_energy_section() { return has_SCF_energy_section; }
		bool file_has_orbital_energies_section() { return has_orbital_energies_section; }
		bool file_has_molecular_orbitals_section() { return has_molecular_orbitals_section; }
		bool file_has_tddft_excited_states_section() { return has_tddft_excited_states_section; }

		int get_electrons() { return electrons; }
		std::vector<QC::Pointcharge> get_pointcharges() { return pointcharges; }
		QC::Gaussian_Basisset get_basisset() { return basisset; }
		Eigen::MatrixXd get_SCF_MO_Matrix() { return SCF_MO_Matrix; }
		double get_scf_energy() { return scf_energy; }
		Eigen::VectorXd get_orbital_energys() { return orbital_energys; }
		std::vector<std::string> get_CI_strings() 
		{ 
			std::vector<std::string> CI_strings_return;
			for (int i = 0; i < CI_strings.size(); i++)
			{
				CI_strings_return.push_back(CI_strings[i].print_configuration_string());
			}
			return CI_strings_return;
		}
		Eigen::MatrixXd get_CI_Matrix() { return CI_Matrix; }
		Eigen::VectorXd get_CI_Energys() { return CI_Energys; }

	private:
		void ResetData();
		void ReadAll();
		void FindStartOfRelevantSections();
		std::vector<std::string> split_string_in_strings(std::string string, char delimiter);
		std::vector<std::string> split_string_in_strings(std::string string);
		std::vector<std::string> split_MO_table_line(std::string string);
		void change_rows_and_cols_in_MO_Matrix(int index1, int index2);

		bool are_maps_created = false;
		void create_maps();
		std::map<std::string, int> atom_charge_map;
		std::map<int, std::string> charge_atom_map;
		std::map<std::string, int> basisfunction_name_to_int;
		std::map<std::string, QC::Gaussian_Basisset> atom_to_basisfunction_map;

	private: //Section-Stuff
		bool has_coordinates_section;
		int coordinates_section_start_line;
		void read_coordinates_section();
		bool has_basisset_section;
		int basisset_section_start_line;
		void read_basisset_section();
		int read_basisset_to_one_atom(int start_line);
		bool has_SCF_energy_section;
		int SCF_energy_section_start_line;
		void read_SCF_energy_section();
		bool has_orbital_energies_section;
		int orbital_energies_section_start_line;
		void read_orbital_energies_section();
		bool has_molecular_orbitals_section;
		int molecular_orbitals_section_start_line;
		void read_molecular_orbitals_section();
		bool has_tddft_excited_states_section;
		int tddft_excited_states_section_start_line;
		void read_tddft_excited_states_section();
	};
}