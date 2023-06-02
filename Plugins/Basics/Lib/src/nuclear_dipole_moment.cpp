#include "../header/nuclear_dipole_moment.h"

namespace QC
{

	void Nuclear_Dipole_Moment::compute()
	{
		Eigen::VectorXd sum(3);
		sum(0) = 0;
		sum(1) = 0;
		sum(2) = 0;

		for (int i = 0; i < this->Pointcharges.size(); i++) {
			sum(0) += this->Pointcharges[i].get_charge() * this->Pointcharges[i].get_center_x();
			sum(1) += this->Pointcharges[i].get_charge() * this->Pointcharges[i].get_center_y();
			sum(2) += this->Pointcharges[i].get_charge() * this->Pointcharges[i].get_center_z();
		}
		this->Nuclear_Dipole_Moment = sum;
	}
	
}