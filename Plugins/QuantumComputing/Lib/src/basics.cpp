#include "../header/basics.h"
namespace QC
{
	void Laser_Pauli_Operator::calculate_transition_matrices()
	{
		QC::Integrator_libint_interface_1e Integrator;
		QC::Transformation_Integrals_Basisfunction_To_HFOrbitals Transform;

		Integrator.set_basisset(this->Basisset);
		Integrator.set_integraltype(2);
		Integrator.compute();
		Transform.set_one_electron_integrals_basisfunction(Integrator.get_Integrals());
		Transform.set_hf_cmatrix(this->transformation_matrix);
		Transform.compute_one_electron_integrals();
		this->transition_matrix_x = Transform.get_one_electron_integrals_hforbitals();

		Integrator.set_basisset(this->Basisset);
		Integrator.set_integraltype(3);
		Integrator.compute();
		Transform.set_one_electron_integrals_basisfunction(Integrator.get_Integrals());
		Transform.set_hf_cmatrix(this->transformation_matrix);
		Transform.compute_one_electron_integrals();
		this->transition_matrix_y = Transform.get_one_electron_integrals_hforbitals();

		Integrator.set_basisset(this->Basisset);
		Integrator.set_integraltype(4);
		Integrator.compute();
		Transform.set_one_electron_integrals_basisfunction(Integrator.get_Integrals());
		Transform.set_hf_cmatrix(this->transformation_matrix);
		Transform.compute_one_electron_integrals();
		this->transition_matrix_z = Transform.get_one_electron_integrals_hforbitals();

		this->transition_matrices_uptodate = true;
	}
}