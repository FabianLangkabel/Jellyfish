#pragma once
#include <vector>

namespace QC
{
	class Potential_Gaussian
	{
	private:
		double coefficient;
		double exponent;
		std::vector<double> center;

	public:
		void set_center(double x, double y, double z)
		{
			this->center = std::vector<double>(3);
			this->center[0] = x;
			this->center[1] = y;
			this->center[2] = z;
		}
		void set_coefficient(double coefficient)
		{
			this->coefficient = coefficient;
		}
		void set_exponent(double exponent)
		{
			this->exponent = exponent;
		}

		double get_center_x() { return center[0]; }
		double get_center_y() { return center[1]; }
		double get_center_z() { return center[2]; }
		double get_coefficient() { return coefficient; }
		double get_exponent() { return exponent; }
		std::vector<double> get_center() { return center; }
	};
}