#pragma once
#define NOMINMAX
#include <vector>
#include <list>
#include <iostream>
#include <string>
#include <algorithm>
#include <Eigen/Dense>
#include <unsupported/Eigen/CXX11/Tensor>
#include <QuEST.h>

#include "basics.h"

namespace QC
{

	class MO_Integrals_to_JW_Pauli_Operator
	{
	private:
		Eigen::MatrixXd one_electron_integrals;
		Eigen::Tensor<double, 4> two_electron_integrals;
		double e0 = 0;
		double integral_tresh = 0;
		QC::Pauli_Operator Pauli_Operator;

		std::vector<std::tuple<std::vector<int>, double>> fermionic_operator;

		std::vector<int> fermionic_terms_pp_sym;
		std::vector<int> fermionic_terms_pq_sym;
		std::vector<int> fermionic_terms_pqqp_sym;
		std::vector<int> fermionic_terms_pqqr_sym;
		std::vector<int> fermionic_terms_pqrs_sym;

		double JW_Identity;
		std::vector<std::tuple<int, double>> JW_z_term;
		std::vector<std::tuple<std::vector<int>, double>> JW_pq_term;
		std::vector<std::tuple<std::vector<int>, double>> JW_zz_term;
		std::vector<std::tuple<std::vector<int>, double>> JW_pqqr_term;
		std::vector<std::tuple<std::vector<int>, std::vector<double>>> JW_v01234_term;

	public:
		void set_e0(double e0)
		{
			this->e0 = e0;
		}
		void set_one_electron_integrals(Eigen::MatrixXd one_electron_integrals)
		{
			this->one_electron_integrals = one_electron_integrals;
		}
		void set_two_electron_integrals(Eigen::Tensor<double, 4> two_electron_integrals)
		{
			this->two_electron_integrals = two_electron_integrals;
		}
		void set_integral_threshold(double threshold) { this->integral_tresh = threshold; }
		void one_e_integral_to_one_e_spin_Orbitals(int i, int j, double coef);
		void two_e_integral_to_two_e_spin_Orbitals(int i, int j, int k, int l, double coef);
		void add_term_to_fermionic_operator(std::vector<int> indecies, double coef);
		void sort_fermionic_terms_by_sym();
		void print_fermionic_operator();
		std::vector<std::vector<int>> enumerate_one_e_integral_symmetries_and_spin(int i, int j);
		std::vector<std::vector<int>> enumerate_two_e_integral_symmetries_and_spin(int i, int j, int k, int l);
		void jordan_wigner_transformation_for_operator();
		void add_single_pauli_term(int termtype, std::vector<int> indicies, std::vector<double>);
		void print_JW_operator();
		void JW_operator_to_pauli_operator();
		void compute(bool translate_one_electron_integrals, bool translate_two_electron_integrals);
		QC::Pauli_Operator get_Pauli_Operator() { return this->Pauli_Operator; }
	};

	class JW_Laser_Pauli_Operator : public Laser_Pauli_Operator
	{
	public:
		std::vector<QC::Pauli_Operator> get_Pauli_Operators_at_time(double time) override;
	};
}