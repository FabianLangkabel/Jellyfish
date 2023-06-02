#include "../header/read_orca_file.h"

namespace QC
{
	void ReadOrcaFile::ReadFile()
	{
		if (!are_maps_created) { create_maps(); }
		ResetData();
		ReadAll();
		FindStartOfRelevantSections();
		if (has_coordinates_section) { 
			std::cout << "Start read Coordinate Section" << std::endl;
			read_coordinates_section();
			std::cout << "Finish read Coordinate Section" << std::endl;
		}
		if (has_basisset_section) { 
			std::cout << "Start read Basisset Section" << std::endl;
			read_basisset_section(); 
			std::cout << "Finish read Basisset Section" << std::endl;
		}
		if (has_SCF_energy_section) { 
			std::cout << "Start read SCF-Energy Section" << std::endl;
			read_SCF_energy_section(); 
			std::cout << "Finish read SCF-Energy Section" << std::endl;
		}
		if (has_orbital_energies_section) { 
			std::cout << "Start read Orbital-Energys Section" << std::endl;
			read_orbital_energies_section(); 
			std::cout << "Finish read Orbital-Energys Section" << std::endl;
		}
		if (has_molecular_orbitals_section) { 
			std::cout << "Start read MO Section" << std::endl;
			read_molecular_orbitals_section(); 
			std::cout << "Finish read MO Section" << std::endl;
		}
		if (has_tddft_excited_states_section) { 
			std::cout << "Start read Excited States Section" << std::endl;
			read_tddft_excited_states_section(); 
			std::cout << "Finish read Excited States Section" << std::endl;
		}
	}

	void ReadOrcaFile::ResetData()
	{
		AllLinesOfFile.clear();
		has_coordinates_section = false;
		coordinates_section_start_line = 0;
		has_basisset_section = false;
		basisset_section_start_line = 0;
		has_SCF_energy_section = false;
		SCF_energy_section_start_line = 0;
		has_orbital_energies_section = false;
		orbital_energies_section_start_line = 0;
		has_molecular_orbitals_section = false;
		molecular_orbitals_section_start_line = 0;
		has_tddft_excited_states_section = false;
		tddft_excited_states_section_start_line = 0;

		electrons = 0;
		pointcharges.clear();
		basisset.clear();
		atom_to_basisfunction_map.clear();
		scf_energy = 0;
		CI_strings.clear();
	}

	void ReadOrcaFile::ReadAll()
	{
		std::fstream OrcaFile(this->OrcaFilePath);
		std::string OrcaFile_line;
		int linenumber = 0;
		while (std::getline(OrcaFile, OrcaFile_line))
		{
			AllLinesOfFile.push_back(OrcaFile_line);
		}
	}

	void ReadOrcaFile::FindStartOfRelevantSections()
	{
		for (int i = 0; i < AllLinesOfFile.size(); i++)
		{
			if (AllLinesOfFile[i].find("CARTESIAN COORDINATES (ANGSTROEM)") != std::string::npos) {
				has_coordinates_section = true;
				coordinates_section_start_line = i;
			}
			//if (AllLinesOfFile[i].find("BASIS SET IN INPUT FORMAT") != std::string::npos) {
			if (AllLinesOfFile[i] == "BASIS SET IN INPUT FORMAT") {
				has_basisset_section = true;
				basisset_section_start_line = i;
			}
			if (AllLinesOfFile[i].find("TOTAL SCF ENERGY") != std::string::npos) {
				has_SCF_energy_section = true;
				SCF_energy_section_start_line = i;
			}
			if (AllLinesOfFile[i].find("ORBITAL ENERGIES") != std::string::npos) {
				has_orbital_energies_section = true;
				orbital_energies_section_start_line = i;
			}
			if (AllLinesOfFile[i].find("MOLECULAR ORBITALS") != std::string::npos) {
				has_molecular_orbitals_section = true;
				molecular_orbitals_section_start_line = i;
			}
			if (AllLinesOfFile[i].find("TD-DFT/TDA EXCITED STATES (SINGLETS)") != std::string::npos || AllLinesOfFile[i].find("CIS-EXCITED STATES (SINGLETS)") != std::string::npos) {
				has_tddft_excited_states_section = true;
				tddft_excited_states_section_start_line = i;
			}
		}
	}

	void ReadOrcaFile::read_coordinates_section()
	{
		int line_to_read = coordinates_section_start_line + 2;
		const double A_to_Bohr = 1.889725989;
		while (true)
		{
			std::string line = AllLinesOfFile[line_to_read];
			if (line == "")
				break;
			
			QC::Pointcharge charge;
			std::vector<std::string> splitted = split_string_in_strings(line);
			charge.set_center(std::stod(splitted[1]) * A_to_Bohr, std::stod(splitted[2]) * A_to_Bohr, std::stod(splitted[3]) * A_to_Bohr);
			charge.set_charge(atom_charge_map[splitted[0]]);
			electrons += atom_charge_map[splitted[0]];
			pointcharges.push_back(charge);
			line_to_read++;
		}
	}

	void ReadOrcaFile::read_basisset_section()
	{
		int line_to_read = basisset_section_start_line + 3;
		while (true)
		{
			if (AllLinesOfFile[line_to_read].find(" # Basis set for element : ") != std::string::npos) {
				line_to_read = read_basisset_to_one_atom(line_to_read);
				line_to_read += 2;
			}
			else
				break;
		}

		basisset.set_basisset_type(0); //ACHTUNG WIRD GERADE EINFACH ALS SPHERISCH GESETZT UND NICHT AUSGELESEN
		for (int i = 0; i < pointcharges.size(); i++)
		{
			std::vector<QC::Gaussian_Basisfunction> basisfuncs = atom_to_basisfunction_map[charge_atom_map[pointcharges[i].get_charge()]].get_all_Basisfunction_with_angular_compact();
			for (int j = 0; j < basisfuncs.size(); j++)
			{
				QC::Gaussian_Basisfunction func = basisfuncs[j];
				func.set_center(pointcharges[i].get_center_x(), pointcharges[i].get_center_y(), pointcharges[i].get_center_z());
				basisset.add_Basisfunction(func);
			}
		}

	}
	int ReadOrcaFile::read_basisset_to_one_atom(int start_line)
	{
		int line_to_read = start_line;
		std::string atomname = split_string_in_strings(AllLinesOfFile[line_to_read])[6];
		QC::Gaussian_Basisset basisset_for_atom;
		basisset_for_atom.set_basisset_type(0); //ACHTUNG WIRD GERADE EINFACH ALS SPHERISCH GESETZT UND NICHT AUSGELESEN
		line_to_read += 2;
		while (AllLinesOfFile[line_to_read] != "  end;")
		{
			QC::Gaussian_Basisfunction new_basisfunction;

			std::vector<std::string> header_line_splitted = split_string_in_strings(AllLinesOfFile[line_to_read]);
			int contration_level = std::stoi(header_line_splitted[1]);
			std::string function_type = header_line_splitted[0];
			line_to_read++;

			new_basisfunction.set_total_angular_moment(basisfunction_name_to_int[function_type]);

			for (int i = 0; i < contration_level; i++)
			{
				std::vector<std::string> line_splitted = split_string_in_strings(AllLinesOfFile[line_to_read]);
				double exp = std::stod(line_splitted[1]);
				double coef = std::stod(line_splitted[2]);
				new_basisfunction.add_parameterdouble(coef, exp);
				line_to_read++;
			}
			basisset_for_atom.add_Basisfunction(new_basisfunction);
		}

		atom_to_basisfunction_map[atomname] = basisset_for_atom;
		return line_to_read;
	}

	void ReadOrcaFile::read_SCF_energy_section()
	{
		scf_energy = std::stod(split_string_in_strings(AllLinesOfFile[SCF_energy_section_start_line + 3])[3]);
	}

	void ReadOrcaFile::read_orbital_energies_section()
	{
		int line_to_read = orbital_energies_section_start_line + 4;
		int number_mos = basisset.get_Basisfunctionnumber_angular_expanded();
		orbital_energys = Eigen::VectorXd::Zero(number_mos);
		for (int i = 0; i < number_mos; i++)
		{
			std::vector<std::string> line_splitted = split_string_in_strings(AllLinesOfFile[line_to_read]);
			orbital_energys(i) = std::stod(line_splitted[2]);
			line_to_read++;
		}
	}

	void ReadOrcaFile::read_molecular_orbitals_section()
	{
		int line_to_read = molecular_orbitals_section_start_line + 6;

		int number_mos = basisset.get_Basisfunctionnumber_angular_expanded();
		SCF_MO_Matrix = Eigen::MatrixXd::Zero(number_mos, number_mos);

		int number_rows_in_orca_file = number_mos / 6;
		int entrys_last_row = number_mos % 6;
		if (entrys_last_row != 0) { number_rows_in_orca_file++; }
		else { entrys_last_row = 6; }

		for (int row_in_orca_file = 0; row_in_orca_file < number_rows_in_orca_file; row_in_orca_file++)
		{
			int entrys_in_line = 6;
			if (row_in_orca_file == number_rows_in_orca_file - 1) { entrys_in_line = entrys_last_row; }

			for (int mo = 0; mo < number_mos; mo++)
			{
				std::vector<std::string> splitted = split_MO_table_line(AllLinesOfFile[line_to_read]);
				for (int entry = 0; entry < entrys_in_line; entry++)
				{
					SCF_MO_Matrix(mo, row_in_orca_file * 6 + entry) = std::stod(splitted[2 + entry]);
				}
				line_to_read++;
			}
			line_to_read += 4;
		}

		//Swap Cols and Rows to match internal Integral-Order
		std::vector<QC::Gaussian_Basisfunction> all_basisfunctions = basisset.get_all_Basisfunction_with_angular_compact();
		int idx = 0;
		if (basisset.get_basisset_type() == 0)
		{
			for (int i = 0; i < all_basisfunctions.size(); i++)
			{
				if (all_basisfunctions[i].get_total_angular_moment() == 0)
				{
					idx += 1;
				}
				else if (all_basisfunctions[i].get_total_angular_moment() == 1)
				{
					change_rows_and_cols_in_MO_Matrix(idx + 0, idx + 1);
					change_rows_and_cols_in_MO_Matrix(idx + 1, idx + 2);
					idx += 3;
				}
				else if (all_basisfunctions[i].get_total_angular_moment() == 2)
				{
					change_rows_and_cols_in_MO_Matrix(idx + 0, idx + 4);
					change_rows_and_cols_in_MO_Matrix(idx + 1, idx + 2);
					change_rows_and_cols_in_MO_Matrix(idx + 2, idx + 4);
					change_rows_and_cols_in_MO_Matrix(idx + 3, idx + 4);
					idx += 5;
				}
				else if (all_basisfunctions[i].get_total_angular_moment() == 3)
				{
					/*
					change_rows_and_cols_in_MO_Matrix(idx + 0, idx + 5);
					change_rows_and_cols_in_MO_Matrix(idx + 1, idx + 3);
					change_rows_and_cols_in_MO_Matrix(idx + 2, idx + 3);
					change_rows_and_cols_in_MO_Matrix(idx + 3, idx + 5);
					change_rows_and_cols_in_MO_Matrix(idx + 4, idx + 5);
					*/
					change_rows_and_cols_in_MO_Matrix(idx + 0, idx + 6);
					change_rows_and_cols_in_MO_Matrix(idx + 1, idx + 4);
					change_rows_and_cols_in_MO_Matrix(idx + 3, idx + 6);
					change_rows_and_cols_in_MO_Matrix(idx + 5, idx + 6);
					idx += 7;
				}
				//Add Exchange for higher Moments
			}
		}

		//std::cout << SCF_MO_Matrix << std::endl;

	}

	void ReadOrcaFile::read_tddft_excited_states_section()
	{
		//Build Closed HF Conf-Vec
		QC::Configuration hf_conf;
		for (int orb = 0; orb < basisset.get_Basisfunctionnumber_angular_expanded(); orb++)
		{
			if ((electrons / 2) > orb)
			{
				hf_conf.alphastring.push_back(true);
				hf_conf.betastring.push_back(true);
			}
			else
			{
				hf_conf.alphastring.push_back(false);
				hf_conf.betastring.push_back(false);
			}
		}
		CI_strings.push_back(hf_conf);

		//Save Vec of Exci
		std::vector<std::string> excitation_strings;

		//Read In first CI-Vector + Energy
		int line_to_read = tddft_excited_states_section_start_line + 5;
		double first_ci_energy = std::stod(split_string_in_strings(AllLinesOfFile[line_to_read])[3]);
		line_to_read++;
		std::vector<double> first_ci_vec_values;
		first_ci_vec_values.push_back(0); //CIS -> NO OVERLAP BETWEEN GS AND EXCITED CIS STATE
		
		while (AllLinesOfFile[line_to_read] != "")
		{

			std::vector<std::string> line_splitted = split_string_in_strings(AllLinesOfFile[line_to_read]);
			std::string hole = line_splitted[0];
			std::string particle = line_splitted[2];
			std::string c_val = line_splitted[6];

			excitation_strings.push_back(hole + " " + particle);

			int hole_pos = std::stoi(hole.substr(0, hole.size() - 1));
			int particle_pos = std::stoi(particle.substr(0, particle.size() - 1));
			c_val = c_val.substr(0, c_val.size() - 1);
			double c_val_insert = sqrt((std::stod(c_val) * std::stod(c_val)) / 2);
			if (std::stod(c_val) < 0) { c_val_insert = -1 * c_val_insert; }
			first_ci_vec_values.push_back(c_val_insert); //alpha excitation
			first_ci_vec_values.push_back(-c_val_insert);  //beta excitation

			QC::Configuration new_conf_string_a = hf_conf;
			new_conf_string_a.alphastring[hole_pos] = false;
			new_conf_string_a.alphastring[particle_pos] = true;
			CI_strings.push_back(new_conf_string_a);

			QC::Configuration new_conf_string_b = hf_conf;
			new_conf_string_b.betastring[hole_pos] = false;
			new_conf_string_b.betastring[particle_pos] = true;
			CI_strings.push_back(new_conf_string_b);

			line_to_read++;
		}
		line_to_read++;
		
		//Prepare CI-Matrix & Energys and insert first values
		CI_Matrix = Eigen::MatrixXd::Zero(CI_strings.size(), CI_strings.size());
		CI_Matrix(0, 0) = 1;//sqrt(2); //PROBLEMSTELLE
		
		CI_Energys = Eigen::VectorXd::Zero(CI_strings.size());
		CI_Energys(0) = scf_energy;
		CI_Energys(1) = scf_energy + first_ci_energy;

		for (int i = 1; i < CI_strings.size(); i++)
		{
			CI_Matrix(0, i) = 0;
			CI_Matrix(i, 0) = 0;
			CI_Matrix(i, 1) = first_ci_vec_values[i];
		}

		//Insert Rest
		int number_excitations = (CI_strings.size() - 1) / 2;

		for (int ci_vec_num = 2; ci_vec_num < number_excitations + 1; ci_vec_num++)
		{
			if(split_string_in_strings(AllLinesOfFile[line_to_read]).size() == 0) return;
			CI_Energys(ci_vec_num) = std::stod(split_string_in_strings(AllLinesOfFile[line_to_read])[3]) + scf_energy;
			line_to_read++;
			for (int exc = 0; exc < number_excitations; exc++)
			{
				std::vector<std::string> line_splitted = split_string_in_strings(AllLinesOfFile[line_to_read]);
				if (line_splitted.size() == 0)
				{
					CI_Matrix(1 + exc * 2, ci_vec_num) = 0;
					CI_Matrix(2 + exc * 2, ci_vec_num) = 0;
				}
				else
				{
					if (line_splitted[0] + " " + line_splitted[2] != excitation_strings[exc])
					{
						CI_Matrix(1 + exc * 2, ci_vec_num) = 0;
						CI_Matrix(2 + exc * 2, ci_vec_num) = 0;
					}
					else
					{
						std::string c_val = line_splitted[6];
						c_val = c_val.substr(0, c_val.size() - 1);
						double c_val_insert = sqrt((std::stod(c_val) * std::stod(c_val)) / 2);
						if (std::stod(c_val) < 0) { c_val_insert = -1 * c_val_insert; }
						CI_Matrix(1 + exc * 2, ci_vec_num) = c_val_insert;
						CI_Matrix(2 + exc * 2, ci_vec_num) = -c_val_insert;
						line_to_read++;
					}
				}
			}
			line_to_read++;
		}
	}







	void ReadOrcaFile::change_rows_and_cols_in_MO_Matrix(int index1, int index2)
	{
		//std::cout << "Change " << index1 << " with " << index2 << std::endl;

		int size = SCF_MO_Matrix.rows();
		for (int i = 0; i < size; i++)
		{
			double temp = SCF_MO_Matrix(index1, i);
			SCF_MO_Matrix(index1, i) = SCF_MO_Matrix(index2, i);
			SCF_MO_Matrix(index2, i) = temp;
		}

		//SCF_MO_Matrix.row(index1).swap(SCF_MO_Matrix.row(index2));
	}

	void ReadOrcaFile::create_maps()
	{
		atom_charge_map[std::string("H")] = 1;
		atom_charge_map[std::string("He")] = 2;
		atom_charge_map[std::string("Li")] = 3;
		atom_charge_map[std::string("Be")] = 4;
		atom_charge_map[std::string("B")] = 5;
		atom_charge_map[std::string("C")] = 6;
		atom_charge_map[std::string("N")] = 7;
		atom_charge_map[std::string("O")] = 8;
		atom_charge_map[std::string("F")] = 9;
		atom_charge_map[std::string("Ne")] = 10;
		atom_charge_map[std::string("Na")] = 11;
		atom_charge_map[std::string("Mg")] = 12;
		atom_charge_map[std::string("Al")] = 13;
		atom_charge_map[std::string("Si")] = 14;
		atom_charge_map[std::string("P")] = 15;
		atom_charge_map[std::string("S")] = 16;
		atom_charge_map[std::string("Cl")] = 17;
		atom_charge_map[std::string("Ar")] = 18;
		atom_charge_map[std::string("K")] = 19;
		atom_charge_map[std::string("Ca")] = 20;
		atom_charge_map[std::string("Sc")] = 21;
		atom_charge_map[std::string("Ti")] = 22;
		atom_charge_map[std::string("V")] = 23;
		atom_charge_map[std::string("Cr")] = 24;
		atom_charge_map[std::string("Mn")] = 25;
		atom_charge_map[std::string("Fe")] = 26;
		atom_charge_map[std::string("Co")] = 27;
		atom_charge_map[std::string("Ni")] = 28;
		atom_charge_map[std::string("Cu")] = 29;
		atom_charge_map[std::string("Zn")] = 30;
		atom_charge_map[std::string("Ga")] = 31;
		atom_charge_map[std::string("Ge")] = 32;
		atom_charge_map[std::string("As")] = 33;
		atom_charge_map[std::string("Se")] = 34;
		atom_charge_map[std::string("Br")] = 35;
		atom_charge_map[std::string("Kr")] = 36;
		atom_charge_map[std::string("Rb")] = 37;
		atom_charge_map[std::string("Sr")] = 38;
		atom_charge_map[std::string("Y")] = 39;
		atom_charge_map[std::string("Zr")] = 40;
		atom_charge_map[std::string("Nb")] = 41;
		atom_charge_map[std::string("Mo")] = 42;
		atom_charge_map[std::string("Tc")] = 43;
		atom_charge_map[std::string("Ru")] = 44;
		atom_charge_map[std::string("Rh")] = 45;
		atom_charge_map[std::string("Pd")] = 46;
		atom_charge_map[std::string("Ag")] = 47;
		atom_charge_map[std::string("Cd")] = 48;
		atom_charge_map[std::string("In")] = 49;
		atom_charge_map[std::string("Sn")] = 50;
		atom_charge_map[std::string("Sb")] = 51;
		atom_charge_map[std::string("Te")] = 52;
		atom_charge_map[std::string("I")] = 53;
		atom_charge_map[std::string("Xe")] = 54;
		atom_charge_map[std::string("Cs")] = 55;
		atom_charge_map[std::string("Ba")] = 56;
		atom_charge_map[std::string("La")] = 57;
		atom_charge_map[std::string("Ce")] = 58;
		atom_charge_map[std::string("Pr")] = 59;
		atom_charge_map[std::string("Nd")] = 60;
		atom_charge_map[std::string("Pm")] = 61;
		atom_charge_map[std::string("Sm")] = 62;
		atom_charge_map[std::string("Eu")] = 63;
		atom_charge_map[std::string("Gd")] = 64;
		atom_charge_map[std::string("Tb")] = 65;
		atom_charge_map[std::string("Dy")] = 66;
		atom_charge_map[std::string("Ho")] = 67;
		atom_charge_map[std::string("Er")] = 68;
		atom_charge_map[std::string("Tm")] = 69;
		atom_charge_map[std::string("Yb")] = 70;
		atom_charge_map[std::string("Lu")] = 71;
		atom_charge_map[std::string("Hf")] = 72;
		atom_charge_map[std::string("Ta")] = 73;
		atom_charge_map[std::string("W")] = 74;
		atom_charge_map[std::string("Re")] = 75;
		atom_charge_map[std::string("Os")] = 76;
		atom_charge_map[std::string("Ir")] = 77;
		atom_charge_map[std::string("Pt")] = 78;
		atom_charge_map[std::string("Au")] = 79;
		atom_charge_map[std::string("Hg")] = 80;
		atom_charge_map[std::string("Tl")] = 81;
		atom_charge_map[std::string("Pb")] = 82;
		atom_charge_map[std::string("Bi")] = 83;
		atom_charge_map[std::string("Po")] = 84;
		atom_charge_map[std::string("At")] = 85;
		atom_charge_map[std::string("Rn")] = 86;
		atom_charge_map[std::string("Fr")] = 87;
		atom_charge_map[std::string("Ra")] = 88;
		atom_charge_map[std::string("Ac")] = 89;
		atom_charge_map[std::string("Th")] = 90;
		atom_charge_map[std::string("Pa")] = 91;
		atom_charge_map[std::string("U")] = 92;
		atom_charge_map[std::string("Np")] = 93;
		atom_charge_map[std::string("Pu")] = 94;
		atom_charge_map[std::string("Am")] = 95;
		atom_charge_map[std::string("Cm")] = 96;
		atom_charge_map[std::string("Bk")] = 97;
		atom_charge_map[std::string("Cf")] = 98;
		atom_charge_map[std::string("Es")] = 99;
		atom_charge_map[std::string("Fm")] = 100;
		atom_charge_map[std::string("Md")] = 101;
		atom_charge_map[std::string("No")] = 102;
		atom_charge_map[std::string("Lr")] = 103;
		atom_charge_map[std::string("Rf")] = 104;
		atom_charge_map[std::string("Db")] = 105;
		atom_charge_map[std::string("Sg")] = 106;
		atom_charge_map[std::string("Bh")] = 107;
		atom_charge_map[std::string("Hs")] = 108;
		atom_charge_map[std::string("Mt")] = 109;
		atom_charge_map[std::string("Ds")] = 110;
		atom_charge_map[std::string("Rg")] = 111;
		atom_charge_map[std::string("Cn")] = 112;
		atom_charge_map[std::string("Nh")] = 113;
		atom_charge_map[std::string("Fl")] = 114;
		atom_charge_map[std::string("Mc")] = 115;
		atom_charge_map[std::string("Lv")] = 116;
		atom_charge_map[std::string("Ts")] = 117;
		atom_charge_map[std::string("Og")] = 118;

		charge_atom_map[1] = std::string("H");
		charge_atom_map[2] = std::string("He");
		charge_atom_map[3] = std::string("Li");
		charge_atom_map[4] = std::string("Be");
		charge_atom_map[5] = std::string("B");
		charge_atom_map[6] = std::string("C");
		charge_atom_map[7] = std::string("N");
		charge_atom_map[8] = std::string("O");
		charge_atom_map[9] = std::string("F");
		charge_atom_map[10] = std::string("Ne");
		charge_atom_map[11] = std::string("Na");
		charge_atom_map[12] = std::string("Mg");
		charge_atom_map[13] = std::string("Al");
		charge_atom_map[14] = std::string("Si");
		charge_atom_map[15] = std::string("P");
		charge_atom_map[16] = std::string("S");
		charge_atom_map[17] = std::string("Cl");
		charge_atom_map[18] = std::string("Ar");
		charge_atom_map[19] = std::string("K");
		charge_atom_map[20] = std::string("Ca");
		charge_atom_map[21] = std::string("Sc");
		charge_atom_map[22] = std::string("Ti");
		charge_atom_map[23] = std::string("V");
		charge_atom_map[24] = std::string("Cr");
		charge_atom_map[25] = std::string("Mn");
		charge_atom_map[26] = std::string("Fe");
		charge_atom_map[27] = std::string("Co");
		charge_atom_map[28] = std::string("Ni");
		charge_atom_map[29] = std::string("Cu");
		charge_atom_map[30] = std::string("Zn");
		charge_atom_map[31] = std::string("Ga");
		charge_atom_map[32] = std::string("Ge");
		charge_atom_map[33] = std::string("As");
		charge_atom_map[34] = std::string("Se");
		charge_atom_map[35] = std::string("Br");
		charge_atom_map[36] = std::string("Kr");
		charge_atom_map[37] = std::string("Rb");
		charge_atom_map[38] = std::string("Sr");
		charge_atom_map[39] = std::string("Y");
		charge_atom_map[40] = std::string("Zr");
		charge_atom_map[41] = std::string("Nb");
		charge_atom_map[42] = std::string("Mo");
		charge_atom_map[43] = std::string("Tc");
		charge_atom_map[44] = std::string("Ru");
		charge_atom_map[45] = std::string("Rh");
		charge_atom_map[46] = std::string("Pd");
		charge_atom_map[47] = std::string("Ag");
		charge_atom_map[48] = std::string("Cd");
		charge_atom_map[49] = std::string("In");
		charge_atom_map[50] = std::string("Sn");
		charge_atom_map[51] = std::string("Sb");
		charge_atom_map[52] = std::string("Te");
		charge_atom_map[53] = std::string("I");
		charge_atom_map[54] = std::string("Xe");
		charge_atom_map[55] = std::string("Cs");
		charge_atom_map[56] = std::string("Ba");
		charge_atom_map[57] = std::string("La");
		charge_atom_map[58] = std::string("Ce");
		charge_atom_map[59] = std::string("Pr");
		charge_atom_map[60] = std::string("Nd");
		charge_atom_map[61] = std::string("Pm");
		charge_atom_map[62] = std::string("Sm");
		charge_atom_map[63] = std::string("Eu");
		charge_atom_map[64] = std::string("Gd");
		charge_atom_map[65] = std::string("Tb");
		charge_atom_map[66] = std::string("Dy");
		charge_atom_map[67] = std::string("Ho");
		charge_atom_map[68] = std::string("Er");
		charge_atom_map[69] = std::string("Tm");
		charge_atom_map[70] = std::string("Yb");
		charge_atom_map[71] = std::string("Lu");
		charge_atom_map[72] = std::string("Hf");
		charge_atom_map[73] = std::string("Ta");
		charge_atom_map[74] = std::string("W");
		charge_atom_map[75] = std::string("Re");
		charge_atom_map[76] = std::string("Os");
		charge_atom_map[77] = std::string("Ir");
		charge_atom_map[78] = std::string("Pt");
		charge_atom_map[79] = std::string("Au");
		charge_atom_map[80] = std::string("Hg");
		charge_atom_map[81] = std::string("Tl");
		charge_atom_map[82] = std::string("Pb");
		charge_atom_map[83] = std::string("Bi");
		charge_atom_map[84] = std::string("Po");
		charge_atom_map[85] = std::string("At");
		charge_atom_map[86] = std::string("Rn");
		charge_atom_map[87] = std::string("Fr");
		charge_atom_map[88] = std::string("Ra");
		charge_atom_map[89] = std::string("Ac");
		charge_atom_map[90] = std::string("Th");
		charge_atom_map[91] = std::string("Pa");
		charge_atom_map[92] = std::string("U");
		charge_atom_map[93] = std::string("Np");
		charge_atom_map[94] = std::string("Pu");
		charge_atom_map[95] = std::string("Am");
		charge_atom_map[96] = std::string("Cm");
		charge_atom_map[97] = std::string("Bk");
		charge_atom_map[98] = std::string("Cf");
		charge_atom_map[99] = std::string("Es");
		charge_atom_map[100] = std::string("Fm");
		charge_atom_map[101] = std::string("Md");
		charge_atom_map[102] = std::string("No");
		charge_atom_map[103] = std::string("Lr");
		charge_atom_map[104] = std::string("Rf");
		charge_atom_map[105] = std::string("Db");
		charge_atom_map[106] = std::string("Sg");
		charge_atom_map[107] = std::string("Bh");
		charge_atom_map[108] = std::string("Hs");
		charge_atom_map[109] = std::string("Mt");
		charge_atom_map[110] = std::string("Ds");
		charge_atom_map[111] = std::string("Rg");
		charge_atom_map[112] = std::string("Cn");
		charge_atom_map[113] = std::string("Nh");
		charge_atom_map[114] = std::string("Fl");
		charge_atom_map[115] = std::string("Mc");
		charge_atom_map[116] = std::string("Lv");
		charge_atom_map[117] = std::string("Ts");
		charge_atom_map[118] = std::string("Og");

		basisfunction_name_to_int[std::string("S")] = 0;
		basisfunction_name_to_int[std::string("P")] = 1;
		basisfunction_name_to_int[std::string("D")] = 2;
		basisfunction_name_to_int[std::string("F")] = 3;
		basisfunction_name_to_int[std::string("G")] = 4;
		basisfunction_name_to_int[std::string("H")] = 5;
	}

	std::vector<std::string> ReadOrcaFile::split_string_in_strings(std::string string, char delimiter)
	{
		std::vector<std::string> return_vec;
		std::istringstream iss(string);
		std::string s;
		while (std::getline(iss, s, delimiter)) {
			return_vec.push_back(s);
		}
		return return_vec;
	}

	std::vector<std::string> ReadOrcaFile::split_string_in_strings(std::string string)
	{
		std::vector<std::string> return_vec;
		std::istringstream iss(string);
		std::string s;
		while (std::getline(iss, s, (char)0x20)) {
			if(s != "")
				return_vec.push_back(s);
		}
		return return_vec;
	}

	std::vector<std::string> ReadOrcaFile::split_MO_table_line(std::string string)
	{
		std::vector<std::string> return_vec;
		int string_lenght = string.length();

		return_vec.push_back(string.substr(0, 7)); //Stimmt zwar nicht immer wird aber ehe nicht verwendet
		return_vec.push_back(string.substr(7, 9)); //Stimmt zwar nicht immer wird aber ehe nicht verwendet

		std::string rest = string;
		rest.erase(0, 14); //Remove first 14 Char

		std::string::iterator end_pos = std::remove(rest.begin(), rest.end(), ' ');
		rest.erase(end_pos, rest.end()); //Remove Spaces

		{
			std::string in = "";
			bool after_dot = false;
			int after_dot_int = 0;
			for (int i = 0; i < rest.size(); i++)
			{
				if (after_dot_int == 7)
				{
					//std::cout << in << std::endl;
					return_vec.push_back(in);
					after_dot = false;
					in = "";
					after_dot_int = 0;
				}
				if (rest[i] == '.')
				{
					after_dot = true;
				}
				if (after_dot)
				{
					after_dot_int++;
				}
				in += rest[i];
			}
			//std::cout << in << std::endl;
			return_vec.push_back(in);
		}

		/*
		if (string_lenght > 17) { return_vec.push_back(string.substr(16, 10)); }
		if (string_lenght > 27) { return_vec.push_back(string.substr(26, 10)); }
		if (string_lenght > 37) { return_vec.push_back(string.substr(36, 10)); }
		if (string_lenght > 47) { return_vec.push_back(string.substr(46, 10)); }
		if (string_lenght > 57) { return_vec.push_back(string.substr(56, 10)); }
		if (string_lenght > 67) { return_vec.push_back(string.substr(66, 10)); }
		*/

		return return_vec;
	}
}