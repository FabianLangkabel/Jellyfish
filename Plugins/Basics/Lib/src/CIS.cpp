#include "../header/CIS.h"
namespace QC
{
	void CIS::compute()
	{
		int number_basisfunction = this->HF_Orbitalenergys.rows() * 2;
		Eigen::VectorXd F_eigvals = Eigen::VectorXd::Zero(number_basisfunction);
		for (int i = 0; i < F_eigvals.rows() / 2; i++)
		{
			F_eigvals(2 * i) = this->HF_Orbitalenergys(i, i);
			F_eigvals(2 * i + 1) = this->HF_Orbitalenergys(i, i);
		}


		int dim = electrons * (number_basisfunction - electrons);

		Eigen::MatrixXd A = Eigen::MatrixXd::Zero(dim, dim);

		int row = 0;
		for (int i = 0; i < electrons; i++)
		{
			for (int a = electrons; a < number_basisfunction; a++)
			{
				int col = 0;
				for (int j = 0; j < electrons; j++)
				{
					for (int b = electrons; b < number_basisfunction; b++)
					{
						A(row, col) = (F_eigvals(a) - F_eigvals(i)) * (i == j) * (a == b) + this->two_electron_integrals_spinorbitals(a, j, i, b);
						col++;
					}
				}
				row++;
			}
		}
		//std::cout << A << std::endl;


		Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> es(A);
		this->ci_matrix = es.eigenvectors();
		this->ci_values = es.eigenvalues();
	}
}