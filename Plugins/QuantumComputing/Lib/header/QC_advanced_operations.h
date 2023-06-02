#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <stdio.h>
#include <cmath>
#include "basics.h"

namespace QC
{

	class QC_advanced_operations
	{
	private:
		QC::QGateSequence qft_rotations(QC::QGateSequence, int act_qubit, int lowest_qubit);
		void apply_trotter(QC::Pauli_Operator* Pauli_Operator, double trotter_time, int trotter_order, QC::QGateSequence* seq);
		void apply_exponentiated_PauliOperator(QC::Pauli_Operator* Pauli_Operator, double trotter_tim, QC::QGateSequence* seq, bool inverse);

	public:
		QC::QGateSequence get_trotter_seq(QC::Pauli_Operator Pauli_Operator, double trotter_time, int trotter_order);
		QC::QGateSequence get_exp_paulistring(QC::Pauli_String Pauli_String, double coef);
		QC::QGateSequence transform_seq_to_controlled_version(QC::QGateSequence sequence, int controll_qubit);
		QC::QGateSequence quantum_fourier_transformation(int lowest_qubit, int number_qubits);
		QC::QGateSequence inverse_quantum_fourier_transformation(int lowest_qubit, int number_qubits);
	};
}