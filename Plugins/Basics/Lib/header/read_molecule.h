#pragma once
#include "gaussian_basisset.h"
#include "pointcharge.h"
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iterator>


namespace QC
{
	class Read_Molecule
	{
	private:
		Gaussian_Basisset Basisset;
		std::vector<Pointcharge> Pointcharges;
		std::vector<std::string> atomnames_to_pointcharges;
		std::string moleculefile = "";
		std::string basissetfile = "";
		int charge = 0;
		int electrons = 0;
		void create_Basisfunctions();


	public:
		void set_basissetfile(std::string basissetfile)
		{
			this->basissetfile = basissetfile;
		}
		void set_moleculefile(std::string moleculefile)
		{
			this->moleculefile = moleculefile;
		}
		void set_charge(int charge)
		{
			this->charge = charge;
		}
		void set_basisset(Gaussian_Basisset Basisset) { this->Basisset = Basisset; }
		void set_pointcharges(std::vector<Pointcharge> Pointcharges) { this->Pointcharges = Pointcharges; }
		void set_electrons(int electrons) { this->electrons = electrons; }

		Gaussian_Basisset get_basisset() { return this->Basisset; }
		std::vector<Pointcharge> get_pointcharges() { return this->Pointcharges; }

		int get_electrons()
		{
			return this->electrons - this->charge;
		}
		std::string get_moleculefile()
		{
			return this->moleculefile;
		}
		int get_charge()
		{
			return this->charge;
		}
		std::string get_basissetfile()
		{
			return this->basissetfile;
		}
		std::vector<std::string> get_basissetfiles()
		{
			std::string path = "basissets/";
			std::vector<std::string> filenames;
			for (const auto& entry : std::filesystem::directory_iterator(path))
			{
				//filenames.push_back(entry.path());
				std::string path = entry.path().string();
				size_t i = path.rfind("/", path.length());
				filenames.push_back(path.substr(i + 1, path.length() - i));
			}
			return filenames;
		}
		void compute();
	};
}