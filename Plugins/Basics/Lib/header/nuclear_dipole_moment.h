#pragma once
#include <vector>
#include <iostream>
#include <Eigen/Dense>
#include "../header/pointcharge.h"

namespace QC
{
	class Nuclear_Dipole_Moment
	{
	private:
		std::vector<Pointcharge> Pointcharges;
		Eigen::VectorXd Nuclear_Dipole_Moment;

	public:
		void set_pointcharges(std::vector<Pointcharge> Pointcharges) { this->Pointcharges = Pointcharges; }
		Eigen::VectorXd get_nuclear_dipole_moment() { return Nuclear_Dipole_Moment; }

		void compute();
	};
}