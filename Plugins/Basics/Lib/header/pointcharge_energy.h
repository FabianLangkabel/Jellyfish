#pragma once
#include <vector>
#include <math.h>
#include <iostream>
#include "pointcharge.h"

namespace QC
{
	class Pointcharge_Energy
	{
	private:
		std::vector<Pointcharge> Pointcharges;
		double energy;

	public:
		void set_pointcharges(std::vector<Pointcharge> Pointcharges)
		{
			this->Pointcharges = Pointcharges;
		}
		void set_energy(double energy){ this->energy = energy; }
		double get_energy() { return energy; }
		void compute();
	};
}