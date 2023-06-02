#include "../header/pointcharge_energy.h"
namespace QC
{
	void Pointcharge_Energy::compute()
	{
		double energy = 0;
		for (int i = 0; i < this->Pointcharges.size(); i++)
		{
			for (int j = 0; j < i; j++)
			{
				double dist = sqrt(
					pow(Pointcharges[i].get_center_x() - Pointcharges[j].get_center_x(), 2) +
					pow(Pointcharges[i].get_center_y() - Pointcharges[j].get_center_y(), 2) +
					pow(Pointcharges[i].get_center_z() - Pointcharges[j].get_center_z(), 2));

				energy += Pointcharges[i].get_charge() * Pointcharges[j].get_charge() * (1 / dist);
			}
		}
		this->energy = energy;
	}
}