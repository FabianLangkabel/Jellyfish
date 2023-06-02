#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <stdio.h>
#include <complex>
#include <math.h>
#include "basics.h"
#include "QC_advanced_operations.h"
#include "QC_Simulator.h"

namespace QC
{
	class expection_values_data
	{
	public:
		std::vector<QC::Pauli_String> pauli_strings;
		std::map<int, double> expection_values_map;

		void add_expection_value(QC::Pauli_String pauli_string, double expection_value)
		{
			pauli_strings.push_back(pauli_string);
			expection_values_map[pauli_strings_to_int(pauli_string)] = expection_value;
		}
		double get_expection_value(QC::Pauli_String pauli_string)
		{
			return expection_values_map[pauli_strings_to_int(pauli_string)];
		}
		int pauli_strings_to_int(QC::Pauli_String pauli_string)
		{
			int value = 0;
			std::vector<std::string> pauli_operations = pauli_string.get_PauliOperations();
			for (int i = 0; i < pauli_operations.size(); i++)
			{

				if (pauli_operations[i] == "I") { value += pow(4, pauli_operations.size() - i - 1) * 0; }
				else if (pauli_operations[i] == "X") { value += pow(4, pauli_operations.size() - i - 1) * 1; }
				else if (pauli_operations[i] == "Y") { value += pow(4, pauli_operations.size() - i - 1) * 2; }
				else if (pauli_operations[i] == "Z") { value += pow(4, pauli_operations.size() - i - 1) * 3; }
			}

			return value;

		}
	};

	class quantum_propagation_laser_cap
	{
	private:
		QC::Pauli_Operator TimeIndependent_Pauli_Operator;
		QC::Laser_Pauli_Operator* Laser_Pauli_Operator;
		QC::Pauli_Operator CAP_Operator;
		bool use_CAP_Operator;
		bool use_Laser_Operator;
		bool get_qite_expecation_values_without_measurement;
		int measurments_per_qite_expection_value;

		QC::QGateSequence state_prep_gate_seq;
		double trotter_step_size = 1;
		int trotter_order = 1;
		int timesteps;
		QC::QC_Simulator* Simulator;

		bool dump_statevectors = false;
		std::vector<int> amplitudes_to_dump;
		QC::QC_TD_Statevectors dumped_statevectors;
		QC::QC_TD_Norm TD_Norm;

		QC::QGateSequence full_circuit;
		int qbits_for_circuit;
		expection_values_data expection_values;


	public:
		void set_TimeIndependent_Pauli_Operator(QC::Pauli_Operator TimeIndependent_Pauli_Operator) { this->TimeIndependent_Pauli_Operator = TimeIndependent_Pauli_Operator; }
		void set_Laser_Pauli_Operator(QC::Laser_Pauli_Operator* Laser_Pauli_Operator) { this->Laser_Pauli_Operator = Laser_Pauli_Operator; }
		void set_CAP_Operator(QC::Pauli_Operator CAP_Operator) { this->CAP_Operator = CAP_Operator; }
		void set_state_preparation_gate_sequence(QC::QGateSequence state_preparation_gate_sequence) { this->state_prep_gate_seq = state_preparation_gate_sequence; }
		void set_trotter_parameter(double trotter_step_size, int trotter_order) { this->trotter_step_size = trotter_step_size; this->trotter_order = trotter_order; }
		void set_number_timesteps(int timesteps) { this->timesteps = timesteps; }
		void set_QC_Simulator(QC::QC_Simulator* QC_Simulator) { this->Simulator = QC_Simulator; }
		void set_use_CAP_Operator(bool use_CAP_Operator) { this->use_CAP_Operator = use_CAP_Operator; }
		void set_use_Laser_Operator(bool use_Laser_Operator) { this->use_Laser_Operator = use_Laser_Operator; }
		void set_get_qite_expecation_values_without_measurement(bool get_qite_expecation_values_without_measurement) { this->get_qite_expecation_values_without_measurement = get_qite_expecation_values_without_measurement; }
		void set_measurments_per_qite_expection_value(int measurments_per_qite_expection_value) { this->measurments_per_qite_expection_value = measurments_per_qite_expection_value; }

		void set_dump_statevector(bool dump_statevectors) { this->dump_statevectors = dump_statevectors; }
		void set_amplitudes_to_dump(std::vector<int> amplitudes_to_dump) { this->amplitudes_to_dump = amplitudes_to_dump; }
		QC::QC_TD_Statevectors get_dumped_statevectors() { return dumped_statevectors; }

		void create_init_state_as_lowest_flipped(int number_qubits_to_flip);

		QC::QC_TD_Norm get_TD_Norm() { return this->TD_Norm; }
		void compute();

	private:
		void add_laser_and_tid_operator_to_circuit(double time);
		long long int perform_circuit(QC::QGateSequence circuit);
		void perform_circuit_and_dump_statevectors(QC::QGateSequence circuit);
		void add_dumping_of_statevectors_to_circuit();
		void extract_dumped_statevectors_from_simulator();
		double add_qite_parameter_for_pauli_string(QC::Pauli_String Pauli_String, double norm);
		void get_expectation_values_for_pauli_strings(std::vector<QC::Pauli_String> Pauli_Strings);
		Eigen::MatrixXcd calculate_S_Matrix();
		Eigen::VectorXcd calculate_b_vector(Pauli_String pauli, double c);
		std::tuple<int, std::complex<double>> pauli_matrix_multiplication(std::string PauliMatrix1, std::string PauliMatrix2);
		std::vector<Pauli_String> get_all_pauli_strings(int number_qubits, QC::Pauli_String Pauli_String);
		std::tuple<std::complex<double>, QC::Pauli_String> multiply_two_pauli_strings(QC::Pauli_String pauli_string1, QC::Pauli_String pauli_string2);
		double get_single_expectation_value(QC::Pauli_String PauliString);
	};
}