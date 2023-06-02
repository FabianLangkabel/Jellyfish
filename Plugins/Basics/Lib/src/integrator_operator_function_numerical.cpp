#include "../header/integrator_operator_function_numerical.h"
// get number of cores/ logistical threads
const int poolSize = std::thread::hardware_concurrency();
const double pi = 3.1415926535897932384626433832795;
const double e = 2.7182818284590452353602874713527;


namespace QC
{

	double Integrator_Operator_Function_Numerical::operator_function(double x, double y, double z) {
		return this->Operator_Function->get_operator_function(x, y, z);
	}

	void Integrator_Operator_Function_Numerical::compute()
	{
		this->basisset_size = this->Basisset.get_Basisfunctionnumber_angular_expanded();
		this->Integrals = Eigen::MatrixXd::Zero(this->basisset_size, this->basisset_size);
		this->stepsize = 2 * this->range / this->steps;

		switch (this->Basisset.get_basisset_type()) {
		case 0:
		{
			std::thread* ts = new std::thread[poolSize]; //generate computation-thread for each logistical thread/core of the hardware
			//Start each thread
			for (int k = 0; k < poolSize; k++)
			{
				ts[k] = std::thread([this](int k) {Integrator_Operator_Function_Numerical::computeCase0(k); }, k);
			}
			//wait for threads to finish
			for (int k = 0; k < poolSize; k++) {
				ts[k].join();
			}
			delete[] ts;
			break;
		}
		case 1:
		{
			std::thread* ts = new std::thread[poolSize];
			for (int k = 0; k < poolSize; k++)
			{
				ts[k] = std::thread([this](int k) {Integrator_Operator_Function_Numerical::computeCase1(k); }, k);
			}
			for (int k = 0; k < poolSize; k++) {
				ts[k].join();
			}
			delete[] ts;
			break;
		}
		}
	}

	void Integrator_Operator_Function_Numerical::computeCase0(int id)
	{

		for (int i = id; i < this->basisset_size; i += poolSize) {
			for (int j = i; j < this->basisset_size; j++) {

				QC::Gaussian_Basisfunction basisfunction1;
				QC::Gaussian_Basisfunction basisfunction2;
				int angular_moment1;
				int angular_moment2;
				std::tie(basisfunction1, angular_moment1) = this->Basisset.get_all_Basisfunction_with_angular_expanded()[i];
				std::tie(basisfunction2, angular_moment2) = this->Basisset.get_all_Basisfunction_with_angular_expanded()[j];
				double x1 = basisfunction1.get_center_x();
				double y1 = basisfunction1.get_center_y();
				double z1 = basisfunction1.get_center_z();
				double x2 = basisfunction2.get_center_x();
				double y2 = basisfunction2.get_center_y();
				double z2 = basisfunction2.get_center_z();

				double overlap = 0;
				double z = -this->range;
				for (int a = 0; a < this->steps; a++) {
					double y = -this->range;
					for (int b = 0; b < this->steps; b++) {
						double x = -this->range;
						for (int c = 0; c < this->steps; c++) {
							double operator_value = operator_function(x, y, z);
							if (operator_value != 0) {
								overlap += basisfunction1.get_spherical_basisfunction_value(x - x1, y - y1, z - z1, angular_moment1) *
									operator_value *
									basisfunction2.get_spherical_basisfunction_value(x - x2, y - y2, z - z2, angular_moment2);
							}
							x += stepsize;
						}
						y += stepsize;
					}
					z += stepsize;
				}
				overlap = overlap * pow(stepsize, 3);
				this->Integrals(i, j) = overlap;
				this->Integrals(j, i) = overlap;
			}
		}
	}
	void Integrator_Operator_Function_Numerical::computeCase1(int id)
	{
		for (int i = id; i < basisset_size; i += poolSize) {
			for (int j = 0; j < this->basisset_size; j++) {

				QC::Gaussian_Basisfunction basisfunction1;
				QC::Gaussian_Basisfunction basisfunction2;
				int angular_moment1;
				int angular_moment2;
				std::tie(basisfunction1, angular_moment1) = this->Basisset.get_all_Basisfunction_with_angular_expanded()[i];
				std::tie(basisfunction2, angular_moment2) = this->Basisset.get_all_Basisfunction_with_angular_expanded()[j];
				double x1 = basisfunction1.get_center_x();
				double y1 = basisfunction1.get_center_y();
				double z1 = basisfunction1.get_center_z();
				double x2 = basisfunction2.get_center_x();
				double y2 = basisfunction2.get_center_y();
				double z2 = basisfunction2.get_center_z();

				double overlap = 0;
				double z = -this->range;
				for (int a = 0; a < this->steps; a++) {
					double y = -this->range;
					for (int b = 0; b < this->steps; b++) {
						double x = -this->range;
						for (int c = 0; c < this->steps; c++) {
							double operator_value = operator_function(x, y, z);
							if (operator_value != 0) {
								overlap += basisfunction1.get_cartesian_basisfunction_value(x - x1, y - y1, z - z1, angular_moment1) *
									operator_value *
									basisfunction2.get_cartesian_basisfunction_value(x - x2, y - y2, z - z2, angular_moment2);
							}
							x += stepsize;
						}
						y += stepsize;
					}
					z += stepsize;
				}

				this->Integrals(i, j) = overlap * pow(stepsize, 3);
			}
		}
	}
	/*
	Eigen::MatrixXd get_semianalyiticalIntegrals() 
	{ 
		//for each orbital combination
		//get overlap

		return Integrals; 
	}
		
	double getsemianalyticalOverlap(double exponent, double R)
	{
		//for each radial expansion
		//get radial overlap (wigthed errorfct)
		
		//get angular overlap

		return overlap;
	}
	
	// Error function with max error of 1.5e-7
	double getErrorfct(double R)
	{
		const double p = 0.3275911;
		const double a1 = 0.254829592;
		const double a2 = -0.284496736;
		const double a3 = 1.421413741;
		const double a4 = -1.453152027:
		const double a5 = 1.061405429;
		double t = 1/(1+p*x);
		double P = t*(a1+t*(a2+t*(a3+t*(a4+t*a5))));
		return 1-P*(pow(e,-x*x));
	}
	*/
}
	