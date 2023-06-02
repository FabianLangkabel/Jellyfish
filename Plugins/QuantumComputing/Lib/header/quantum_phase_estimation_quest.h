#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <stdio.h>
#include "basics.h"
#include "QC_advanced_operations.h"
#include "QC_QuEST_build_in.h"

namespace QC
{

	class quantum_phase_estimation_quest
	{
	private:
		QC::Pauli_Operator Pauli_Operator;
		QC::QC_Statevector initial_statevector;
		int initial_state_type = 0;
		int number_qubits_to_flipp_initial;
		double trotter_step_size = 1;
		int trotter_order = 1;
		int prec_qubits = 5;
		int runs = 1;
		QC::QGateSequence qpe_circuit;
		std::vector<double> phases;
		std::vector<double> positive_energys;
		std::vector<double> negative_energys;

		QC::QC_QuEST_build_in Simulator;
		QC::QC_advanced_operations advanced_operations;

	public:
		void set_initial_state_type(int initial_state_type) { this->initial_state_type = initial_state_type; }
		void set_Pauli_Operator(QC::Pauli_Operator Pauli_Operator) { this->Pauli_Operator = Pauli_Operator; }
		void set_initial_statevector(QC::QC_Statevector statevector) { this->initial_statevector = statevector; }
		void set_trotter_parameter(double trotter_step_size, int trotter_order) { this->trotter_step_size = trotter_step_size; this->trotter_order = trotter_order; }
		void set_precision_qubits(int precision_qubits) { this->prec_qubits = precision_qubits; }
		void set_number_runs(int runs) { this->runs = runs; }
		void set_number_qubits_to_flipp_initial(int number_qubits_to_flipp_initial) { this->number_qubits_to_flipp_initial = number_qubits_to_flipp_initial; }
		void create_qpe_circuit();
		std::vector<double> get_phases() { return this->phases; }
		std::vector<double> get_positive_energys() { return this->positive_energys; }
		std::vector<double> get_negative_energys() { return this->negative_energys; }
		void compute();

	private:
		void create_init_state_as_lowest_flipped(int number_qubits_to_flip);
	};
}