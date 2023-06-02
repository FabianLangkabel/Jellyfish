#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <stdio.h>
#include "basics.h"
#include "QC_advanced_operations.h"
#include "QC_Simulator.h"

namespace QC
{

	class quantum_phase_estimation
	{
	private:
		QC::Pauli_Operator Pauli_Operator;
		QC::QGateSequence state_prep_gate_seq;
		double trotter_step_size = 1;
		int trotter_order = 1;
		int prec_qubits = 5;
		int runs = 1;
		QC::QGateSequence full_circuit;
		std::vector<double> phases;
		std::vector<double> positive_energys;
		std::vector<double> negative_energys;

		QC::QC_Simulator* Simulator;
		QC::QC_advanced_operations advanced_operations;

	public:
		void set_QC_Simulator(QC::QC_Simulator* QC_Simulator) { this->Simulator = QC_Simulator; }
		void set_Pauli_Operator(QC::Pauli_Operator Pauli_Operator) { this->Pauli_Operator = Pauli_Operator; }
		void set_state_preparation_gate_sequence(QC::QGateSequence state_preparation_gate_sequence) { this->state_prep_gate_seq = state_preparation_gate_sequence; }
		void set_trotter_parameter(double trotter_step_size, int trotter_order) { this->trotter_step_size = trotter_step_size; this->trotter_order = trotter_order; }
		void set_precision_qubits(int precision_qubits) { this->prec_qubits = precision_qubits; }
		void set_number_runs(int runs) { this->runs = runs; }
		void create_init_state_as_lowest_flipped(int number_qubits_to_flip);
		void create_full_circuit();
		QC::QGateSequence get_full_circuit() { return this->full_circuit; }
		std::vector<double> get_phases() { return this->phases; }
		std::vector<double> get_positive_energys() { return this->positive_energys; }
		std::vector<double> get_negative_energys() { return this->negative_energys; }
		void compute();
	};
}