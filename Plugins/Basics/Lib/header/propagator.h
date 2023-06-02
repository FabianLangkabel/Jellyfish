#pragma once
#include <vector>
#include <iostream>
#include <complex>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iostream>
#include <ctime>
#include <Eigen/Dense>
#include <unsupported/Eigen/MatrixFunctions>
#include "../header/laser.h"
#include "../header/tdci_wf.h"

namespace QC
{
	class Propagator
	{
	private:
		TDCI_WF TDCI_Wavefunction;
		int steps;
		double stepsize;
		std::vector<int> States;
		Eigen::VectorXd Eigen_Energys;
		std::vector<Eigen::MatrixXd> Electronic_Dipole_Matrices;
		Eigen::MatrixXcd One_Electron_Operator_Matrix;
		QC::Laser* Laser;
		bool TruncateStates = false;
		int StateNumberThresh = -1;

		std::vector<double> get_laser_intensity_at_time(double time);
		Eigen::MatrixXcd get_initial_CI_vectors();
		void add_ci_vectors(double time, Eigen::MatrixXcd* CI_vecs);

	public:
		void add_initial_wf(int State) { this->States.push_back(State); }
		void set_steps(int steps) { this->steps = steps; }
		void set_stepsize(double stepsize) { this->stepsize = stepsize; }
		void set_eigen_energys(Eigen::VectorXd Eigen_Energys) { this->Eigen_Energys = Eigen_Energys; }
		void set_electronic_dipole_matrices(std::vector<Eigen::MatrixXd> Electronic_Dipole_Matrices) { this->Electronic_Dipole_Matrices = Electronic_Dipole_Matrices; }
		void set_one_electron_operator(Eigen::MatrixXcd One_Electron_Operator_Matrix) { this->One_Electron_Operator_Matrix = One_Electron_Operator_Matrix; }
		void set_laser(QC::Laser* Laser) { this->Laser = Laser; }
		void set_TruncateStates(bool TruncateStates) { this->TruncateStates = TruncateStates; }
		void set_StateNumberThresh(int StateNumberThresh) { this->StateNumberThresh = StateNumberThresh; }
		
		TDCI_WF get_tdci_wf(){ return TDCI_Wavefunction; }
		
		void compute_propagation();
		void compute_propagation_with_laser();
		void compute_propagation_with_cap();
		void compute_propagation_with_laser_and_cap();
		void compute_propagation_with_laserfile();
		void compute_propagation_with_laserfile_and_cap();
	};
}
