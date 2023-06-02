#include "../header/propagator.h"
#include <chrono>


namespace QC
{

	std::vector<double> Propagator::get_laser_intensity_at_time(double time)
	{
		return this->Laser->get_field_strengths(time);
	}

	Eigen::MatrixXcd Propagator::get_initial_CI_vectors()
	{
		int number_of_states;
		if(TruncateStates)
		{
			number_of_states = StateNumberThresh;
		}
		else
		{
			number_of_states = this->Eigen_Energys.size();
		}

		Eigen::MatrixXcd C_Matrix_Full = Eigen::MatrixXcd::Identity(number_of_states, number_of_states);
		Eigen::MatrixXcd C_Matrix = Eigen::MatrixXcd::Zero(number_of_states, this->States.size());
		if (this->States.empty() == true) {
			C_Matrix = C_Matrix_Full;
		}
		else {
			for (int i = 0; i < this->States.size(); i++) {
				C_Matrix.col(i) = C_Matrix_Full.col(this->States[i]);
			}
		}
		return C_Matrix;
	}

	void Propagator::add_ci_vectors(double time, Eigen::MatrixXcd* CI_vecs)
	{
		this->TDCI_Wavefunction.add_time(time);
		Eigen::MatrixXcd C_Insert = Eigen::MatrixXcd::Zero(this->Eigen_Energys.size(), this->States.size());
		C_Insert.block(0, 0, CI_vecs->rows(),  CI_vecs->cols()) = *CI_vecs;
		this->TDCI_Wavefunction.add_ci_vectors(C_Insert);
	}

	void Propagator::compute_propagation()
	{
		int number_of_states;
		if(TruncateStates)
		{
			number_of_states = StateNumberThresh;
		}
		else
		{
			number_of_states = this->Eigen_Energys.size();
		}

		std::complex<double> I(0, 1);
		Eigen::MatrixXcd H_dia_exp = Eigen::MatrixXcd::Zero(number_of_states, number_of_states);
		for (int i = 0; i < this->Eigen_Energys.rows(); i++) {
			H_dia_exp(i, i) = exp(-I * this->Eigen_Energys(i) * this->stepsize);
		}

		double time = 0;
		Eigen::MatrixXcd C_Matrix = get_initial_CI_vectors();
		for (int i = 0; i < this->steps; i++) {
			add_ci_vectors(time, &C_Matrix);
			C_Matrix = H_dia_exp * C_Matrix;
			time += this->stepsize;
		}

		for (int i = 0; i < this->States.size(); i++) {
			this->TDCI_Wavefunction.add_state(this->States[i]);
		}
	}

	void Propagator::compute_propagation_with_laser()
	{
		int number_of_states;
		if(TruncateStates)
		{
			number_of_states = StateNumberThresh;
		}
		else
		{
			number_of_states = this->Eigen_Energys.size();
		}

		//auto t1 = std::chrono::high_resolution_clock::now();
		std::complex<double> I(0, 1);
		Eigen::MatrixXcd H_dia_exp = Eigen::MatrixXcd::Zero(number_of_states, number_of_states);
		for (int i = 0; i < number_of_states; i++) {
			H_dia_exp(i, i) = exp(-I * this->Eigen_Energys(i) * this->stepsize);
		}

		Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> esx(this->Electronic_Dipole_Matrices[0].block(0,0,number_of_states,number_of_states));
		Eigen::VectorXcd Mux_dia = esx.eigenvalues();
		Eigen::MatrixXcd Ux = esx.eigenvectors();
		Eigen::MatrixXcd UxT = Ux.adjoint();
		Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> esy(this->Electronic_Dipole_Matrices[1].block(0,0,number_of_states,number_of_states));
		Eigen::VectorXcd Muy_dia = esy.eigenvalues();
		Eigen::MatrixXcd Uy = esy.eigenvectors();
		Eigen::MatrixXcd UyT = Uy.adjoint();
		Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> esz(this->Electronic_Dipole_Matrices[2].block(0,0,number_of_states,number_of_states));
		Eigen::VectorXcd Muz_dia = esz.eigenvalues();
		Eigen::MatrixXcd Uz = esz.eigenvectors();
		Eigen::MatrixXcd UzT = Uz.adjoint();

		Eigen::MatrixXcd UxT_Uy = UxT * Uy;
		Eigen::MatrixXcd UyT_Uz = UyT * Uz;
		Eigen::MatrixXcd UzT_H_dia_exp = UzT * H_dia_exp;

		Eigen::MatrixXcd Mux_dia_exp = Eigen::MatrixXcd::Zero(number_of_states, number_of_states);
		Eigen::MatrixXcd Muy_dia_exp = Eigen::MatrixXcd::Zero(number_of_states, number_of_states);
		Eigen::MatrixXcd Muz_dia_exp = Eigen::MatrixXcd::Zero(number_of_states, number_of_states);

		double Laser_Intensity_x = 0;
		double Laser_Intensity_y = 0;
		double Laser_Intensity_z = 0;

		//auto t2 = std::chrono::high_resolution_clock::now();
		//std::cout << "Prep Time: " << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count()<< std::endl;
		double time = 0;

		Eigen::MatrixXcd C_Matrix = get_initial_CI_vectors();
		for (int i = 0; i < steps; i++) {
			std::cout << "Step: " << i << " (" << ((float)i/ (float)steps)*100 << " %)" << std::endl;

			if (i != 0)
			{ 
				Laser_Intensity_x = get_laser_intensity_at_time(time - 0.5 * this->stepsize)[0];
				Laser_Intensity_y = get_laser_intensity_at_time(time - 0.5 * this->stepsize)[1];
				Laser_Intensity_z = get_laser_intensity_at_time(time - 0.5 * this->stepsize)[2];
			}
			
			if (Laser_Intensity_x == 0.0) {
				Mux_dia_exp = Eigen::MatrixXcd::Identity(number_of_states, number_of_states);
			}
			else {
				for (int j = 0; j < number_of_states; j++) {
					Mux_dia_exp(j, j) = exp(I * Mux_dia(j) * Laser_Intensity_x * this->stepsize);
				}
			}

			if (Laser_Intensity_y == 0.0) {
				Muy_dia_exp = Eigen::MatrixXcd::Identity(number_of_states, number_of_states);
			}
			else {
				for (int j = 0; j < number_of_states; j++) {
					Muy_dia_exp(j, j) = exp(I * Muy_dia(j) * Laser_Intensity_y * this->stepsize);
				}
			}

			if (Laser_Intensity_z == 0.0) {
				Muz_dia_exp = Eigen::MatrixXcd::Identity(number_of_states, number_of_states);
			}
			else {
				for (int j = 0; j < number_of_states; j++) {
					Muz_dia_exp(j, j) = exp(I * Muz_dia(j) * Laser_Intensity_z * this->stepsize);
				}
			}
			add_ci_vectors(time, &C_Matrix);
			C_Matrix = Ux * Mux_dia_exp * UxT_Uy * Muy_dia_exp * UyT_Uz * Muz_dia_exp * UzT_H_dia_exp * C_Matrix;
			time += this->stepsize;
		}

		for (int i = 0; i < this->States.size(); i++) {
			this->TDCI_Wavefunction.add_state(this->States[i]);
		}
		//auto t3 = std::chrono::high_resolution_clock::now();
		//std::cout << "Loop Time: " << std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2).count()<< std::endl;
	}

	void Propagator::compute_propagation_with_cap()
	{
		int number_of_states;
		if(TruncateStates)
		{
			number_of_states = StateNumberThresh;
		}
		else
		{
			number_of_states = this->Eigen_Energys.size();
		}

		std::complex<double> I(0, 1);
		Eigen::MatrixXcd H_tid = this->One_Electron_Operator_Matrix;
		for (int i = 0; i < number_of_states; i++) {
			H_tid(i, i) += this->Eigen_Energys(i);
		}
		Eigen::MatrixXcd H_tid_exp = (-I * H_tid * this->stepsize).exp();

		double time = 0;
		Eigen::MatrixXcd C_Matrix = get_initial_CI_vectors();
		for (int i = 0; i < this->steps; i++) {
			add_ci_vectors(time, &C_Matrix);
			C_Matrix = H_tid_exp * C_Matrix;
			time += this->stepsize;
		}

		for (int i = 0; i < this->States.size(); i++) {
			this->TDCI_Wavefunction.add_state(this->States[i]);
		}
	}

	void Propagator::compute_propagation_with_laser_and_cap()
	{
		int number_of_states;
		if(TruncateStates)
		{
			number_of_states = StateNumberThresh;
		}
		else
		{
			number_of_states = this->Eigen_Energys.size();
		}

		//auto t1 = std::chrono::high_resolution_clock::now();
		std::complex<double> I(0, 1);
		Eigen::MatrixXcd H_tid = this->One_Electron_Operator_Matrix.block(0,0,number_of_states,number_of_states);
		for (int i = 0; i < number_of_states; i++) {
			H_tid(i, i) += this->Eigen_Energys(i);
		}
		Eigen::MatrixXcd H_tid_exp = (-I * H_tid * this->stepsize).exp();

		Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> esx(this->Electronic_Dipole_Matrices[0].block(0,0,number_of_states,number_of_states));
		Eigen::VectorXcd Mux_dia = esx.eigenvalues();
		Eigen::MatrixXcd Ux = esx.eigenvectors();
		Eigen::MatrixXcd UxT = Ux.adjoint();
		Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> esy(this->Electronic_Dipole_Matrices[1].block(0,0,number_of_states,number_of_states));
		Eigen::VectorXcd Muy_dia = esy.eigenvalues();
		Eigen::MatrixXcd Uy = esy.eigenvectors();
		Eigen::MatrixXcd UyT = Uy.adjoint();
		Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> esz(this->Electronic_Dipole_Matrices[2].block(0,0,number_of_states,number_of_states));
		Eigen::VectorXcd Muz_dia = esz.eigenvalues();
		Eigen::MatrixXcd Uz = esz.eigenvectors();
		Eigen::MatrixXcd UzT = Uz.adjoint();

		Eigen::MatrixXcd UxT_Uy = UxT * Uy;
		Eigen::MatrixXcd UyT_Uz = UyT * Uz;
		Eigen::MatrixXcd UzT_H_tid_exp = UzT * H_tid_exp;

		Eigen::MatrixXcd Mux_dia_exp = Eigen::MatrixXcd::Zero(number_of_states, number_of_states);
		Eigen::MatrixXcd Muy_dia_exp = Eigen::MatrixXcd::Zero(number_of_states, number_of_states);
		Eigen::MatrixXcd Muz_dia_exp = Eigen::MatrixXcd::Zero(number_of_states, number_of_states);

		double Laser_Intensity_x = 0;
		double Laser_Intensity_y = 0;
		double Laser_Intensity_z = 0;

		//auto t2 = std::chrono::high_resolution_clock::now();
		//std::cout << "Prep Time: " << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count()<< std::endl;

		double time = 0;
		Eigen::MatrixXcd C_Matrix = get_initial_CI_vectors();
		
		for (int i = 0; i < steps; i++) {
			std::cout << "Step: " << i << " (" << ((float)i/ (float)steps)*100 << " %)" << std::endl;

			if (i != 0)
			{
				Laser_Intensity_x = get_laser_intensity_at_time(time - 0.5 * this->stepsize)[0];
				Laser_Intensity_y = get_laser_intensity_at_time(time - 0.5 * this->stepsize)[1];
				Laser_Intensity_z = get_laser_intensity_at_time(time - 0.5 * this->stepsize)[2];
			}

			if (Laser_Intensity_x == 0.0) {
				Mux_dia_exp = Eigen::MatrixXcd::Identity(number_of_states, number_of_states);
			}
			else {
				for (int j = 0; j < number_of_states; j++) {
					Mux_dia_exp(j, j) = exp(I * Mux_dia(j) * Laser_Intensity_x * this->stepsize);
				}
			}

			if (Laser_Intensity_y == 0.0) {
				Muy_dia_exp = Eigen::MatrixXcd::Identity(number_of_states, number_of_states);
			}
			else {
				for (int j = 0; j < number_of_states; j++) {
					Muy_dia_exp(j, j) = exp(I * Muy_dia(j) * Laser_Intensity_y * this->stepsize);
				}
			}

			if (Laser_Intensity_z == 0.0) {
				Muz_dia_exp = Eigen::MatrixXcd::Identity(number_of_states, number_of_states);
			}
			else {
				for (int j = 0; j < number_of_states; j++) {
					Muz_dia_exp(j, j) = exp(I * Muz_dia(j) * Laser_Intensity_z * this->stepsize);
				}
			}

			add_ci_vectors(time, &C_Matrix);
			//C_Matrix = (-I * (Laser_Intensity_z * this->Electronic_Dipole_Matrices[2] + H_tid) * this->stepsize).exp() * C_Matrix;
			C_Matrix = Ux * Mux_dia_exp * UxT_Uy * Muy_dia_exp * UyT_Uz * Muz_dia_exp * UzT_H_tid_exp * C_Matrix;
			time += this->stepsize;
		}

		for (int i = 0; i < this->States.size(); i++) {
			this->TDCI_Wavefunction.add_state(this->States[i]);
		}
		//auto t3 = std::chrono::high_resolution_clock::now();
		//std::cout << "Loop Time: " << std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2).count()<< std::endl;
	}

	void Propagator::compute_propagation_with_laserfile()
	{
		//Read Laser File
		std::string laserfilestring = this->Laser->get_laserfile();
		std::fstream laserfile(laserfilestring);
		std::string laserfile_line;
		std::vector<double> Times;
		std::vector<double> Laser_Intensity_x;
		std::vector<double> Laser_Intensity_y;
		std::vector<double> Laser_Intensity_z;
		int linenumber = 0;
		while (std::getline(laserfile, laserfile_line))
		{
			if (linenumber > 0)
			{
				std::istringstream iss(laserfile_line);
				double t, x, y, z;
				if (!(iss >> t >> x >> y >> z)) { break; }
				Times.push_back(t);
				Laser_Intensity_x.push_back(x);
				Laser_Intensity_y.push_back(y);
				Laser_Intensity_z.push_back(z);
			}
			linenumber++;
		}


		int number_of_states;
		if(TruncateStates)
		{
			number_of_states = StateNumberThresh;
		}
		else
		{
			number_of_states = this->Eigen_Energys.size();
		}

		std::complex<double> I(0, 1);

		Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> esx(this->Electronic_Dipole_Matrices[0].block(0,0,number_of_states,number_of_states));
		Eigen::VectorXcd Mux_dia = esx.eigenvalues();
		Eigen::MatrixXcd Ux = esx.eigenvectors();
		Eigen::MatrixXcd UxT = Ux.adjoint();
		Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> esy(this->Electronic_Dipole_Matrices[1].block(0,0,number_of_states,number_of_states));
		Eigen::VectorXcd Muy_dia = esy.eigenvalues();
		Eigen::MatrixXcd Uy = esy.eigenvectors();
		Eigen::MatrixXcd UyT = Uy.adjoint();
		Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> esz(this->Electronic_Dipole_Matrices[2].block(0,0,number_of_states,number_of_states));
		Eigen::VectorXcd Muz_dia = esz.eigenvalues();
		Eigen::MatrixXcd Uz = esz.eigenvectors();
		Eigen::MatrixXcd UzT = Uz.adjoint();

		Eigen::MatrixXcd UxT_Uy = UxT * Uy;
		Eigen::MatrixXcd UyT_Uz = UyT * Uz;

		Eigen::MatrixXcd H_dia_exp = Eigen::MatrixXcd::Zero(number_of_states, number_of_states);
		Eigen::MatrixXcd Mux_dia_exp = Eigen::MatrixXcd::Zero(number_of_states, number_of_states);
		Eigen::MatrixXcd Muy_dia_exp = Eigen::MatrixXcd::Zero(number_of_states, number_of_states);
		Eigen::MatrixXcd Muz_dia_exp = Eigen::MatrixXcd::Zero(number_of_states, number_of_states);


		Eigen::MatrixXcd C_Matrix = get_initial_CI_vectors();
		for (int i = 0; i < Times.size() - 1; i++) {
			std::cout << "Step: " << i << " (" << ((float)i/ (float)Times.size())*100 << " %)" << std::endl;

			double StepsizeFromFile = Times[i + 1] - Times[i];
			

			for (int i = 0; i < number_of_states; i++) {
				H_dia_exp(i, i) = exp(-I * this->Eigen_Energys(i) * StepsizeFromFile);
			}

			if (Laser_Intensity_x[i] == 0.0) {
				Mux_dia_exp = Eigen::MatrixXcd::Identity(number_of_states, number_of_states);
			}
			else {
				for (int j = 0; j < number_of_states; j++) {
					Mux_dia_exp(j, j) = exp(I * Mux_dia(j) * Laser_Intensity_x[i] * StepsizeFromFile);
				}
			}

			if (Laser_Intensity_y[i] == 0.0) {
				Muy_dia_exp = Eigen::MatrixXcd::Identity(number_of_states, number_of_states);
			}
			else {
				for (int j = 0; j < number_of_states; j++) {
					Muy_dia_exp(j, j) = exp(I * Muy_dia(j) * Laser_Intensity_y[i] * StepsizeFromFile);
				}
			}

			if (Laser_Intensity_z[i] == 0.0) {
				Muz_dia_exp = Eigen::MatrixXcd::Identity(number_of_states, number_of_states);
			}
			else {
				for (int j = 0; j < number_of_states; j++) {
					Muz_dia_exp(j, j) = exp(I * Muy_dia(j) * Laser_Intensity_z[i] * StepsizeFromFile);
				}
			}
			add_ci_vectors(Times[i], &C_Matrix);
			C_Matrix = Ux * Mux_dia_exp * UxT_Uy * Muy_dia_exp * UyT_Uz * Muz_dia_exp * UzT * H_dia_exp * C_Matrix;
		}

		for (int i = 0; i < this->States.size(); i++) {
			this->TDCI_Wavefunction.add_state(this->States[i]);
		}
	}

	void Propagator::compute_propagation_with_laserfile_and_cap()
	{
		//Read Laser File
		std::string laserfilestring = this->Laser->get_laserfile();
		std::fstream laserfile(laserfilestring);
		std::string laserfile_line;
		std::vector<double> Times;
		std::vector<double> Laser_Intensity_x;
		std::vector<double> Laser_Intensity_y;
		std::vector<double> Laser_Intensity_z;
		int linenumber = 0;
		while (std::getline(laserfile, laserfile_line))
		{
			if (linenumber > 0)
			{
				std::istringstream iss(laserfile_line);
				double t, x, y, z;
				if (!(iss >> t >> x >> y >> z)) { break; }
				Times.push_back(t);
				Laser_Intensity_x.push_back(x);
				Laser_Intensity_y.push_back(y);
				Laser_Intensity_z.push_back(z);
			}
			linenumber++;
		}


		int number_of_states;
		if(TruncateStates)
		{
			number_of_states = StateNumberThresh;
		}
		else
		{
			number_of_states = this->Eigen_Energys.size();
		}

		std::complex<double> I(0, 1);
		Eigen::MatrixXcd H_tid = this->One_Electron_Operator_Matrix.block(0,0,number_of_states,number_of_states);
		for (int i = 0; i < number_of_states; i++) {
			H_tid(i, i) += this->Eigen_Energys(i);
		}

		Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> esx(this->Electronic_Dipole_Matrices[0].block(0,0,number_of_states,number_of_states));
		Eigen::VectorXcd Mux_dia = esx.eigenvalues();
		Eigen::MatrixXcd Ux = esx.eigenvectors();
		Eigen::MatrixXcd UxT = Ux.adjoint();
		Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> esy(this->Electronic_Dipole_Matrices[1].block(0,0,number_of_states,number_of_states));
		Eigen::VectorXcd Muy_dia = esy.eigenvalues();
		Eigen::MatrixXcd Uy = esy.eigenvectors();
		Eigen::MatrixXcd UyT = Uy.adjoint();
		Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> esz(this->Electronic_Dipole_Matrices[2].block(0,0,number_of_states,number_of_states));
		Eigen::VectorXcd Muz_dia = esz.eigenvalues();
		Eigen::MatrixXcd Uz = esz.eigenvectors();
		Eigen::MatrixXcd UzT = Uz.adjoint();

		Eigen::MatrixXcd UxT_Uy = UxT * Uy;
		Eigen::MatrixXcd UyT_Uz = UyT * Uz;

		Eigen::MatrixXcd H_dia_exp = Eigen::MatrixXcd::Zero(number_of_states, number_of_states);
		Eigen::MatrixXcd Mux_dia_exp = Eigen::MatrixXcd::Zero(number_of_states, number_of_states);
		Eigen::MatrixXcd Muy_dia_exp = Eigen::MatrixXcd::Zero(number_of_states, number_of_states);
		Eigen::MatrixXcd Muz_dia_exp = Eigen::MatrixXcd::Zero(number_of_states, number_of_states);


		Eigen::MatrixXcd C_Matrix = get_initial_CI_vectors();
		for (int i = 0; i < Times.size() - 1; i++) {
			std::cout << "Step: " << i << " (" << ((float)i/ (float)Times.size())*100 << " %)" << std::endl;

			double StepsizeFromFile = Times[i + 1] - Times[i];
			
			Eigen::MatrixXcd H_tid_exp = (-I * H_tid * this->stepsize).exp();

			if (Laser_Intensity_x[i] == 0.0) {
				Mux_dia_exp = Eigen::MatrixXcd::Identity(number_of_states, number_of_states);
			}
			else {
				for (int j = 0; j < number_of_states; j++) {
					Mux_dia_exp(j, j) = exp(I * Mux_dia(j) * Laser_Intensity_x[i] * StepsizeFromFile);
				}
			}

			if (Laser_Intensity_y[i] == 0.0) {
				Muy_dia_exp = Eigen::MatrixXcd::Identity(number_of_states, number_of_states);
			}
			else {
				for (int j = 0; j < number_of_states; j++) {
					Muy_dia_exp(j, j) = exp(I * Muy_dia(j) * Laser_Intensity_y[i] * StepsizeFromFile);
				}
			}

			if (Laser_Intensity_z[i] == 0.0) {
				Muz_dia_exp = Eigen::MatrixXcd::Identity(number_of_states, number_of_states);
			}
			else {
				for (int j = 0; j < number_of_states; j++) {
					Muz_dia_exp(j, j) = exp(I * Muy_dia(j) * Laser_Intensity_z[i] * StepsizeFromFile);
				}
			}
			add_ci_vectors(Times[i], &C_Matrix);
			C_Matrix = Ux * Mux_dia_exp * UxT_Uy * Muy_dia_exp * UyT_Uz * Muz_dia_exp * UzT * H_dia_exp * C_Matrix;
		}

		for (int i = 0; i < this->States.size(); i++) {
			this->TDCI_Wavefunction.add_state(this->States[i]);
		}
	}
}
