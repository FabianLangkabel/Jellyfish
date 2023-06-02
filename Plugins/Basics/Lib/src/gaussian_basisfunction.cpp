#include "../header/gaussian_basisfunction.h"
#include "../header/integrator_libcint_interface_1e.h"


namespace QC
{
	inline int Gaussian_Basisfunction::fact(int n) {
		return n <= 1 ? 1 : n * fact(n - 1);
	}

	std::vector<int> Gaussian_Basisfunction::angular_moment_expanded_to_cartesian_angular_moments(int angular_moment_expanded)
	{
		int ang_x, ang_y, ang_z;
		switch (angular_moment_expanded) {
		case 0:
			ang_x = 0; ang_y = 0; ang_z = 0;
			break;
		case 1:
			ang_x = 1; ang_y = 0; ang_z = 0;
			break;
		case 2:
			ang_x = 0; ang_y = 1; ang_z = 0;
			break;
		case 3:
			ang_x = 0; ang_y = 0; ang_z = 1;
			break;
		case 4:
			ang_x = 2; ang_y = 0; ang_z = 0;
			break;
		case 5:
			ang_x = 1; ang_y = 1; ang_z = 0;
			break;
		case 6:
			ang_x = 1; ang_y = 0; ang_z = 1;
			break;
		case 7:
			ang_x = 0; ang_y = 2; ang_z = 0;
			break;
		case 8:
			ang_x = 0; ang_y = 1; ang_z = 1;
			break;
		case 9:
			ang_x = 0; ang_y = 0; ang_z = 2;
			break;
		case 10:
			ang_x = 3; ang_y = 0; ang_z = 0;
			break;
		case 11:
			ang_x = 2; ang_y = 1; ang_z = 0;
			break;
		case 12:
			ang_x = 2; ang_y = 0; ang_z = 1;
			break;
		case 13:
			ang_x = 1; ang_y = 2; ang_z = 0;
			break;
		case 14:
			ang_x = 1; ang_y = 1; ang_z = 1;
			break;
		case 15:
			ang_x = 1; ang_y = 0; ang_z = 2;
			break;
		case 16:
			ang_x = 0; ang_y = 3; ang_z = 0;
			break;
		case 17:
			ang_x = 0; ang_y = 2; ang_z = 1;
			break;
		case 18:
			ang_x = 0; ang_y = 1; ang_z = 2;
			break;
		case 19:
			ang_x = 0; ang_y = 0; ang_z = 3;
			break;
		}

		std::vector<int> cartesian_angular_moments{ ang_x, ang_y, ang_z };
		return cartesian_angular_moments;
	}

	double Gaussian_Basisfunction::spherical_harmonic(double x, double y, double z, int angular_moment_expanded)
	{
		switch (angular_moment_expanded) {
		case 0:
			return 1; //s
			break;
		case 1:
			return x; //px
			break;
		case 2:
			return y; //py
			break;
		case 3:
			return z; //pz
			break;
		case 4:
			return x * y; //dxy
			break;
		case 5:
			return y * z; //dyz
			break;
		case 6:
			return sqrt(3) / 6 * (2 * pow(z, 2) - pow(x, 2) - pow(y, 2)); //dz^2
			break;
		case 7:
			return x * z; //dxz
			break;
		case 8:
			return 0.5 * (pow(x, 2) - pow(y, 2)); //dx^2-y^2
			break;
		case 9:
			return sqrt(6) / 12 * y * (3 * pow(x, 2) - pow(y, 2)); // fy(3x^2 - y^2)
			break;
		case 10:
			return x * y * z; // fxyz
			break;
		case 11:
			return sqrt(10) / 20 * y * (4 * pow(z, 2) - pow(x, 2) - pow(y, 2)); // fyz^2
			break;
		case 12:
			return sqrt(15) / 30 * z * (2 * pow(z, 2) - 3 * pow(x, 2) - 3 * pow(y, 2)); // fz^3
			break;
		case 13:
			return sqrt(10) / 20 * x * (4 * pow(z, 2) - pow(x, 2) - pow(y, 2)); // fxz^2
			break;
		case 14:
			return 0.5 * z * (pow(x, 2) - pow(y, 2)); // fz(x^2 - y^2)
			break;
		case 15:
			return sqrt(6) / 12 * x * (pow(x, 2) - 3 * pow(y, 2)); // fx(x^2 - 3y^2)
			break;
		case 16:
			return sqrt(3) / 6 * x * y * (pow(x, 2) - pow(y, 2)); // gxy(x^2-y^2)
			break;
		case 17:
			return sqrt(6) / 12 * (3 * pow(x, 2) - pow(y, 2)) * y * z; // gzy^3
			break;
		case 18:
			return sqrt(21) / 42 * x * y * (6 * pow(z, 2) - pow(x, 2) - pow(y, 2)); // gz^2xy
			break;
		case 19:
			return sqrt(42) / 84 * y * z * (4 * pow(z, 2) - 3 * pow(x, 2) - 3 * pow(y, 2)); // gz^3y
			break;
		case 20:
			return sqrt(105) / 840 * (3 * pow(x, 4) + 3 * pow(y, 4) + 8 * pow(z, 4) + 6 * pow(x * y, 2) - 24 * pow(x * z, 2) - 24 * pow(y * z, 2)); // gz^4
			break;
		case 21:
			return sqrt(42) / 84 * x * z * (4 * pow(z, 2) - 3 * pow(x, 2) - 3 * pow(y, 2)); // gz^3x
			break;
		case 22:
			return sqrt(21) / 84 * (6 * pow(x * z, 2) - pow(x, 4) - 6 * pow(y * z, 2)  + pow(y, 4)); // gz^2(x^2-y^2)
			break;
		case 23:
			return sqrt(6) / 12 * x * z * (pow(x, 2) - 3 * pow(y, 2)); // gzx^3
			break;
		case 24:
			return sqrt(3) / 24 * (pow(x, 4) - 6 * pow(x * y, 2) + pow(y, 4)); // gx^4+y^4
			break;
		case 25:
			return sqrt(30) / 240 * y * (10 * pow(x * y, 2) - 5 * pow(x, 4) - pow(y, 4)); // hy^5
			break;
		case 26:
			return sqrt(3) / 6 * x * y * z * (pow(x, 2) - pow(y, 2)); // hxyz(x^2-y^2)
			break;
		case 27:
			return sqrt(6) / 144 * y * (3*pow(x, 4) - pow(y, 4) + 2 * pow(x * y, 2) - 24 * pow(x * z, 2) + 8 * pow(y * z, 2)); // hyz^2(y^2-x^2)
			break;
		case 28:
			return 1 / 6 * x * y * z * (2 * pow(z, 2) - pow(x, 2) - pow(y, 2)); // hxyz(z^2-x^2-y^2)
			break;
		case 29:
			return sqrt(7) / 168 * y * (12 * pow(x * z, 2) + 12 *  pow(y * z, 2) - 2 * pow(x * y, 2) - pow(x, 4) - pow(y, 4) - 8 * pow(z, 4)); // hyz^4
			break;
		case 30:
			return sqrt(105) / 2520 * z * (15 * pow(x, 4) + 15 * pow(y, 4) + 8 * pow(z, 4) + 30 * pow(x * y, 2) - 40 * pow(x * z, 2) - 40 * pow(y * z, 2)); // hz(x^4+y^4+x^2y^2-x^2z^2-y^2z^2)
			break;
		case 31:
			return sqrt(7) / 168 * x * (12 * pow(x * z, 2) + 12 * pow(y * z, 2) - 2 * pow(x * y, 2) - pow(x, 4) - pow(y, 4) - 8 * pow(z, 4)); // hx(x^2z^2+y^2z^2-x^2y^2-x^4-y^4-z^4)
			break;
		case 32:
			return 1 / 12 * z * (2 * pow(x * z, 2) - 2 * pow(y * z, 2) - pow(x, 4) + pow(y, 4)); // hz(x^2z^2-y^2z^2-x^4+y^4)
			break;
		case 33:
			return sqrt(6) / 144 * x * (24 * pow(y * z, 2) - 8 * pow(x * z, 2) - 2 * pow(x * y, 2) + pow(x, 4) - 3 * pow(y, 4)); // hx(y^2z^2-x^2z^2-x^2y^2+x^4-y^4)
			break;
		case 34:
			return sqrt(3) / 24 * z * (pow(x, 4) - 6 * pow(x * y, 2) + pow(y, 4)); // hz(x^4-xy+y^4)
			break;
		case 35:
			return sqrt(30) / 240 * x * (10 * pow(x * y, 2) - pow(x, 4) - 5 * pow(y, 4)); // hx(xy-x^4-y^4)
			break;
		}
	}

	double Gaussian_Basisfunction::cartesian_harmonic(double x, double y, double z, int angular_moment_expanded)
	{
		int ang_x = angular_moment_expanded_to_cartesian_angular_moments(angular_moment_expanded)[0];
		int ang_y = angular_moment_expanded_to_cartesian_angular_moments(angular_moment_expanded)[1];
		int ang_z = angular_moment_expanded_to_cartesian_angular_moments(angular_moment_expanded)[2];
		return pow(x, ang_x) * pow(y, ang_y) * pow(z, ang_z);
	}

	double Gaussian_Basisfunction::get_spherical_basisfunction_value(double x, double y, double z, int angular_moment)
	{
		if (normed_coefficients.size() == 0) { norm_spherical_basisfunction(); }
		
        double rsquare = pow(x, 2) + pow(y, 2) + pow(z, 2);
		double basisfunction = 0;
		for (int i = 0; i < this->get_contraction(); i++) {
			basisfunction +=  normed_coefficients[i] * spherical_harmonic(x, y, z, angular_moment) * exp(-this->get_exponent(i) * rsquare);
		}
		return basisfunction;
	}

	double Gaussian_Basisfunction::get_cartesian_basisfunction_value(double x, double y, double z, int angular_moment)
	{
		if (normed_coefficients.size() == 0) { norm_cartesian_basisfunction(angular_moment); }

		double rsquare = pow(x, 2) + pow(y, 2) + pow(z, 2);
		double basisfunction = 0;
		for (int i = 0; i < this->get_contraction(); i++) {
			basisfunction += normed_coefficients[i] * cartesian_harmonic(x, y, z, angular_moment) * exp(-this->get_exponent(i) * rsquare);
		}
		return basisfunction;
	}

	void Gaussian_Basisfunction::norm_spherical_basisfunction()
	{
		double pi = 3.1415926535897932384626433832795028841971693993751;

		for (int i = 0; i < this->get_contraction(); i++) {
			this->normed_coefficients.push_back(this->get_coefficient(i) * pow(2 * this->get_exponent(i) / pi, 0.75) * pow(2, this->get_total_angular_moment()) * pow(this->get_exponent(i), 0.5 * this->get_total_angular_moment()));
		}

	}

	void Gaussian_Basisfunction::norm_cartesian_basisfunction(int angular_moment_expanded)
	{
		double pi = 3.1415926535897932384626433832795028841971693993751;

		int ang_x = angular_moment_expanded_to_cartesian_angular_moments(angular_moment_expanded)[0];
		int ang_y = angular_moment_expanded_to_cartesian_angular_moments(angular_moment_expanded)[1];
		int ang_z = angular_moment_expanded_to_cartesian_angular_moments(angular_moment_expanded)[2];
		for (int i = 0; i < this->get_contraction(); i++) {
				this->normed_coefficients.push_back(this->get_coefficient(i) * pow(2 * this->get_exponent(i) / pi, 0.75) * sqrt(pow(8 * this->get_exponent(i), ang_x + ang_y + ang_z) * fact(ang_x) * fact(ang_y) * fact(ang_z) / (fact(2 * ang_x) * fact(2 * ang_y) * fact(2 * ang_z))));
			}
	}

	double Gaussian_Basisfunction::get_basisfunction_value(int basisettype, double x, double y, double z, int angular_moment)
	{
		if (basisettype == 0)
		{
			return get_spherical_basisfunction_value(x,y,z, angular_moment);
		}
		else if (basisettype == 1)
		{
			return get_cartesian_basisfunction_value(x, y, z, angular_moment);
		}
	}

}