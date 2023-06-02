#include <QtWidgets>
#include "evaluate_ntos_window.h"


Evaluate_NTOs_Window::Evaluate_NTOs_Window(
	std::shared_ptr <QC::CIResults> ciresults,
	std::shared_ptr <Eigen::MatrixXd> hfcmatrix,
	std::shared_ptr <QC::Gaussian_Basisset> basissetdata,
	std::shared_ptr <QC::TDCI_WF> TDCICoefficients
) : QMainWindow()
{
	this->ciresults = *(ciresults.get());
	this->hfcmatrix = hfcmatrix;
	this->basissetdata = basissetdata;
	this->TDCICoefficients = TDCICoefficients;
	
	QTabWidget* tab_widget = new QTabWidget;
	{
		this->TID_NTOs_Evaluation = new EvaluateTIDWidget();
		tab_widget->addTab(TID_NTOs_Evaluation, "TID-NTOs Evaluation");

		int ci_excited_state_number = this->ciresults.ci_matrix.cols();
		for (int i = 1; i < ci_excited_state_number; i++)
		{
			TID_NTOs_Evaluation->excited_states_list->addItem("State " + QString::number(i));
		}
		TID_NTOs_Evaluation->excited_states_list->setCurrentRow(0);
		connect(TID_NTOs_Evaluation->calculate_button, &QPushButton::clicked, this, &Evaluate_NTOs_Window::update_tid_results);
	}
	{
		this->TD_NTOs_Evaluation = new EvaluateTDWidget();
		tab_widget->addTab(TD_NTOs_Evaluation, "TD-NTOs Evaluation");
		connect(TD_NTOs_Evaluation->calculate_button, &QPushButton::clicked, this, &Evaluate_NTOs_Window::plot_td_results);
		connect(TD_NTOs_Evaluation->save_button, &QPushButton::clicked, this, &Evaluate_NTOs_Window::save_td_results);
	}
	this->setCentralWidget(tab_widget);
}

void Evaluate_NTOs_Window::update_tid_results()
{
	//Input-Paramaters
	int excited_state = this->TID_NTOs_Evaluation->excited_states_list->currentRow();
	Eigen::MatrixXd CI_Matrix = this->ciresults.ci_matrix;
	Eigen::MatrixXd HFCMatrix = *(this->hfcmatrix.get());
	QC::Gaussian_Basisset Basisset = *(this->basissetdata.get());
	std::vector<std::string> confs = this->ciresults.configuration_strings;

	//(intermediate) results
	Eigen::MatrixXd OnePTDM, OnePTDM_adj, OnePTDM_I0, OnePTDM_0I;
	Eigen::MatrixXd NTO_matrix;
	Eigen::VectorXd NTOVector;
	Eigen::MatrixXd S, Mx, My, Mz, Mxx, Mxy, Mxz, Myx, Myy, Myz, Mzx, Mzy, Mzz;


	//********** Start Calculation **********

	//Calculate 1PTDM
	int occupied_spin_orbitals = 0;
	std::string HF_String = confs[0];
	for (int i = 0; i < HF_String.size(); i++)
	{
		if (HF_String[i] == '1') { occupied_spin_orbitals++; }
	}
	int unoccupied_spin_orbitals = HF_String.size() - occupied_spin_orbitals;
	int occupied_spartial_orbitals = occupied_spin_orbitals / 2;
	int unoccupied_spartial_orbitals = unoccupied_spin_orbitals / 2;
	int total_spartial_orbitals = occupied_spartial_orbitals + unoccupied_spartial_orbitals;

	{
		std::map<std::string, int> conf_map;
		for (int i = 0; i < confs.size(); i++)
		{
			std::string conf = confs[i];
			conf_map[conf] = i;
		}

		OnePTDM = Eigen::MatrixXd::Zero(occupied_spartial_orbitals, unoccupied_spartial_orbitals);
		for (int occ = 0; occ < occupied_spartial_orbitals; occ++)
		{
			for (int virt = 0; virt < unoccupied_spartial_orbitals; virt++)
			{
				std::string alphastring = HF_String;
				alphastring[2 * occ] = '0';
				alphastring[2 * (occupied_spartial_orbitals + virt)] = '1';

				std::string betastring = HF_String;
				betastring[2 * occ + 1] = '0';
				betastring[2 * (occupied_spartial_orbitals + virt) + 1] = '1';

				OnePTDM(occ, virt) = (1 / sqrt(2)) * CI_Matrix(conf_map[alphastring], excited_state) - (1 / sqrt(2)) * CI_Matrix(conf_map[betastring], excited_state);
			}
		}
		OnePTDM_adj = OnePTDM.adjoint();


		OnePTDM_I0 = Eigen::MatrixXd::Zero(total_spartial_orbitals, total_spartial_orbitals);
		for (int p = 0; p < total_spartial_orbitals; p++)
		{
			for (int q = 0; q < total_spartial_orbitals; q++)
			{
				if (p >= occupied_spartial_orbitals && q < occupied_spartial_orbitals)
				{
					std::string alphastring = HF_String;
					alphastring[2 * q] = '0';
					alphastring[2 * p] = '1';

					std::string betastring = HF_String;
					betastring[2 * q + 1] = '0';
					betastring[2 * p + 1] = '1';
					OnePTDM_I0(p, q) = (1 / sqrt(2)) * CI_Matrix(conf_map[alphastring], excited_state) - (1 / sqrt(2)) * CI_Matrix(conf_map[betastring], excited_state);
				}

				else if (p < occupied_spartial_orbitals && p == q)
				{
					OnePTDM_I0(p, q) = CI_Matrix(conf_map[HF_String], excited_state);
				}

				else
				{
					OnePTDM_I0(p, q) = 0;
				}
			}
		}
		OnePTDM_0I = OnePTDM_I0.adjoint();

		
		//std::cout << "OnePTDM:" << std::endl << OnePTDM << std::endl << std::endl;
		//std::cout << "OnePTDM_adj:" << std::endl << OnePTDM_adj << std::endl << std::endl;
		//std::cout << "OnePTDM_I0:" << std::endl << OnePTDM_I0 << std::endl << std::endl;
		//std::cout << "OnePTDM_0I:" << std::endl << OnePTDM_0I << std::endl << std::endl;
		
	}

	//Calculate NTOs
	{
		Eigen::JacobiSVD<Eigen::MatrixXd> svd(OnePTDM, Eigen::ComputeFullU | Eigen::ComputeFullV);

		Eigen::MatrixXd NTOs_occ = HFCMatrix.block(0, 0, total_spartial_orbitals, occupied_spartial_orbitals) * svd.matrixU();
		Eigen::MatrixXd NTOs_virt = HFCMatrix.block(0, occupied_spartial_orbitals, total_spartial_orbitals, unoccupied_spartial_orbitals) * svd.matrixV();


		NTO_matrix = Eigen::MatrixXd::Zero(total_spartial_orbitals, total_spartial_orbitals);
		NTO_matrix << NTOs_occ.rowwise().reverse(), NTOs_virt;

		NTOVector = Eigen::VectorXd::Zero(total_spartial_orbitals);
		Eigen::VectorXd svdvec = svd.singularValues();
		for (int i = 0; i < svdvec.rows(); i++)
		{
			NTOVector(occupied_spartial_orbitals - 1 - i) = svdvec(i);
			NTOVector(occupied_spartial_orbitals + i) = svdvec(i);
		}
	}

	//Calculate Multipole-matricies
	{
		QC::Integrator_libint_interface_1e Integrator;
		Integrator.set_basisset(Basisset);
		QC::Transformation_Integrals_Basisfunction_To_HFOrbitals transform;
		transform.set_hf_cmatrix(HFCMatrix);

		Integrator.set_integraltype(0);
		Integrator.compute();
		transform.set_one_electron_integrals_basisfunction(Integrator.get_Integrals());
		transform.compute_one_electron_integrals();
		S = transform.get_one_electron_integrals_hforbitals();

		std::vector<Eigen::MatrixXd> ints;

		Integrator.set_integraltype(100);
		Integrator.compute();
		ints = Integrator.get_multipleIntegrals();
		
		transform.set_one_electron_integrals_basisfunction(ints[0]);
		transform.compute_one_electron_integrals();
		Mx = transform.get_one_electron_integrals_hforbitals();

		transform.set_one_electron_integrals_basisfunction(ints[1]);
		transform.compute_one_electron_integrals();
		My = transform.get_one_electron_integrals_hforbitals();

		transform.set_one_electron_integrals_basisfunction(ints[2]);
		transform.compute_one_electron_integrals();
		Mz = transform.get_one_electron_integrals_hforbitals();

		Integrator.set_integraltype(101);
		Integrator.compute();
		ints = Integrator.get_multipleIntegrals();

		transform.set_one_electron_integrals_basisfunction(ints[0]);
		transform.compute_one_electron_integrals();
		Mxx = transform.get_one_electron_integrals_hforbitals();

		transform.set_one_electron_integrals_basisfunction(ints[1]);
		transform.compute_one_electron_integrals();
		Mxy = transform.get_one_electron_integrals_hforbitals();

		transform.set_one_electron_integrals_basisfunction(ints[2]);
		transform.compute_one_electron_integrals();
		Mxz = transform.get_one_electron_integrals_hforbitals();

		transform.set_one_electron_integrals_basisfunction(ints[3]);
		transform.compute_one_electron_integrals();
		Myx = transform.get_one_electron_integrals_hforbitals();

		transform.set_one_electron_integrals_basisfunction(ints[4]);
		transform.compute_one_electron_integrals();
		Myy = transform.get_one_electron_integrals_hforbitals();

		transform.set_one_electron_integrals_basisfunction(ints[5]);
		transform.compute_one_electron_integrals();
		Myz = transform.get_one_electron_integrals_hforbitals();

		transform.set_one_electron_integrals_basisfunction(ints[6]);
		transform.compute_one_electron_integrals();
		Mzx = transform.get_one_electron_integrals_hforbitals();

		transform.set_one_electron_integrals_basisfunction(ints[7]);
		transform.compute_one_electron_integrals();
		Mzy = transform.get_one_electron_integrals_hforbitals();

		transform.set_one_electron_integrals_basisfunction(ints[8]);
		transform.compute_one_electron_integrals();
		Mzz = transform.get_one_electron_integrals_hforbitals();
	}

	{
		double hole_pos_x, hole_pos_y, hole_pos_z, 
			part_pos_x, part_pos_y, part_pos_z, 
			hole_part_x, hole_part_y, hole_part_z, hole_part_len, 
			hole_size_x, hole_size_y, hole_size_z, hole_size_total,
			particle_size_x, particle_size_y, particle_size_z, particle_size_total,
			exciton_size_x, exciton_size_y, exciton_size_z, exciton_size_total;
		{
			Eigen::MatrixXd temp = OnePTDM_I0 * Mx * OnePTDM_0I * S;
			hole_pos_x = temp.trace();
		}
		{
			Eigen::MatrixXd temp = OnePTDM_I0 * My * OnePTDM_0I * S;
			hole_pos_y = temp.trace();
		}
		{
			Eigen::MatrixXd temp = OnePTDM_I0 * Mz * OnePTDM_0I * S;
			hole_pos_z = temp.trace();
		}
		{
			Eigen::MatrixXd temp = OnePTDM_I0 * S * OnePTDM_0I * Mx;
			part_pos_x = temp.trace();
		}
		{
			Eigen::MatrixXd temp = OnePTDM_I0 * S * OnePTDM_0I * My;
			part_pos_y = temp.trace();
		}
		{
			Eigen::MatrixXd temp = OnePTDM_I0 * S * OnePTDM_0I * Mz;
			part_pos_z = temp.trace();
		}
		{
			Eigen::MatrixXd temp = OnePTDM_I0 * S * OnePTDM_0I * Mz;
		}
		{
			Eigen::MatrixXd temp = OnePTDM_I0 * S * OnePTDM_0I * Mz;
		}
		hole_part_x = part_pos_x - hole_pos_x;
		hole_part_y = part_pos_y - hole_pos_y;
		hole_part_z = part_pos_z - hole_pos_z;
		hole_part_len = sqrt(hole_part_x * hole_part_x + hole_part_y * hole_part_y + hole_part_z * hole_part_z);

		{
			double x2_hole, y2_hole, z2_hole, x2_particle, y2_particle, z2_particle;
			{
				//Hole Size
				double x, y, z;
				Eigen::MatrixXd temp;

				temp = OnePTDM_I0 * Mx * OnePTDM_0I * S;
				x = temp.trace();
				temp = OnePTDM_I0 * My * OnePTDM_0I * S;
				y = temp.trace();
				temp = OnePTDM_I0 * Mz * OnePTDM_0I * S;
				z = temp.trace();

				temp = OnePTDM_I0 * Mxx * OnePTDM_0I * S;
				x2_hole = temp.trace();
				temp = OnePTDM_I0 * Myy * OnePTDM_0I * S;
				y2_hole = temp.trace();
				temp = OnePTDM_I0 * Mzz * OnePTDM_0I * S;
				z2_hole = temp.trace();

				hole_size_x = sqrt(x2_hole - x * x);
				hole_size_y = sqrt(y2_hole - y * y);
				hole_size_z = sqrt(z2_hole - z * z);
				hole_size_total = sqrt((x2_hole - x * x) + (y2_hole - y * y) + (z2_hole - z * z));
			}
			{
				//Particle Size
				double x, y, z;
				Eigen::MatrixXd temp;

				temp = OnePTDM_I0 * S * OnePTDM_0I * Mx;
				x = temp.trace();
				temp = OnePTDM_I0 * S * OnePTDM_0I * My;
				y = temp.trace();
				temp = OnePTDM_I0 * S * OnePTDM_0I * Mz;
				z = temp.trace();

				temp = OnePTDM_I0 * S * OnePTDM_0I * Mxx;
				x2_particle = temp.trace();
				temp = OnePTDM_I0 * S * OnePTDM_0I * Myy;
				y2_particle = temp.trace();
				temp = OnePTDM_I0 * S * OnePTDM_0I * Mzz;
				z2_particle = temp.trace();

				particle_size_x = sqrt(x2_particle - x * x);
				particle_size_y = sqrt(y2_particle - y * y);
				particle_size_z = sqrt(z2_particle - z * z);
				particle_size_total = sqrt((x2_particle - x * x) + (y2_particle - y * y) + (z2_particle - z * z));
			}
			{
				//Exciton Size
				double x_hole_part, y_hole_part, z_hole_part;
				Eigen::MatrixXd temp;

				temp = OnePTDM_I0 * Mx * OnePTDM_0I * Mx;
				x_hole_part = temp.trace();
				temp = OnePTDM_I0 * My * OnePTDM_0I * My;
				y_hole_part = temp.trace();
				temp = OnePTDM_I0 * Mz * OnePTDM_0I * Mz;
				z_hole_part = temp.trace();

				double exciton_size_x_sq = x2_hole + x2_particle - 2 * x_hole_part;
				double exciton_size_y_sq = y2_hole + y2_particle - 2 * y_hole_part;
				double exciton_size_z_sq = z2_hole + z2_particle - 2 * z_hole_part;

				exciton_size_x = sqrt(exciton_size_x_sq);
				exciton_size_y = sqrt(exciton_size_y_sq);
				exciton_size_z = sqrt(exciton_size_z_sq);
				exciton_size_total = sqrt(exciton_size_x_sq + exciton_size_y_sq + exciton_size_z_sq);
			}
		}

		this->TID_NTOs_Evaluation->result_table->setItem(0, 0, new QTableWidgetItem(QString::number(hole_pos_x)));
		this->TID_NTOs_Evaluation->result_table->setItem(0, 1, new QTableWidgetItem(QString::number(hole_pos_y)));
		this->TID_NTOs_Evaluation->result_table->setItem(0, 2, new QTableWidgetItem(QString::number(hole_pos_z)));
		this->TID_NTOs_Evaluation->result_table->setItem(1, 0, new QTableWidgetItem(QString::number(part_pos_x)));
		this->TID_NTOs_Evaluation->result_table->setItem(1, 1, new QTableWidgetItem(QString::number(part_pos_y)));
		this->TID_NTOs_Evaluation->result_table->setItem(1, 2, new QTableWidgetItem(QString::number(part_pos_z)));
		this->TID_NTOs_Evaluation->result_table->setItem(2, 0, new QTableWidgetItem(QString::number(hole_part_x)));
		this->TID_NTOs_Evaluation->result_table->setItem(2, 1, new QTableWidgetItem(QString::number(hole_part_y)));
		this->TID_NTOs_Evaluation->result_table->setItem(2, 2, new QTableWidgetItem(QString::number(hole_part_z)));
		this->TID_NTOs_Evaluation->result_table->setItem(2, 3, new QTableWidgetItem(QString::number(hole_part_len)));
		this->TID_NTOs_Evaluation->result_table->setItem(3, 0, new QTableWidgetItem(QString::number(hole_size_x)));
		this->TID_NTOs_Evaluation->result_table->setItem(3, 1, new QTableWidgetItem(QString::number(hole_size_y)));
		this->TID_NTOs_Evaluation->result_table->setItem(3, 2, new QTableWidgetItem(QString::number(hole_size_z)));
		this->TID_NTOs_Evaluation->result_table->setItem(3, 3, new QTableWidgetItem(QString::number(hole_size_total)));
		this->TID_NTOs_Evaluation->result_table->setItem(4, 0, new QTableWidgetItem(QString::number(particle_size_x)));
		this->TID_NTOs_Evaluation->result_table->setItem(4, 1, new QTableWidgetItem(QString::number(particle_size_y)));
		this->TID_NTOs_Evaluation->result_table->setItem(4, 2, new QTableWidgetItem(QString::number(particle_size_z)));
		this->TID_NTOs_Evaluation->result_table->setItem(4, 3, new QTableWidgetItem(QString::number(particle_size_total)));
		this->TID_NTOs_Evaluation->result_table->setItem(5, 0, new QTableWidgetItem(QString::number(exciton_size_x)));
		this->TID_NTOs_Evaluation->result_table->setItem(5, 1, new QTableWidgetItem(QString::number(exciton_size_y)));
		this->TID_NTOs_Evaluation->result_table->setItem(5, 2, new QTableWidgetItem(QString::number(exciton_size_z)));
		this->TID_NTOs_Evaluation->result_table->setItem(5, 3, new QTableWidgetItem(QString::number(exciton_size_total)));
	}
}

void Evaluate_NTOs_Window::update_td_matrices()
{
	Eigen::MatrixXd HFCMatrix = *(this->hfcmatrix.get());
	QC::Gaussian_Basisset Basisset = *(this->basissetdata.get());

	QC::Integrator_libint_interface_1e Integrator;
	Integrator.set_basisset(Basisset);
	QC::Transformation_Integrals_Basisfunction_To_HFOrbitals transform;
	transform.set_hf_cmatrix(HFCMatrix);

	Integrator.set_integraltype(0);
	Integrator.compute();
	transform.set_one_electron_integrals_basisfunction(Integrator.get_Integrals());
	transform.compute_one_electron_integrals();
	TD_NTOs_Evaluation->S = transform.get_one_electron_integrals_hforbitals();

	std::vector<Eigen::MatrixXd> ints;

	Integrator.set_integraltype(100);
	Integrator.compute();
	ints = Integrator.get_multipleIntegrals();

	transform.set_one_electron_integrals_basisfunction(ints[0]);
	transform.compute_one_electron_integrals();
	TD_NTOs_Evaluation->Mx = transform.get_one_electron_integrals_hforbitals();

	transform.set_one_electron_integrals_basisfunction(ints[1]);
	transform.compute_one_electron_integrals();
	TD_NTOs_Evaluation->My = transform.get_one_electron_integrals_hforbitals();

	transform.set_one_electron_integrals_basisfunction(ints[2]);
	transform.compute_one_electron_integrals();
	TD_NTOs_Evaluation->Mz = transform.get_one_electron_integrals_hforbitals();

	Integrator.set_integraltype(101);
	Integrator.compute();
	ints = Integrator.get_multipleIntegrals();

	transform.set_one_electron_integrals_basisfunction(ints[0]);
	transform.compute_one_electron_integrals();
	TD_NTOs_Evaluation->Mxx = transform.get_one_electron_integrals_hforbitals();

	transform.set_one_electron_integrals_basisfunction(ints[1]);
	transform.compute_one_electron_integrals();
	TD_NTOs_Evaluation->Mxy = transform.get_one_electron_integrals_hforbitals();

	transform.set_one_electron_integrals_basisfunction(ints[2]);
	transform.compute_one_electron_integrals();
	TD_NTOs_Evaluation->Mxz = transform.get_one_electron_integrals_hforbitals();

	transform.set_one_electron_integrals_basisfunction(ints[3]);
	transform.compute_one_electron_integrals();
	TD_NTOs_Evaluation->Myx = transform.get_one_electron_integrals_hforbitals();

	transform.set_one_electron_integrals_basisfunction(ints[4]);
	transform.compute_one_electron_integrals();
	TD_NTOs_Evaluation->Myy = transform.get_one_electron_integrals_hforbitals();

	transform.set_one_electron_integrals_basisfunction(ints[5]);
	transform.compute_one_electron_integrals();
	TD_NTOs_Evaluation->Myz = transform.get_one_electron_integrals_hforbitals();

	transform.set_one_electron_integrals_basisfunction(ints[6]);
	transform.compute_one_electron_integrals();
	TD_NTOs_Evaluation->Mzx = transform.get_one_electron_integrals_hforbitals();

	transform.set_one_electron_integrals_basisfunction(ints[7]);
	transform.compute_one_electron_integrals();
	TD_NTOs_Evaluation->Mzy = transform.get_one_electron_integrals_hforbitals();

	transform.set_one_electron_integrals_basisfunction(ints[8]);
	transform.compute_one_electron_integrals();
	TD_NTOs_Evaluation->Mzz = transform.get_one_electron_integrals_hforbitals();

	TD_NTOs_Evaluation->intermediates_calculated = true;
}

void Evaluate_NTOs_Window::update_td_OnePTDMs()
{
	TD_NTOs_Evaluation->TD_OnePTDMs.clear();


	//Get all needed Data
	Eigen::MatrixXd CI_Matrix = this->ciresults.ci_matrix;
	std::vector<std::string> confs = this->ciresults.configuration_strings;
	std::vector<double> times = this->TDCICoefficients->get_times();
	std::vector<Eigen::MatrixXcd> TDCI_Vectors = this->TDCICoefficients->get_ci_vectors();


	//Prepare Configuration Map for Excitation to CI-Value
	int occupied_spin_orbitals = 0;
	std::string HF_String = confs[0];
	for (int i = 0; i < HF_String.size(); i++)
	{
		if (HF_String[i] == '1') { occupied_spin_orbitals++; }
	}
	int unoccupied_spin_orbitals = HF_String.size() - occupied_spin_orbitals;
	int occupied_spartial_orbitals = occupied_spin_orbitals / 2;
	int unoccupied_spartial_orbitals = unoccupied_spin_orbitals / 2;
	int total_spartial_orbitals = occupied_spartial_orbitals + unoccupied_spartial_orbitals;

	std::map<std::string, int> conf_map;
	for (int i = 0; i < confs.size(); i++)
	{
		std::string conf = confs[i];
		conf_map[conf] = i;
	}

	//Loop over all Time Steps
	for (int timestep = 0; timestep < times.size(); timestep++)
	{
		//Calculate r_hole over time

		//Calculate OnePTDM_I0 and OnePTDM_0I at timestep
		Eigen::MatrixXcd OnePTDM_I0 = Eigen::MatrixXcd::Zero(total_spartial_orbitals, total_spartial_orbitals);

		for (int p = 0; p < total_spartial_orbitals; p++)
		{
			for (int q = 0; q < total_spartial_orbitals; q++)
			{
				if (p >= occupied_spartial_orbitals && q < occupied_spartial_orbitals)
				{
					std::string alphastring = HF_String;
					alphastring[2 * q] = '0';
					alphastring[2 * p] = '1';

					std::string betastring = HF_String;
					betastring[2 * q + 1] = '0';
					betastring[2 * p + 1] = '1';
					for (int statenum = 0; statenum < CI_Matrix.cols(); statenum++)
					{
						double alpha_value = (1 / sqrt(2)) * CI_Matrix(conf_map[alphastring], statenum);
						double beta_value = (1 / sqrt(2)) * CI_Matrix(conf_map[betastring], statenum);
						OnePTDM_I0(p, q) += TDCI_Vectors[timestep](statenum, 0) * (alpha_value - beta_value);
					}
				}
				else if (p < occupied_spartial_orbitals && p == q)
				{
					for (int statenum = 0; statenum < CI_Matrix.cols(); statenum++)
					{
						OnePTDM_I0(p, q) += TDCI_Vectors[timestep](statenum, 0) * CI_Matrix(conf_map[HF_String], statenum);
					}
				}
				else
				{
					OnePTDM_I0(p, q) = 0;
				}
			}
		}

		TD_NTOs_Evaluation->TD_OnePTDMs.push_back(OnePTDM_I0);
	}
}

void Evaluate_NTOs_Window::plot_td_results()
{
	bool MultiplyWithNorm = TD_NTOs_Evaluation->MultiplyWithNorm_box->isChecked();

	if (!TD_NTOs_Evaluation->intermediates_calculated)
	{
		update_td_matrices();
		update_td_OnePTDMs();
	}

	//Get all needed Data
	std::vector<double> times = this->TDCICoefficients->get_times();
	std::vector<Eigen::MatrixXcd> TDCI_Vectors = this->TDCICoefficients->get_ci_vectors();

	//Hole-Position
	if(TD_NTOs_Evaluation->plot_selector->currentRow() == 0)
	{
		TD_NTOs_Evaluation->chart->setTitle("Hole-Position");
		TD_NTOs_Evaluation->chart->removeAllSeries();
		QSplineSeries* series_x = new QSplineSeries();
		QSplineSeries* series_y = new QSplineSeries();
		QSplineSeries* series_z = new QSplineSeries();
		QSplineSeries* series_total = new QSplineSeries();
		series_x->setName("X");
		series_y->setName("Y");
		series_z->setName("Z");
		series_total->setName("Total");

		for (int timestep = 0; timestep < times.size(); timestep++)
		{
			std::complex<double> gs_fac_sq = TDCI_Vectors[timestep](0, 0) * std::conj(TDCI_Vectors[timestep](0, 0));
			double norm = 1 - gs_fac_sq.real();
			if (MultiplyWithNorm)
			{
				Eigen::MatrixXcd OnePTDM_I0 = TD_NTOs_Evaluation->TD_OnePTDMs[timestep];
				Eigen::MatrixXcd OnePTDM_0I = OnePTDM_I0.adjoint();
				double hole_pos_x = 0;
				double hole_pos_y = 0;
				double hole_pos_z = 0;

				if (norm != 0)
				{
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->Mx * OnePTDM_0I * TD_NTOs_Evaluation->S;
						hole_pos_x = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->My * OnePTDM_0I * TD_NTOs_Evaluation->S;
						hole_pos_y = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->Mz * OnePTDM_0I * TD_NTOs_Evaluation->S;
						hole_pos_z = temp.trace().real();
					}
				}

				series_x->append(times[timestep], norm * hole_pos_x);
				series_y->append(times[timestep], norm * hole_pos_y);
				series_z->append(times[timestep], norm * hole_pos_z);
				series_total->append(times[timestep], norm * sqrt(hole_pos_x * hole_pos_x + hole_pos_y * hole_pos_y + hole_pos_z * hole_pos_z));
			}
			else
			{
				if (norm > TD_NTOs_Evaluation->norm_threshold)
				{
					Eigen::MatrixXcd OnePTDM_I0 = TD_NTOs_Evaluation->TD_OnePTDMs[timestep];
					Eigen::MatrixXcd OnePTDM_0I = OnePTDM_I0.adjoint();
					double hole_pos_x, hole_pos_y, hole_pos_z;

					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->Mx * OnePTDM_0I * TD_NTOs_Evaluation->S;
						hole_pos_x = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->My * OnePTDM_0I * TD_NTOs_Evaluation->S;
						hole_pos_y = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->Mz * OnePTDM_0I * TD_NTOs_Evaluation->S;
						hole_pos_z = temp.trace().real();
					}

					series_x->append(times[timestep], hole_pos_x);
					series_y->append(times[timestep], hole_pos_y);
					series_z->append(times[timestep], hole_pos_z);
					series_total->append(times[timestep], sqrt(hole_pos_x * hole_pos_x + hole_pos_y * hole_pos_y + hole_pos_z * hole_pos_z));
					//std::cout << "x: " << hole_pos_x << ", y: " << hole_pos_y << ", z: " << hole_pos_z << std::endl;
				}
			}
		}

		TD_NTOs_Evaluation->chart->addSeries(series_x);
		TD_NTOs_Evaluation->chart->addSeries(series_y);
		TD_NTOs_Evaluation->chart->addSeries(series_z);
		//TD_NTOs_Evaluation->chart->addSeries(series_total);
		TD_NTOs_Evaluation->chart->createDefaultAxes();
		TD_NTOs_Evaluation->chart->legend()->setVisible(true);
	}

	//Particle-Position
	else if(TD_NTOs_Evaluation->plot_selector->currentRow() == 1)
	{
		TD_NTOs_Evaluation->chart->setTitle("Particle-Position");
		TD_NTOs_Evaluation->chart->removeAllSeries();
		QSplineSeries* series_x = new QSplineSeries();
		QSplineSeries* series_y = new QSplineSeries();
		QSplineSeries* series_z = new QSplineSeries();
		QSplineSeries* series_total = new QSplineSeries();
		series_x->setName("X");
		series_y->setName("Y");
		series_z->setName("Z");
		series_total->setName("Total");

		for (int timestep = 0; timestep < times.size(); timestep++)
		{
			std::complex<double> gs_fac_sq = TDCI_Vectors[timestep](0, 0) * std::conj(TDCI_Vectors[timestep](0, 0));
			double norm = 1 - gs_fac_sq.real();
			if (MultiplyWithNorm)
			{
				Eigen::MatrixXcd OnePTDM_I0 = TD_NTOs_Evaluation->TD_OnePTDMs[timestep];
				Eigen::MatrixXcd OnePTDM_0I = OnePTDM_I0.adjoint();
				double particle_pos_x = 0;
				double particle_pos_y = 0;
				double particle_pos_z = 0;

				if (norm != 0)
				{
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->S * OnePTDM_0I * TD_NTOs_Evaluation->Mx;
						particle_pos_x = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->S * OnePTDM_0I * TD_NTOs_Evaluation->My;
						particle_pos_y = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->S * OnePTDM_0I * TD_NTOs_Evaluation->Mz;
						particle_pos_z = temp.trace().real();
					}
				}

				series_x->append(times[timestep], norm * particle_pos_x);
				series_y->append(times[timestep], norm * particle_pos_y);
				series_z->append(times[timestep], norm * particle_pos_z);
				series_total->append(times[timestep], norm * sqrt(particle_pos_x * particle_pos_x + particle_pos_y * particle_pos_y + particle_pos_z * particle_pos_z));
			}
			else
			{
				if (norm > TD_NTOs_Evaluation->norm_threshold)
				{

					Eigen::MatrixXcd OnePTDM_I0 = TD_NTOs_Evaluation->TD_OnePTDMs[timestep];
					Eigen::MatrixXcd OnePTDM_0I = OnePTDM_I0.adjoint();
					double particle_pos_x, particle_pos_y, particle_pos_z;

					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->S * OnePTDM_0I * TD_NTOs_Evaluation->Mx;
						particle_pos_x = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->S * OnePTDM_0I * TD_NTOs_Evaluation->My;
						particle_pos_y = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->S * OnePTDM_0I * TD_NTOs_Evaluation->Mz;
						particle_pos_z = temp.trace().real();
					}

					series_x->append(times[timestep], particle_pos_x);
					series_y->append(times[timestep], particle_pos_y);
					series_z->append(times[timestep], particle_pos_z);
					series_total->append(times[timestep], sqrt(particle_pos_x * particle_pos_x + particle_pos_y * particle_pos_y + particle_pos_z * particle_pos_z));
					//std::cout << "x: " << particle_pos_x << ", y: " << particle_pos_y << ", z: " << particle_pos_z << std::endl;
				}
			}
		}

		TD_NTOs_Evaluation->chart->addSeries(series_x);
		TD_NTOs_Evaluation->chart->addSeries(series_y);
		TD_NTOs_Evaluation->chart->addSeries(series_z);
		//TD_NTOs_Evaluation->chart->addSeries(series_total);
		TD_NTOs_Evaluation->chart->createDefaultAxes();
		TD_NTOs_Evaluation->chart->legend()->setVisible(true);
	}

	//average hole particle distance
	else if (TD_NTOs_Evaluation->plot_selector->currentRow() == 2)
	{
		TD_NTOs_Evaluation->chart->setTitle("average hole particle distance");
		TD_NTOs_Evaluation->chart->removeAllSeries();
		QSplineSeries* series_x = new QSplineSeries();
		QSplineSeries* series_y = new QSplineSeries();
		QSplineSeries* series_z = new QSplineSeries();
		QSplineSeries* series_total = new QSplineSeries();
		series_x->setName("X");
		series_y->setName("Y");
		series_z->setName("Z");
		series_total->setName("Total");

		for (int timestep = 0; timestep < times.size(); timestep++)
		{
			std::complex<double> gs_fac_sq = TDCI_Vectors[timestep](0, 0) * std::conj(TDCI_Vectors[timestep](0, 0));
			double norm = 1 - gs_fac_sq.real();
			if (MultiplyWithNorm)
			{
				Eigen::MatrixXcd OnePTDM_I0 = TD_NTOs_Evaluation->TD_OnePTDMs[timestep];
				Eigen::MatrixXcd OnePTDM_0I = OnePTDM_I0.adjoint();
				double hole_pos_x = 0;
				double hole_pos_y = 0;
				double hole_pos_z = 0;
				double particle_pos_x = 0;
				double particle_pos_y = 0;
				double particle_pos_z = 0;

				if (norm != 0)
				{
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->Mx * OnePTDM_0I * TD_NTOs_Evaluation->S;
						hole_pos_x = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->My * OnePTDM_0I * TD_NTOs_Evaluation->S;
						hole_pos_y = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->Mz * OnePTDM_0I * TD_NTOs_Evaluation->S;
						hole_pos_z = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->S * OnePTDM_0I * TD_NTOs_Evaluation->Mx;
						particle_pos_x = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->S * OnePTDM_0I * TD_NTOs_Evaluation->My;
						particle_pos_y = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->S * OnePTDM_0I * TD_NTOs_Evaluation->Mz;
						particle_pos_z = temp.trace().real();
					}
				}

				double hole_part_x = particle_pos_x - hole_pos_x;
				double hole_part_y = particle_pos_y - hole_pos_y;
				double hole_part_z = particle_pos_z - hole_pos_z;
				double hole_part_len = sqrt(hole_part_x * hole_part_x + hole_part_y * hole_part_y + hole_part_z * hole_part_z);

				series_x->append(times[timestep], norm * hole_part_x);
				series_y->append(times[timestep], norm * hole_part_y);
				series_z->append(times[timestep], norm * hole_part_z);
				series_total->append(times[timestep], norm * hole_part_len);
			}
			else
			{
				if (norm > TD_NTOs_Evaluation->norm_threshold)
				{
					Eigen::MatrixXcd OnePTDM_I0 = TD_NTOs_Evaluation->TD_OnePTDMs[timestep];
					Eigen::MatrixXcd OnePTDM_0I = OnePTDM_I0.adjoint();
					double hole_pos_x, hole_pos_y, hole_pos_z, particle_pos_x, particle_pos_y, particle_pos_z;

					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->Mx * OnePTDM_0I * TD_NTOs_Evaluation->S;
						hole_pos_x = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->My * OnePTDM_0I * TD_NTOs_Evaluation->S;
						hole_pos_y = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->Mz * OnePTDM_0I * TD_NTOs_Evaluation->S;
						hole_pos_z = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->S * OnePTDM_0I * TD_NTOs_Evaluation->Mx;
						particle_pos_x = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->S * OnePTDM_0I * TD_NTOs_Evaluation->My;
						particle_pos_y = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->S * OnePTDM_0I * TD_NTOs_Evaluation->Mz;
						particle_pos_z = temp.trace().real();
					}

					double hole_part_x = particle_pos_x - hole_pos_x;
					double hole_part_y = particle_pos_y - hole_pos_y;
					double hole_part_z = particle_pos_z - hole_pos_z;
					double hole_part_len = sqrt(hole_part_x * hole_part_x + hole_part_y * hole_part_y + hole_part_z * hole_part_z);

					series_x->append(times[timestep], hole_part_x);
					series_y->append(times[timestep], hole_part_y);
					series_z->append(times[timestep], hole_part_z);
					series_total->append(times[timestep], hole_part_len);
				}
			}
		}

		TD_NTOs_Evaluation->chart->addSeries(series_x);
		TD_NTOs_Evaluation->chart->addSeries(series_y);
		TD_NTOs_Evaluation->chart->addSeries(series_z);
		TD_NTOs_Evaluation->chart->addSeries(series_total);
		TD_NTOs_Evaluation->chart->createDefaultAxes();
		TD_NTOs_Evaluation->chart->legend()->setVisible(true);
	}

	//hole size
	else if (TD_NTOs_Evaluation->plot_selector->currentRow() == 3)
	{
		TD_NTOs_Evaluation->chart->setTitle("hole size");
		TD_NTOs_Evaluation->chart->removeAllSeries();
		QSplineSeries* series_x = new QSplineSeries();
		QSplineSeries* series_y = new QSplineSeries();
		QSplineSeries* series_z = new QSplineSeries();
		QSplineSeries* series_total = new QSplineSeries();
		series_x->setName("X");
		series_y->setName("Y");
		series_z->setName("Z");
		series_total->setName("Total");

		for (int timestep = 0; timestep < times.size(); timestep++)
		{
			std::complex<double> gs_fac_sq = TDCI_Vectors[timestep](0, 0) * std::conj(TDCI_Vectors[timestep](0, 0));
			double norm = 1 - gs_fac_sq.real();
			if (MultiplyWithNorm)
			{
				Eigen::MatrixXcd OnePTDM_I0 = TD_NTOs_Evaluation->TD_OnePTDMs[timestep];
				Eigen::MatrixXcd OnePTDM_0I = OnePTDM_I0.adjoint();

				double hole_pos_x = 0;
				double hole_pos_y = 0;
				double hole_pos_z = 0;
				double hole_x2 = 0;
				double hole_y2 = 0;
				double hole_z2 = 0;

				if (norm != 0)
				{
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->Mx * OnePTDM_0I * TD_NTOs_Evaluation->S;
						hole_pos_x = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->My * OnePTDM_0I * TD_NTOs_Evaluation->S;
						hole_pos_y = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->Mz * OnePTDM_0I * TD_NTOs_Evaluation->S;
						hole_pos_z = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->Mxx * OnePTDM_0I * TD_NTOs_Evaluation->S;
						hole_x2 = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->Myy * OnePTDM_0I * TD_NTOs_Evaluation->S;
						hole_y2 = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->Mzz * OnePTDM_0I * TD_NTOs_Evaluation->S;
						hole_z2 = temp.trace().real();
					}
				}

				double hole_size_x = norm * sqrt(hole_x2 - hole_pos_x * hole_pos_x);
				double hole_size_y = norm * sqrt(hole_y2 - hole_pos_y * hole_pos_y);
				double hole_size_z = norm * sqrt(hole_z2 - hole_pos_z * hole_pos_z);
				double hole_size_total = norm * sqrt((hole_x2 - hole_pos_x * hole_pos_x) + (hole_y2 - hole_pos_y * hole_pos_y) + (hole_z2 - hole_pos_z * hole_pos_z));

				series_x->append(times[timestep], hole_size_x);
				series_y->append(times[timestep], hole_size_y);
				series_z->append(times[timestep], hole_size_z);
				series_total->append(times[timestep], hole_size_total);
			}
			else
			{
				if (norm > TD_NTOs_Evaluation->norm_threshold)
				{
					Eigen::MatrixXcd OnePTDM_I0 = TD_NTOs_Evaluation->TD_OnePTDMs[timestep];
					Eigen::MatrixXcd OnePTDM_0I = OnePTDM_I0.adjoint();

					double hole_pos_x, hole_pos_y, hole_pos_z, hole_x2, hole_y2, hole_z2;
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->Mx * OnePTDM_0I * TD_NTOs_Evaluation->S;
						hole_pos_x = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->My * OnePTDM_0I * TD_NTOs_Evaluation->S;
						hole_pos_y = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->Mz * OnePTDM_0I * TD_NTOs_Evaluation->S;
						hole_pos_z = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->Mxx * OnePTDM_0I * TD_NTOs_Evaluation->S;
						hole_x2 = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->Myy * OnePTDM_0I * TD_NTOs_Evaluation->S;
						hole_y2 = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->Mzz * OnePTDM_0I * TD_NTOs_Evaluation->S;
						hole_z2 = temp.trace().real();
					}

					double hole_size_x = sqrt(hole_x2 - hole_pos_x * hole_pos_x);
					double hole_size_y = sqrt(hole_y2 - hole_pos_y * hole_pos_y);
					double hole_size_z = sqrt(hole_z2 - hole_pos_z * hole_pos_z);
					double hole_size_total = sqrt((hole_x2 - hole_pos_x * hole_pos_x) + (hole_y2 - hole_pos_y * hole_pos_y) + (hole_z2 - hole_pos_z * hole_pos_z));

					series_x->append(times[timestep], hole_size_x);
					series_y->append(times[timestep], hole_size_y);
					series_z->append(times[timestep], hole_size_z);
					series_total->append(times[timestep], hole_size_total);
				}
			}
		}

		TD_NTOs_Evaluation->chart->addSeries(series_x);
		TD_NTOs_Evaluation->chart->addSeries(series_y);
		TD_NTOs_Evaluation->chart->addSeries(series_z);
		TD_NTOs_Evaluation->chart->addSeries(series_total);
		TD_NTOs_Evaluation->chart->createDefaultAxes();
		TD_NTOs_Evaluation->chart->legend()->setVisible(true);
	}

	//particle size
	else if (TD_NTOs_Evaluation->plot_selector->currentRow() == 4)
	{
		TD_NTOs_Evaluation->chart->setTitle("particle size");
		TD_NTOs_Evaluation->chart->removeAllSeries();
		QSplineSeries* series_x = new QSplineSeries();
		QSplineSeries* series_y = new QSplineSeries();
		QSplineSeries* series_z = new QSplineSeries();
		QSplineSeries* series_total = new QSplineSeries();
		series_x->setName("X");
		series_y->setName("Y");
		series_z->setName("Z");
		series_total->setName("Total");

		for (int timestep = 0; timestep < times.size(); timestep++)
		{
			std::complex<double> gs_fac_sq = TDCI_Vectors[timestep](0, 0) * std::conj(TDCI_Vectors[timestep](0, 0));
			double norm = 1 - gs_fac_sq.real();
			if (MultiplyWithNorm)
			{
				Eigen::MatrixXcd OnePTDM_I0 = TD_NTOs_Evaluation->TD_OnePTDMs[timestep];
				Eigen::MatrixXcd OnePTDM_0I = OnePTDM_I0.adjoint();

				double particle_pos_x = 0;
				double particle_pos_y = 0;
				double particle_pos_z = 0;
				double particle_x2 = 0;
				double particle_y2 = 0;
				double particle_z2 = 0;
				if (norm != 0)
				{
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->S * OnePTDM_0I * TD_NTOs_Evaluation->Mx;
						particle_pos_x = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->S * OnePTDM_0I * TD_NTOs_Evaluation->My;
						particle_pos_y = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->S * OnePTDM_0I * TD_NTOs_Evaluation->Mz;
						particle_pos_z = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->S * OnePTDM_0I * TD_NTOs_Evaluation->Mxx;
						particle_x2 = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->S * OnePTDM_0I * TD_NTOs_Evaluation->Myy;
						particle_y2 = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->S * OnePTDM_0I * TD_NTOs_Evaluation->Mzz;
						particle_z2 = temp.trace().real();
					}
				}

				double particle_size_x = norm * sqrt(particle_x2 - particle_pos_x * particle_pos_x);
				double particle_size_y = norm * sqrt(particle_y2 - particle_pos_y * particle_pos_y);
				double particle_size_z = norm * sqrt(particle_z2 - particle_pos_z * particle_pos_z);
				double particle_size_total = norm * sqrt((particle_x2 - particle_pos_x * particle_pos_x) + (particle_y2 - particle_pos_y * particle_pos_y) + (particle_z2 - particle_pos_z * particle_pos_z));

				series_x->append(times[timestep], particle_size_x);
				series_y->append(times[timestep], particle_size_y);
				series_z->append(times[timestep], particle_size_z);
				series_total->append(times[timestep], particle_size_total);
			}
			else
			{
				if (norm > TD_NTOs_Evaluation->norm_threshold)
				{
					Eigen::MatrixXcd OnePTDM_I0 = TD_NTOs_Evaluation->TD_OnePTDMs[timestep];
					Eigen::MatrixXcd OnePTDM_0I = OnePTDM_I0.adjoint();

					double particle_pos_x, particle_pos_y, particle_pos_z, particle_x2, particle_y2, particle_z2;
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->S * OnePTDM_0I * TD_NTOs_Evaluation->Mx;
						particle_pos_x = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->S * OnePTDM_0I * TD_NTOs_Evaluation->My;
						particle_pos_y = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->S * OnePTDM_0I * TD_NTOs_Evaluation->Mz;
						particle_pos_z = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->S * OnePTDM_0I * TD_NTOs_Evaluation->Mxx;
						particle_x2 = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->S * OnePTDM_0I * TD_NTOs_Evaluation->Myy;
						particle_y2 = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->S * OnePTDM_0I * TD_NTOs_Evaluation->Mzz;
						particle_z2 = temp.trace().real();
					}

					double particle_size_x = sqrt(particle_x2 - particle_pos_x * particle_pos_x);
					double particle_size_y = sqrt(particle_y2 - particle_pos_y * particle_pos_y);
					double particle_size_z = sqrt(particle_z2 - particle_pos_z * particle_pos_z);
					double particle_size_total = sqrt((particle_x2 - particle_pos_x * particle_pos_x) + (particle_y2 - particle_pos_y * particle_pos_y) + (particle_z2 - particle_pos_z * particle_pos_z));

					series_x->append(times[timestep], particle_size_x);
					series_y->append(times[timestep], particle_size_y);
					series_z->append(times[timestep], particle_size_z);
					series_total->append(times[timestep], particle_size_total);
				}
			}
		}

		TD_NTOs_Evaluation->chart->addSeries(series_x);
		TD_NTOs_Evaluation->chart->addSeries(series_y);
		TD_NTOs_Evaluation->chart->addSeries(series_z);
		TD_NTOs_Evaluation->chart->addSeries(series_total);
		TD_NTOs_Evaluation->chart->createDefaultAxes();
		TD_NTOs_Evaluation->chart->legend()->setVisible(true);
	}

	//exciton size
	else if (TD_NTOs_Evaluation->plot_selector->currentRow() == 5)
	{
		TD_NTOs_Evaluation->chart->setTitle("exciton size");
		TD_NTOs_Evaluation->chart->removeAllSeries();
		QSplineSeries* series_x = new QSplineSeries();
		QSplineSeries* series_y = new QSplineSeries();
		QSplineSeries* series_z = new QSplineSeries();
		QSplineSeries* series_total = new QSplineSeries();
		series_x->setName("X");
		series_y->setName("Y");
		series_z->setName("Z");
		series_total->setName("Total");

		for (int timestep = 0; timestep < times.size(); timestep++)
		{
			std::complex<double> gs_fac_sq = TDCI_Vectors[timestep](0, 0) * std::conj(TDCI_Vectors[timestep](0, 0));
			double norm = 1 - gs_fac_sq.real();
			if (MultiplyWithNorm)
			{
				Eigen::MatrixXcd OnePTDM_I0 = TD_NTOs_Evaluation->TD_OnePTDMs[timestep];
				Eigen::MatrixXcd OnePTDM_0I = OnePTDM_I0.adjoint();

				double hole_x2 = 0;
				double hole_y2 = 0;
				double hole_z2 = 0;
				double particle_x2 = 0;
				double particle_y2 = 0;
				double particle_z2 = 0;
				double x_hole_part = 0;
				double y_hole_part = 0;
				double z_hole_part = 0;

				if (norm != 0)
				{
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->Mxx * OnePTDM_0I * TD_NTOs_Evaluation->S;
						hole_x2 = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->Myy * OnePTDM_0I * TD_NTOs_Evaluation->S;
						hole_y2 = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->Mzz * OnePTDM_0I * TD_NTOs_Evaluation->S;
						hole_z2 = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->S * OnePTDM_0I * TD_NTOs_Evaluation->Mxx;
						particle_x2 = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->S * OnePTDM_0I * TD_NTOs_Evaluation->Myy;
						particle_y2 = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->S * OnePTDM_0I * TD_NTOs_Evaluation->Mzz;
						particle_z2 = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->Mx * OnePTDM_0I * TD_NTOs_Evaluation->Mx;
						x_hole_part = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->My * OnePTDM_0I * TD_NTOs_Evaluation->My;
						y_hole_part = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->Mz * OnePTDM_0I * TD_NTOs_Evaluation->Mz;
						z_hole_part = temp.trace().real();
					}
				}

				double exciton_size_x_sq = hole_x2 + particle_x2 - 2 * x_hole_part;
				double exciton_size_y_sq = hole_y2 + particle_y2 - 2 * y_hole_part;
				double exciton_size_z_sq = hole_z2 + particle_z2 - 2 * z_hole_part;

				double exciton_size_x = norm * sqrt(exciton_size_x_sq);
				double exciton_size_y = norm * sqrt(exciton_size_y_sq);
				double exciton_size_z = norm * sqrt(exciton_size_z_sq);
				double exciton_size_total = norm * sqrt(exciton_size_x_sq + exciton_size_y_sq + exciton_size_z_sq);

				series_x->append(times[timestep], exciton_size_x);
				series_y->append(times[timestep], exciton_size_y);
				series_z->append(times[timestep], exciton_size_z);
				series_total->append(times[timestep], exciton_size_total);
			}
			else
			{
				if (norm > TD_NTOs_Evaluation->norm_threshold)
				{
					Eigen::MatrixXcd OnePTDM_I0 = TD_NTOs_Evaluation->TD_OnePTDMs[timestep];
					Eigen::MatrixXcd OnePTDM_0I = OnePTDM_I0.adjoint();

					double  hole_x2, hole_y2, hole_z2, particle_x2, particle_y2, particle_z2, x_hole_part, y_hole_part, z_hole_part;
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->Mxx * OnePTDM_0I * TD_NTOs_Evaluation->S;
						hole_x2 = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->Myy * OnePTDM_0I * TD_NTOs_Evaluation->S;
						hole_y2 = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->Mzz * OnePTDM_0I * TD_NTOs_Evaluation->S;
						hole_z2 = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->S * OnePTDM_0I * TD_NTOs_Evaluation->Mxx;
						particle_x2 = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->S * OnePTDM_0I * TD_NTOs_Evaluation->Myy;
						particle_y2 = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->S * OnePTDM_0I * TD_NTOs_Evaluation->Mzz;
						particle_z2 = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->Mx * OnePTDM_0I * TD_NTOs_Evaluation->Mx;
						x_hole_part = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->My * OnePTDM_0I * TD_NTOs_Evaluation->My;
						y_hole_part = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->Mz * OnePTDM_0I * TD_NTOs_Evaluation->Mz;
						z_hole_part = temp.trace().real();
					}

					double exciton_size_x_sq = hole_x2 + particle_x2 - 2 * x_hole_part;
					double exciton_size_y_sq = hole_y2 + particle_y2 - 2 * y_hole_part;
					double exciton_size_z_sq = hole_z2 + particle_z2 - 2 * z_hole_part;

					double exciton_size_x = sqrt(exciton_size_x_sq);
					double exciton_size_y = sqrt(exciton_size_y_sq);
					double exciton_size_z = sqrt(exciton_size_z_sq);
					double exciton_size_total = sqrt(exciton_size_x_sq + exciton_size_y_sq + exciton_size_z_sq);

					series_x->append(times[timestep], exciton_size_x);
					series_y->append(times[timestep], exciton_size_y);
					series_z->append(times[timestep], exciton_size_z);
					series_total->append(times[timestep], exciton_size_total);
				}
			}
		}
		TD_NTOs_Evaluation->chart->addSeries(series_x);
		TD_NTOs_Evaluation->chart->addSeries(series_y);
		TD_NTOs_Evaluation->chart->addSeries(series_z);
		TD_NTOs_Evaluation->chart->addSeries(series_total);
		TD_NTOs_Evaluation->chart->createDefaultAxes();
		TD_NTOs_Evaluation->chart->legend()->setVisible(true);
	}
}

void Evaluate_NTOs_Window::save_td_results()
{
	bool MultiplyWithNorm = TD_NTOs_Evaluation->MultiplyWithNorm_box->isChecked();

	QString FileName = QFileDialog::getSaveFileName(nullptr, tr("Save Data File"), QDir::homePath(), tr("txt File (*.txt)"));

	if (!FileName.isEmpty())
	{

		if (!TD_NTOs_Evaluation->intermediates_calculated)
		{
			update_td_matrices();
			update_td_OnePTDMs();
		}

		std::vector<double> times = this->TDCICoefficients->get_times();
		std::vector<Eigen::MatrixXcd> TDCI_Vectors = this->TDCICoefficients->get_ci_vectors();
		Eigen::MatrixXd all_data = Eigen::MatrixXd::Zero(times.size(), 23);

		for (int timestep = 0; timestep < times.size(); timestep++)
		{
			std::complex<double> gs_fac_sq = TDCI_Vectors[timestep](0, 0) * std::conj(TDCI_Vectors[timestep](0, 0));
			double norm = 1 - gs_fac_sq.real();
			if (MultiplyWithNorm)
			{
				all_data(timestep, 0) = times[timestep];

				Eigen::MatrixXcd OnePTDM_I0 = TD_NTOs_Evaluation->TD_OnePTDMs[timestep];
				Eigen::MatrixXcd OnePTDM_0I = OnePTDM_I0.adjoint();

				double hole_pos_x = 0; double hole_pos_y = 0; double hole_pos_z = 0;
				double particle_pos_x = 0; double particle_pos_y = 0; double particle_pos_z = 0;
				double hole_x2 = 0; double hole_y2 = 0; double hole_z2 = 0;
				double particle_x2 = 0; double particle_y2 = 0; double particle_z2 = 0;
				double x_hole_part = 0; double y_hole_part = 0; double z_hole_part = 0;

				if (norm != 0)
				{
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->Mx * OnePTDM_0I * TD_NTOs_Evaluation->S;
						hole_pos_x = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->My * OnePTDM_0I * TD_NTOs_Evaluation->S;
						hole_pos_y = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->Mz * OnePTDM_0I * TD_NTOs_Evaluation->S;
						hole_pos_z = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->S * OnePTDM_0I * TD_NTOs_Evaluation->Mx;
						particle_pos_x = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->S * OnePTDM_0I * TD_NTOs_Evaluation->My;
						particle_pos_y = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->S * OnePTDM_0I * TD_NTOs_Evaluation->Mz;
						particle_pos_z = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->Mxx * OnePTDM_0I * TD_NTOs_Evaluation->S;
						hole_x2 = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->Myy * OnePTDM_0I * TD_NTOs_Evaluation->S;
						hole_y2 = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->Mzz * OnePTDM_0I * TD_NTOs_Evaluation->S;
						hole_z2 = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->S * OnePTDM_0I * TD_NTOs_Evaluation->Mxx;
						particle_x2 = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->S * OnePTDM_0I * TD_NTOs_Evaluation->Myy;
						particle_y2 = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->S * OnePTDM_0I * TD_NTOs_Evaluation->Mzz;
						particle_z2 = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->Mx * OnePTDM_0I * TD_NTOs_Evaluation->Mx;
						x_hole_part = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->My * OnePTDM_0I * TD_NTOs_Evaluation->My;
						y_hole_part = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->Mz * OnePTDM_0I * TD_NTOs_Evaluation->Mz;
						z_hole_part = temp.trace().real();
					}
				}

				{
					all_data(timestep, 1) = norm * hole_pos_x;
					all_data(timestep, 2) = norm * hole_pos_y;
					all_data(timestep, 3) = norm * hole_pos_z;
				}

				{
					all_data(timestep, 4) = norm * particle_pos_x;
					all_data(timestep, 5) = norm * particle_pos_y;
					all_data(timestep, 6) = norm * particle_pos_z;
				}

				{
					double hole_part_x = particle_pos_x - hole_pos_x;
					double hole_part_y = particle_pos_y - hole_pos_y;
					double hole_part_z = particle_pos_z - hole_pos_z;
					double hole_part_len = sqrt(hole_part_x * hole_part_x + hole_part_y * hole_part_y + hole_part_z * hole_part_z);
					all_data(timestep, 7) = norm * hole_part_x;
					all_data(timestep, 8) = norm * hole_part_y;
					all_data(timestep, 9) = norm * hole_part_z;
					all_data(timestep, 10) = norm * hole_part_len;
				}

				{
					double hole_size_x = sqrt(hole_x2 - hole_pos_x * hole_pos_x);
					double hole_size_y = sqrt(hole_y2 - hole_pos_y * hole_pos_y);
					double hole_size_z = sqrt(hole_z2 - hole_pos_z * hole_pos_z);
					double hole_size_total = sqrt((hole_x2 - hole_pos_x * hole_pos_x) + (hole_y2 - hole_pos_y * hole_pos_y) + (hole_z2 - hole_pos_z * hole_pos_z));
					all_data(timestep, 11) = norm * hole_size_x;
					all_data(timestep, 12) = norm * hole_size_y;
					all_data(timestep, 13) = norm * hole_size_z;
					all_data(timestep, 14) = norm * hole_size_total;
				}

				{
					double particle_size_x = sqrt(particle_x2 - particle_pos_x * particle_pos_x);
					double particle_size_y = sqrt(particle_y2 - particle_pos_y * particle_pos_y);
					double particle_size_z = sqrt(particle_z2 - particle_pos_z * particle_pos_z);
					double particle_size_total = sqrt((particle_x2 - particle_pos_x * particle_pos_x) + (particle_y2 - particle_pos_y * particle_pos_y) + (particle_z2 - particle_pos_z * particle_pos_z));
					all_data(timestep, 15) = norm * particle_size_x;
					all_data(timestep, 16) = norm * particle_size_y;
					all_data(timestep, 17) = norm * particle_size_z;
					all_data(timestep, 18) = norm * particle_size_total;
				}

				{
					double exciton_size_x_sq = hole_x2 + particle_x2 - 2 * x_hole_part;
					double exciton_size_y_sq = hole_y2 + particle_y2 - 2 * y_hole_part;
					double exciton_size_z_sq = hole_z2 + particle_z2 - 2 * z_hole_part;
					double exciton_size_x = sqrt(exciton_size_x_sq);
					double exciton_size_y = sqrt(exciton_size_y_sq);
					double exciton_size_z = sqrt(exciton_size_z_sq);
					double exciton_size_total = sqrt(exciton_size_x_sq + exciton_size_y_sq + exciton_size_z_sq);
					all_data(timestep, 19) = norm * exciton_size_x;
					all_data(timestep, 20) = norm * exciton_size_y;
					all_data(timestep, 21) = norm * exciton_size_z;
					all_data(timestep, 22) = norm * exciton_size_total;
				}
			}
			else
			{
				if (norm > TD_NTOs_Evaluation->norm_threshold)
				{
					all_data(timestep, 0) = times[timestep];

					Eigen::MatrixXcd OnePTDM_I0 = TD_NTOs_Evaluation->TD_OnePTDMs[timestep];
					Eigen::MatrixXcd OnePTDM_0I = OnePTDM_I0.adjoint();

					double hole_pos_x, hole_pos_y, hole_pos_z,
						particle_pos_x, particle_pos_y, particle_pos_z,
						hole_x2, hole_y2, hole_z2,
						particle_x2, particle_y2, particle_z2,
						x_hole_part, y_hole_part, z_hole_part;

					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->Mx * OnePTDM_0I * TD_NTOs_Evaluation->S;
						hole_pos_x = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->My * OnePTDM_0I * TD_NTOs_Evaluation->S;
						hole_pos_y = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->Mz * OnePTDM_0I * TD_NTOs_Evaluation->S;
						hole_pos_z = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->S * OnePTDM_0I * TD_NTOs_Evaluation->Mx;
						particle_pos_x = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->S * OnePTDM_0I * TD_NTOs_Evaluation->My;
						particle_pos_y = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->S * OnePTDM_0I * TD_NTOs_Evaluation->Mz;
						particle_pos_z = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->Mxx * OnePTDM_0I * TD_NTOs_Evaluation->S;
						hole_x2 = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->Myy * OnePTDM_0I * TD_NTOs_Evaluation->S;
						hole_y2 = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->Mzz * OnePTDM_0I * TD_NTOs_Evaluation->S;
						hole_z2 = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->S * OnePTDM_0I * TD_NTOs_Evaluation->Mxx;
						particle_x2 = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->S * OnePTDM_0I * TD_NTOs_Evaluation->Myy;
						particle_y2 = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->S * OnePTDM_0I * TD_NTOs_Evaluation->Mzz;
						particle_z2 = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->Mx * OnePTDM_0I * TD_NTOs_Evaluation->Mx;
						x_hole_part = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->My * OnePTDM_0I * TD_NTOs_Evaluation->My;
						y_hole_part = temp.trace().real();
					}
					{
						Eigen::MatrixXcd temp = (1 / norm) * OnePTDM_I0 * TD_NTOs_Evaluation->Mz * OnePTDM_0I * TD_NTOs_Evaluation->Mz;
						z_hole_part = temp.trace().real();
					}

					{
						all_data(timestep, 1) = hole_pos_x;
						all_data(timestep, 2) = hole_pos_y;
						all_data(timestep, 3) = hole_pos_z;
					}

					{
						all_data(timestep, 4) = particle_pos_x;
						all_data(timestep, 5) = particle_pos_y;
						all_data(timestep, 6) = particle_pos_z;
					}

					{
						double hole_part_x = particle_pos_x - hole_pos_x;
						double hole_part_y = particle_pos_y - hole_pos_y;
						double hole_part_z = particle_pos_z - hole_pos_z;
						double hole_part_len = sqrt(hole_part_x * hole_part_x + hole_part_y * hole_part_y + hole_part_z * hole_part_z);
						all_data(timestep, 7) = hole_part_x;
						all_data(timestep, 8) = hole_part_y;
						all_data(timestep, 9) = hole_part_z;
						all_data(timestep, 10) = hole_part_len;
					}

					{
						double hole_size_x = sqrt(hole_x2 - hole_pos_x * hole_pos_x);
						double hole_size_y = sqrt(hole_y2 - hole_pos_y * hole_pos_y);
						double hole_size_z = sqrt(hole_z2 - hole_pos_z * hole_pos_z);
						double hole_size_total = sqrt((hole_x2 - hole_pos_x * hole_pos_x) + (hole_y2 - hole_pos_y * hole_pos_y) + (hole_z2 - hole_pos_z * hole_pos_z));
						all_data(timestep, 11) = hole_size_x;
						all_data(timestep, 12) = hole_size_y;
						all_data(timestep, 13) = hole_size_z;
						all_data(timestep, 14) = hole_size_total;
					}

					{
						double particle_size_x = sqrt(particle_x2 - particle_pos_x * particle_pos_x);
						double particle_size_y = sqrt(particle_y2 - particle_pos_y * particle_pos_y);
						double particle_size_z = sqrt(particle_z2 - particle_pos_z * particle_pos_z);
						double particle_size_total = sqrt((particle_x2 - particle_pos_x * particle_pos_x) + (particle_y2 - particle_pos_y * particle_pos_y) + (particle_z2 - particle_pos_z * particle_pos_z));
						all_data(timestep, 15) = particle_size_x;
						all_data(timestep, 16) = particle_size_y;
						all_data(timestep, 17) = particle_size_z;
						all_data(timestep, 18) = particle_size_total;
					}

					{
						double exciton_size_x_sq = hole_x2 + particle_x2 - 2 * x_hole_part;
						double exciton_size_y_sq = hole_y2 + particle_y2 - 2 * y_hole_part;
						double exciton_size_z_sq = hole_z2 + particle_z2 - 2 * z_hole_part;
						double exciton_size_x = sqrt(exciton_size_x_sq);
						double exciton_size_y = sqrt(exciton_size_y_sq);
						double exciton_size_z = sqrt(exciton_size_z_sq);
						double exciton_size_total = sqrt(exciton_size_x_sq + exciton_size_y_sq + exciton_size_z_sq);
						all_data(timestep, 19) = exciton_size_x;
						all_data(timestep, 20) = exciton_size_y;
						all_data(timestep, 21) = exciton_size_z;
						all_data(timestep, 22) = exciton_size_total;
					}
				}
			}
		}
		std::ofstream outfile;
		outfile.open(FileName.toStdString());
		outfile << "time ";
		outfile << "hole_pos_x hole_pos_y hole_pos_z ";
		outfile << "particle_pos_x particle_pos_y particle_pos_z ";
		outfile << "hole_particle_distance_x hole_particle_distance_y hole_particle_distance_z hole_particle_distance_total ";
		outfile << "hole_size_x hole_size_y hole_size_z hole_size_total ";
		outfile << "particle_size_x particle_size_y particle_size_z particle_size_total ";
		outfile << "exciton_size_x exciton_size_y exciton_size_z exciton_size_total";
		outfile << std::endl;
		outfile << all_data;
		outfile.close();
	}
}