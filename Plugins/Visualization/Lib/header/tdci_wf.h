#pragma once
#include <iostream>
#include <string>
#include <Eigen/Dense>
#include <unsupported/Eigen/CXX11/Tensor>


namespace QC
{
	class TDCI_WF
	{
		friend class Propagation;
	private:
		std::vector<double> Times;
		std::vector<int> States;
		std::vector<Eigen::MatrixXcd> CI_Vectors_Time_Dependent;

	public:
		void clear()
		{
			this->Times.clear();
			this->States.clear();
			this->CI_Vectors_Time_Dependent.clear();
		}
		void add_state(int state) { this->States.push_back(state); }
		void add_time(double time) { this->Times.push_back(time); }
		void add_ci_vectors(Eigen::MatrixXcd ci_vectors) { this->CI_Vectors_Time_Dependent.push_back(ci_vectors); }

		std::vector<double> get_times(){ return Times; }
		std::vector<Eigen::MatrixXcd> get_ci_vectors() { return CI_Vectors_Time_Dependent; }
		std::vector<int> get_states() { return States; }

		std::vector<Eigen::VectorXcd> get_ci_vectors_for_state(int State)
		{
			std::vector<Eigen::VectorXcd> CI_Vectors_For_State;
			for (int i = 0; i < CI_Vectors_Time_Dependent.size(); i++) {
				CI_Vectors_For_State.push_back(CI_Vectors_Time_Dependent[i].col(State));
			}
			return CI_Vectors_For_State;
		}
		std::vector<std::complex<double>> get_tdci_coefficient_for_state(int Coeff, int State) {
			std::vector<std::complex<double>> TDCI_Coefficients_For_State;
			for (int i = 0; i < CI_Vectors_Time_Dependent.size(); i++) {
				std::complex<double> coeff = CI_Vectors_Time_Dependent[i](Coeff, State);
				TDCI_Coefficients_For_State.push_back(coeff);
			}
			return TDCI_Coefficients_For_State;
		}
		std::vector<Eigen::VectorXd> get_populations_for_state(int State) {
			std::vector<Eigen::VectorXd> Populations_For_State;

			for (int i = 0; i < CI_Vectors_Time_Dependent.size(); i++) {
				Eigen::VectorXcd CI_Vector = CI_Vectors_Time_Dependent[i].col(State);
				Eigen::VectorXd CI_Vector_abs = (CI_Vector.conjugate() * CI_Vector).real();
				Populations_For_State.push_back(CI_Vector_abs);
			}
			return Populations_For_State;
		}
		std::vector<double> get_population_for_state(int Coeff, int State) {
			std::vector<double> Populations_For_State;
			for (int i = 0; i < CI_Vectors_Time_Dependent.size(); i++) {
				std::complex<double> coeff = CI_Vectors_Time_Dependent[i](Coeff, State);
				double coeffabssquare = pow(coeff.real(), 2) + pow(coeff.imag(), 2);
				Populations_For_State.push_back(coeffabssquare);
			}
			return Populations_For_State;
		}

	};

}