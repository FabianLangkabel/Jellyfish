#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <stdio.h>
#include <complex>
#include <cmath>
#include "basics.h"
#include "QC_advanced_operations.h"
#include "QC_QuEST_build_in.h"

namespace QC
{

	class hadamard_test_pauli_sum_quest
	{
	private:
		QC::Pauli_Operator Pauli_Operator;
		QC::QC_Statevector initial_statevector;
		int runs = 1000;
		QC::QGateSequence real_circuit;
		QC::QGateSequence imag_circuit;
		double expectation_value;
		double prefactor = 0.2;
		bool use_only_real_part_for_real_expectation_value = true; //Not used

		QC::QC_QuEST_build_in Simulator;
		QC::QC_advanced_operations advanced_operations;

	public:
		void set_Pauli_Operator(QC::Pauli_Operator Pauli_Operator) { this->Pauli_Operator = Pauli_Operator; }
		void set_initial_statevector(QC::QC_Statevector statevector) { this->initial_statevector = statevector; }
		void set_prefactor(double prefactor) { this->prefactor = prefactor; }
		//void set_trotter_parameter(double trotter_step_size, int trotter_order) { this->trotter_step_size = trotter_step_size; this->trotter_order = trotter_order; }
		void set_number_runs(int runs) { this->runs = runs; }
		void create_circuits();
		double get_expectation_value() { return this->expectation_value; }
		void compute();
	};
}