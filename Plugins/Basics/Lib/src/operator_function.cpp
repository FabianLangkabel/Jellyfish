#include "../header/operator_function.h"

namespace QC
{

	double CAP1::get_operator_function(double x, double y, double z)
	{
		double radius = pow(pow(x, 2) + pow(y, 2) + pow(z, 2), 0.5);
		double r = radius - this->Radius_abs;
		if (r > 0) {
			return pow(abs(r), this->Order);
		}
		else {
			return 0;
		}
	}

	double CAP2::get_operator_function(double x, double y, double z)
	{
		double radius = pow(pow(x, 2) + pow(y, 2) + pow(z, 2), 0.5);
		double r = radius - this->Radius_abs;
		double curverange = sqrt(this->Potential_Max / (2 * this->Curvate));
		if (r <= 0) {
			return 0;
		}
		else if(0 < r && r <= curverange){
			return this->Curvate * pow(r, 2);
		}
		else if(curverange < r && r < 2 * curverange){
			return -this->Curvate * pow(r - curverange, 2) + this->Potential_Max;
		}
		else {
			return this->Potential_Max;
		}
	}

	double Gaussian_Function::get_operator_function(double x, double y, double z)
	{
		double sum = 0;
		for (int i = 0; i < this->Gaussians.size(); i++) {
			double radiussquared = pow(x - this->Gaussians[i].get_center_x(), 2) + pow(y - this->Gaussians[i].get_center_y(), 2) + pow(z - this->Gaussians[i].get_center_z(), 2);
			sum += -this->Gaussians[i].get_coefficient() * exp(-this->Gaussians[i].get_exponent() * radiussquared);
		}
		return sum;
	}

	double Parser_Function::get_operator_function(double x, double y, double z)
	{
			mu::Parser p;
			p.DefineVar("x", &x);
			p.DefineVar("y", &y);
			p.DefineVar("z", &z);
			//p.DefineFun("MySqr", MySqr);
			p.SetExpr(this->function_string);

			return p.Eval();
	}
}