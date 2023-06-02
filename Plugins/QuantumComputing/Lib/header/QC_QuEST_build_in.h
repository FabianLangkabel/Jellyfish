#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <stdio.h>
#include "basics.h"
#include <QuEST.h>

namespace QC
{
	class QC_QuEST_build_in
	{
	public:
		void initialize_register(int number_qubits);
		void perform_circuit(QC::QGateSequence algorithm);
		void prepare_Statevector(QC::QC_Statevector Statevector);
		long long int measure_with_collaps(std::vector<int> qubits_to_measure);
		long long int measure_without_collaps(std::vector<int> qubits_to_measure);
		QC::QC_Statevector get_partial_statevector(std::vector<int> amps);
		double get_one_qubit_probabilitie(int qubit);
		double get_pauli_expectation_value(QC::Pauli_String pauli_string);
		double get_pauli_expectation_value_with_measurement(QC::Pauli_String pauli_string, int measurements);
		void destroy_register();

		int register_size;
		int type_initial_State = 0; //0 = ZeroState, 1 = State from Amps

	private:
		QuESTEnv env;
		Qureg qbitregister;
	};
}