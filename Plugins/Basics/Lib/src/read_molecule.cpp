#include "../header/read_molecule.h"
namespace QC
{
	void Read_Molecule::compute()
	{
		this->atomnames_to_pointcharges.clear();
		this->Pointcharges.clear();
		this->Basisset.clear();
		this->electrons = 0;

		//Dictionary for Atoms to Charge
		std::map<std::string, int> atom_to_charge;
		atom_to_charge[std::string("H")] = 1;
		atom_to_charge[std::string("He")] = 2;
		atom_to_charge[std::string("Li")] = 3;
		atom_to_charge[std::string("Be")] = 4;
		atom_to_charge[std::string("B")] = 5;
		atom_to_charge[std::string("C")] = 6;
		atom_to_charge[std::string("N")] = 7;
		atom_to_charge[std::string("O")] = 8;
		atom_to_charge[std::string("F")] = 9;
		atom_to_charge[std::string("Ne")] = 10;
		atom_to_charge[std::string("Na")] = 11;
		atom_to_charge[std::string("Mg")] = 12;
		atom_to_charge[std::string("Al")] = 13;
		atom_to_charge[std::string("Si")] = 14;
		atom_to_charge[std::string("P")] = 15;
		atom_to_charge[std::string("S")] = 16;
		atom_to_charge[std::string("Cl")] = 17;
		atom_to_charge[std::string("Ar")] = 18;
		atom_to_charge[std::string("K")] = 19;
		atom_to_charge[std::string("Ca")] = 20;
		atom_to_charge[std::string("Sc")] = 21;
		atom_to_charge[std::string("Ti")] = 22;
		atom_to_charge[std::string("V")] = 23;
		atom_to_charge[std::string("Cr")] = 24;
		atom_to_charge[std::string("Mn")] = 25;
		atom_to_charge[std::string("Fe")] = 26;
		atom_to_charge[std::string("Co")] = 27;
		atom_to_charge[std::string("Ni")] = 28;
		atom_to_charge[std::string("Cu")] = 29;
		atom_to_charge[std::string("Zn")] = 30;
		atom_to_charge[std::string("Ga")] = 31;
		atom_to_charge[std::string("Ge")] = 32;
		atom_to_charge[std::string("As")] = 33;
		atom_to_charge[std::string("Se")] = 34;
		atom_to_charge[std::string("Br")] = 35;
		atom_to_charge[std::string("Kr")] = 36;
		atom_to_charge[std::string("Rb")] = 37;
		atom_to_charge[std::string("Sr")] = 38;
		atom_to_charge[std::string("Y")] = 39;
		atom_to_charge[std::string("Zr")] = 40;
		atom_to_charge[std::string("Nb")] = 41;
		atom_to_charge[std::string("Mo")] = 42;
		atom_to_charge[std::string("Tc")] = 43;
		atom_to_charge[std::string("Ru")] = 44;
		atom_to_charge[std::string("Rh")] = 45;
		atom_to_charge[std::string("Pd")] = 46;
		atom_to_charge[std::string("Ag")] = 47;
		atom_to_charge[std::string("Cd")] = 48;
		atom_to_charge[std::string("In")] = 49;
		atom_to_charge[std::string("Sn")] = 50;
		atom_to_charge[std::string("Sb")] = 51;
		atom_to_charge[std::string("Te")] = 52;
		atom_to_charge[std::string("I")] = 53;
		atom_to_charge[std::string("Xe")] = 54;
		atom_to_charge[std::string("Cs")] = 55;
		atom_to_charge[std::string("Ba")] = 56;
		atom_to_charge[std::string("La")] = 57;
		atom_to_charge[std::string("Ce")] = 58;
		atom_to_charge[std::string("Pr")] = 59;
		atom_to_charge[std::string("Nd")] = 60;
		atom_to_charge[std::string("Pm")] = 61;
		atom_to_charge[std::string("Sm")] = 62;
		atom_to_charge[std::string("Eu")] = 63;
		atom_to_charge[std::string("Gd")] = 64;
		atom_to_charge[std::string("Tb")] = 65;
		atom_to_charge[std::string("Dy")] = 66;
		atom_to_charge[std::string("Ho")] = 67;
		atom_to_charge[std::string("Er")] = 68;
		atom_to_charge[std::string("Tm")] = 69;
		atom_to_charge[std::string("Yb")] = 70;
		atom_to_charge[std::string("Lu")] = 71;
		atom_to_charge[std::string("Hf")] = 72;
		atom_to_charge[std::string("Ta")] = 73;
		atom_to_charge[std::string("W")] = 74;
		atom_to_charge[std::string("Re")] = 75;
		atom_to_charge[std::string("Os")] = 76;
		atom_to_charge[std::string("Ir")] = 77;
		atom_to_charge[std::string("Pt")] = 78;
		atom_to_charge[std::string("Au")] = 79;
		atom_to_charge[std::string("Hg")] = 80;
		atom_to_charge[std::string("Tl")] = 81;
		atom_to_charge[std::string("Pb")] = 82;
		atom_to_charge[std::string("Bi")] = 83;
		atom_to_charge[std::string("Po")] = 84;
		atom_to_charge[std::string("At")] = 85;
		atom_to_charge[std::string("Rn")] = 86;
		atom_to_charge[std::string("Fr")] = 87;
		atom_to_charge[std::string("Ra")] = 88;
		atom_to_charge[std::string("Ac")] = 89;
		atom_to_charge[std::string("Th")] = 90;
		atom_to_charge[std::string("Pa")] = 91;
		atom_to_charge[std::string("U")] = 92;
		atom_to_charge[std::string("Np")] = 93;
		atom_to_charge[std::string("Pu")] = 94;
		atom_to_charge[std::string("Am")] = 95;
		atom_to_charge[std::string("Cm")] = 96;
		atom_to_charge[std::string("Bk")] = 97;
		atom_to_charge[std::string("Cf")] = 98;
		atom_to_charge[std::string("Es")] = 99;
		atom_to_charge[std::string("Fm")] = 100;
		atom_to_charge[std::string("Md")] = 101;
		atom_to_charge[std::string("No")] = 102;
		atom_to_charge[std::string("Lr")] = 103;
		atom_to_charge[std::string("Rf")] = 104;
		atom_to_charge[std::string("Db")] = 105;
		atom_to_charge[std::string("Sg")] = 106;
		atom_to_charge[std::string("Bh")] = 107;
		atom_to_charge[std::string("Hs")] = 108;
		atom_to_charge[std::string("Mt")] = 109;
		atom_to_charge[std::string("Ds")] = 110;
		atom_to_charge[std::string("Rg")] = 111;
		atom_to_charge[std::string("Cn")] = 112;
		atom_to_charge[std::string("Nh")] = 113;
		atom_to_charge[std::string("Fl")] = 114;
		atom_to_charge[std::string("Mc")] = 115;
		atom_to_charge[std::string("Lv")] = 116;
		atom_to_charge[std::string("Ts")] = 117;
		atom_to_charge[std::string("Og")] = 118;

		//Readin Moleculefile
		std::fstream moleculefile(this->moleculefile);
		std::string moleculefile_line;
		int linenumber = 0;
		while (std::getline(moleculefile, moleculefile_line))
		{
			if (linenumber > 1)
			{
				std::istringstream iss(moleculefile_line);
				double x, y, z;
				std::string atomname;
				if (!(iss >> atomname >> x >> y >> z)) { break; }
				this->electrons += atom_to_charge[atomname];
				Pointcharge readedAtom;
				double A_to_Bohr = 1.889725989;
				readedAtom.set_center(x * A_to_Bohr, y * A_to_Bohr, z * A_to_Bohr);
				readedAtom.set_charge(atom_to_charge[atomname]);
				this->Pointcharges.push_back(readedAtom);
				this->atomnames_to_pointcharges.push_back(atomname);
			}
			linenumber++;
		}
		create_Basisfunctions();
	}

	void Read_Molecule::create_Basisfunctions()
	{
		std::map<std::string, int> basissettype_str_to_int; //NEU!!! Konvertiert den String in einen Integer für die Auswahl,ob sphärisch oder kartesisch.
		basissettype_str_to_int[std::string("spehrical")] = 0;
		basissettype_str_to_int[std::string("cartesian")] = 1;

		//Readin Basissetfile
		std::fstream basissetfile("basissets/" + this->basissetfile);
		std::string basissetfile_line;
		bool is_at_section_start = false;
		std::string atomname;
		std::vector<std::string> parameter_lines;
		std::map<std::string, std::vector<std::string>> parameter_lines_to_atoms;

		std::getline(basissetfile, basissetfile_line); //NEU!!! Hier wird der Basissatztyp in dem Basissatz gespeichert.
		int basissettype = basissettype_str_to_int[std::string(basissetfile_line)];
		this->Basisset.set_basisset_type(basissettype);
		
		while (std::getline(basissetfile, basissetfile_line))
		{
			if (basissetfile_line == std::string("****"))
			{
				if (atomname != "")
				{
					parameter_lines_to_atoms[atomname] = parameter_lines;
				}
				atomname = "";
				parameter_lines.clear();
				is_at_section_start = true;
				continue;
			}
			if (is_at_section_start)
			{
				std::istringstream iss(basissetfile_line);
				int ignore;
				if (!(iss >> atomname >> ignore)) { break; }
				is_at_section_start = false;
				continue;
			}
			if (atomname != std::string(""))
			{
				parameter_lines.push_back(basissetfile_line);
			}
		}

		//Translate Atoms to Basisfunctions
		std::map<char, int> angular_name_to_number;
		angular_name_to_number['S'] = 0;
		angular_name_to_number['P'] = 1;
		angular_name_to_number['D'] = 2;
		angular_name_to_number['F'] = 3;
		angular_name_to_number['G'] = 4;
		angular_name_to_number['H'] = 5;

		for (int atomindex = 0; atomindex < this->atomnames_to_pointcharges.size(); atomindex++)
		{
			std::vector<std::string> parameter_lines = parameter_lines_to_atoms[atomnames_to_pointcharges[atomindex]];
			for (int linenumber = 0; linenumber < parameter_lines.size(); linenumber++)
			{
				std::istringstream iss(parameter_lines[linenumber]);
				int contractionlevel, ignore;
				std::string angular_moments;
				iss >> angular_moments >> contractionlevel >> ignore;

				std::vector<Gaussian_Basisfunction> new_Basisfunctions;
				for (int i = 0; i < angular_moments.length(); i++)
				{
					Gaussian_Basisfunction new_Basisfunction;
					new_Basisfunction.set_total_angular_moment(angular_name_to_number[angular_moments[i]]);
					new_Basisfunction.set_center(this->Pointcharges[atomindex].get_center_x(), this->Pointcharges[atomindex].get_center_y(), this->Pointcharges[atomindex].get_center_z());
					new_Basisfunctions.push_back(new_Basisfunction);
				}

				for (int contraction_index = 0; contraction_index < contractionlevel; contraction_index++)
				{
					linenumber++;
					std::string parameter = parameter_lines[linenumber];
					std::replace(parameter.begin(), parameter.end(), 'D', 'E');

					std::istringstream iss(parameter);
					std::istream_iterator<std::string> beg(iss), end;
					std::vector<std::string> tokens(beg, end);
					for (int i = 0; i < angular_moments.size(); i++)
					{
						new_Basisfunctions[i].add_parameterdouble(std::stof(tokens[i+1]), std::stof(tokens[0]));
					}
				}

				for (int i = 0; i < new_Basisfunctions.size(); i++)
				{
					this->Basisset.add_Basisfunction(new_Basisfunctions[i]);
					//Print out Basisfunctiondata
					/*
					std::cout << new_Basisfunctions[i].get_center_x() << " " << new_Basisfunctions[i].get_center_y() << " " << new_Basisfunctions[i].get_center_z() << " " << new_Basisfunctions[i].get_total_angular_moment() << std::endl;
					for (int j = 0; j < new_Basisfunctions[i].get_contraction(); j++)
					{
						std::cout << new_Basisfunctions[i].get_coefficient(j) << " " << new_Basisfunctions[i].get_exponent(j) << std::endl;
					}
					*/

				}
			}
		}
	}

}