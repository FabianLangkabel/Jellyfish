#include "../header/hartree_fock_closed_shell_dipole_moment.h"

namespace QC
{
    void Hartree_Fock_Closed_Shell_Dipole_Moment::compute()
    {
        Eigen::VectorXd elsum(3);
        elsum(0) = 0;
        elsum(1) = 0;
        elsum(2) = 0;
        for (int i = 0; i < this->HF_Density_Matrix.rows(); i++) {
            for (int j = 0; j < this->HF_Density_Matrix.rows(); j++) {
                elsum(0) += - this->HF_Density_Matrix(i, j) * this->Overlapmatrix_x(j, i);
                elsum(1) += - this->HF_Density_Matrix(i, j) * this->Overlapmatrix_y(j, i);
                elsum(2) += - this->HF_Density_Matrix(i, j) * this->Overlapmatrix_z(j, i);
            }
        }
        this->Electric_Dipole_Moment = elsum;

        Eigen::VectorXd sum(3);
        sum(0) = elsum(0) + this->Nuclear_Dipole_Moment(0);
        sum(1) = elsum(1) + this->Nuclear_Dipole_Moment(1);
        sum(2) = elsum(2) + this->Nuclear_Dipole_Moment(2);
        this->Dipole_Moment = sum;

        this->Dipole_Moment_Absolute_Value = pow(pow(sum(0), 2) + pow(sum(1), 2) + pow(sum(2), 2), 0.5);
    }
}