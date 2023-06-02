#pragma once
#include <vector>

namespace QC
{
	class Gaussian_Basisfunction
	{
	private:
		std::vector<double> coefficients;
		std::vector<double> exponents;
		int total_angular_moment;
		std::vector<double> center;
		std::vector<double> normed_coefficients;
		inline int fact(int n);

	public:
		void set_total_angular_moment(int total_angular_moment)
		{	
			this->total_angular_moment = total_angular_moment;
		}
		void set_center(double x, double y, double z)
		{
			this->center = std::vector<double>(3);
			this->center[0] = x;
			this->center[1] = y;
			this->center[2] = z;
		}
		void add_parameterdouble(double coefficient, double exponent)
		{
			this->coefficients.push_back(coefficient);
			this->exponents.push_back(exponent);
		}

		int get_total_angular_moment() { return total_angular_moment; }
		double get_center_x() { return center[0]; }
		double get_center_y() { return center[1]; }
		double get_center_z() { return center[2]; }
		double get_coefficient(int parameterdouble) { return coefficients[parameterdouble]; }
		double get_exponent(int parameterdouble) { return exponents[parameterdouble]; }
		int get_contraction(){ return coefficients.size(); }
		std::vector<double> get_center(){ return center; }

		std::vector<int> angular_moment_expanded_to_cartesian_angular_moments(int angular_moment_expanded);
		void norm_spherical_basisfunction();
		void norm_cartesian_basisfunction(int angular_moment);
		double spherical_harmonic(double x, double y, double z, int angular_moment_expanded);
		double cartesian_harmonic(double x, double y, double z, int angular_moment_expanded);
		double get_spherical_basisfunction_value(double x, double y, double z, int angular_moment_expanded);
		double get_cartesian_basisfunction_value(double x, double y, double z, int angular_moment_expanded);
		
		double get_basisfunction_value(int basisettype, double x, double y, double z, int angular_moment);
	};
}