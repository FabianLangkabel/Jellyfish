#pragma once
#include <vector>
#include <tuple>
#include <cmath>
#include "gaussian_basisfunction.h"

namespace QC
{
	class Gaussian_Basisset
	{
	private:
		std::vector<Gaussian_Basisfunction> Basisset;
		int basisset_type = 0; //0 = spherical, 1 = cartesian NEU!!!

	public:
		void clear()
		{
			this->Basisset.clear();
		}

		void add_Basisfunction(Gaussian_Basisfunction Basisfunction)
		{
			this->Basisset.push_back(Basisfunction);
		}
		void set_basisset_type(int basisset_type)
		{
			this->basisset_type = basisset_type; //NEU!!!
		}

		Gaussian_Basisfunction get_Basisfunction(int Basisfunctionindex) { return Basisset[Basisfunctionindex]; }

		int get_Basisfunctionnumber_angular_compact()
		{
			return this->Basisset.size();
		}
		int get_Basisfunctionnumber_angular_expanded() //NEU!!! Die Anzahl der Basisfunktionen lässt sich allgemein über total_angular_moment berechnent, Unterscheidung zwischen sph. und cart. Gaussians getroffen
		{
			int sum = 0;
			switch (this->basisset_type) {
			case 0:
				for (int i = 0; i < this->Basisset.size(); i++)
				{
					int l = this->Basisset[i].get_total_angular_moment();
					sum += 2 * l + 1;
				}
				break;
			case 1:
				for (int i = 0; i < this->Basisset.size(); i++)
				{
					int l = this->Basisset[i].get_total_angular_moment();
					sum += (l + 1) * (l + 2) / 2;
				}
				break;
			}

			return sum;
		}

		std::vector<std::tuple<Gaussian_Basisfunction, int>> get_all_Basisfunction_with_angular_expanded() //NEU!!! Die angular_moment_expanded können so eingefügt werden. Es ist allgemein und man spart sich die Fallunterscheidung
		{
			std::vector<std::tuple<Gaussian_Basisfunction, int>> all_Basisfunction_with_angular_expanded;

			switch (this->basisset_type) {
			case 0:
				for (int i = 0; i < this->Basisset.size(); i++) {
					for (int j = pow(this->Basisset[i].get_total_angular_moment(), 2); j < pow(this->Basisset[i].get_total_angular_moment() + 1, 2); j++) {

						all_Basisfunction_with_angular_expanded.push_back(std::make_tuple(this->Basisset[i], j));
					}
				}
				break;
			case 1:
				for (int i = 0; i < this->Basisset.size(); i++) {
					int l = this->Basisset[i].get_total_angular_moment();
					for (int j = l * (l + 1) * (l + 2) / 6; j < (l + 1) * (l + 2) * (l + 3) / 6; j++) {

						all_Basisfunction_with_angular_expanded.push_back(std::make_tuple(this->Basisset[i], j));
					}
				}
			}
	
			return all_Basisfunction_with_angular_expanded;
		}

		int get_basisset_type()
		{
			return basisset_type; //NEU!!!
		}

		std::vector<QC::Gaussian_Basisfunction> get_all_Basisfunction_with_angular_compact()
		{
			return Basisset;
		}
	};
}