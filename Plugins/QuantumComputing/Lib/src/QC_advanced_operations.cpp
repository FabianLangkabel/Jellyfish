#include "../header/QC_advanced_operations.h"


namespace QC
{

	QC::QGateSequence QC_advanced_operations::get_trotter_seq(QC::Pauli_Operator Pauli_Operator, double trotter_time, int trotter_order)
	{
		QC::QGateSequence trotter_seq;
		apply_trotter(&Pauli_Operator, trotter_time, trotter_order, &trotter_seq);

		return trotter_seq;
	}

	void QC_advanced_operations::apply_trotter(QC::Pauli_Operator* Pauli_Operator, double trotter_time, int trotter_order, QC::QGateSequence* trotter_seq)
	{
		if(trotter_order == 1)
		{
			apply_exponentiated_PauliOperator(Pauli_Operator, trotter_time, trotter_seq, false);
		}
		else if (trotter_order == 2)
		{
			apply_exponentiated_PauliOperator(Pauli_Operator, trotter_time / 2, trotter_seq, false);
			apply_exponentiated_PauliOperator(Pauli_Operator, trotter_time / 2, trotter_seq, true);
		}
		else if (trotter_order > 2)
		{
			double p = 1 / (4 - pow(4, 1 / (trotter_order - 1)));
			int lower = trotter_order - 2;
			apply_trotter(Pauli_Operator, p * trotter_time, lower, trotter_seq);
			apply_trotter(Pauli_Operator, p * trotter_time, lower, trotter_seq);
			apply_trotter(Pauli_Operator, (1-4*p) * trotter_time, lower, trotter_seq);
			apply_trotter(Pauli_Operator, p * trotter_time, lower, trotter_seq);
			apply_trotter(Pauli_Operator, p * trotter_time, lower, trotter_seq);
		}
	}

	void QC_advanced_operations::apply_exponentiated_PauliOperator(QC::Pauli_Operator* Pauli_Operator, double trotter_time, QC::QGateSequence* trotter_seq, bool inverse)
	{
		std::vector<QC::Pauli_String> Pauli_Strings = Pauli_Operator->get_pauli_strings();
		//Apply Exp for every String 

		if (!inverse)
		{
			for (int i = 0; i < Pauli_Strings.size(); i++)
			{
				trotter_seq->add_sequence_to_sequence(get_exp_paulistring(Pauli_Strings[i], trotter_time));
			}
		}
		else
		{
			for (int i = Pauli_Strings.size() - 1; i >= 0 ; i--)
			{
				trotter_seq->add_sequence_to_sequence(get_exp_paulistring(Pauli_Strings[i], trotter_time));
			}
		}
	}

	QC::QGateSequence QC_advanced_operations::get_exp_paulistring(QC::Pauli_String Pauli_String, double coef)
	{
		const double pi = 3.1415926535;
		QC::QGateSequence return_seq;
		QC::QGateSequence change_to_z_basis;
		QC::QGateSequence change_to_original_basis;
		QC::QGateSequence cnot_seq;
		int prev_qbit;
		int highest_target_qbit = -1;
		bool is_not_first_index = false;

		std::vector<std::string> PauliOperations = Pauli_String.get_PauliOperations();
		std::vector<int> correspondingQbits = Pauli_String.get_corresponding_Qubits();

		for (int i = 0; i < PauliOperations.size(); i++)
		{
			if (PauliOperations[i] != "I")
			{
				if (PauliOperations[i] == "X")
				{
					QC::QGate add_to_change_to_z_basis;
					add_to_change_to_z_basis.set_term("RY");
					add_to_change_to_z_basis.set_qubits_acting_on({ correspondingQbits[i] });
					add_to_change_to_z_basis.set_parameters({ -pi / 2.0 });
					change_to_z_basis.add_gate_to_sequence(add_to_change_to_z_basis);

					QC::QGate add_to_change_to_original_basis;
					add_to_change_to_original_basis.set_term("RY");
					add_to_change_to_original_basis.set_qubits_acting_on({ correspondingQbits[i] });
					add_to_change_to_original_basis.set_parameters({ pi / 2.0 });
					change_to_original_basis.add_gate_to_sequence(add_to_change_to_original_basis);
				}
				else if (PauliOperations[i] == "Y")
				{
					QC::QGate add_to_change_to_z_basis;
					add_to_change_to_z_basis.set_term("RX");
					add_to_change_to_z_basis.set_qubits_acting_on({ correspondingQbits[i] });
					add_to_change_to_z_basis.set_parameters({ pi / 2.0 });
					change_to_z_basis.add_gate_to_sequence(add_to_change_to_z_basis);

					QC::QGate add_to_change_to_original_basis;
					add_to_change_to_original_basis.set_term("RX");
					add_to_change_to_original_basis.set_qubits_acting_on({ correspondingQbits[i] });
					add_to_change_to_original_basis.set_parameters({ -pi / 2.0 });
					change_to_original_basis.add_gate_to_sequence(add_to_change_to_original_basis);
				}

				if (is_not_first_index)
				{
					QC::QGate add_to_cnot_seq;
					add_to_cnot_seq.set_term("CNOT");
					add_to_cnot_seq.set_qubits_acting_on({ prev_qbit, correspondingQbits[i] });
					cnot_seq.add_gate_to_sequence(add_to_cnot_seq);
				}
				prev_qbit = correspondingQbits[i];
				highest_target_qbit = correspondingQbits[i];
				is_not_first_index = true;
			}
		}

		if (highest_target_qbit > -1)
		{
			QC::QGate Rotate_z_gate;
			Rotate_z_gate.set_term("RZ");
			Rotate_z_gate.set_qubits_acting_on({ highest_target_qbit });
			Rotate_z_gate.set_parameters({ 2.0 * Pauli_String.get_coefficient() * coef });

			QC::QGateSequence cnot_seq_inverted;
			std::vector<QC::QGate> cnot_seq_gates = cnot_seq.get_sequence();
			for (int i = cnot_seq_gates.size(); i > 0; i--)
			{
				cnot_seq_inverted.add_gate_to_sequence(cnot_seq_gates[i - 1]);
			}


			return_seq.add_sequence_to_sequence(change_to_z_basis);
			return_seq.add_sequence_to_sequence(cnot_seq);
			return_seq.add_gate_to_sequence(Rotate_z_gate);
			return_seq.add_sequence_to_sequence(cnot_seq_inverted);
			return_seq.add_sequence_to_sequence(change_to_original_basis);
		}
		return return_seq;
	}

	QC::QGateSequence QC_advanced_operations::transform_seq_to_controlled_version(QC::QGateSequence sequence, int controll_qubit)
	{
		QC::QGateSequence new_seq;
		std::vector<QC::QGate> gates = sequence.get_sequence();
		for (int i = 0; i < gates.size(); i++)
		{
			QC::QGate Gate = gates[i];
			std::string term = Gate.get_term();
			std::vector<int> qubits_acting_on = Gate.get_qubits_acting_on();
			std::vector<double> parameters = Gate.get_parameters();

			std::vector<int> new_qubits_acting_on = { controll_qubit };
			for (int j = 0; j < qubits_acting_on.size(); j++)
			{
				new_qubits_acting_on.push_back(qubits_acting_on[j]);
			}

			QC::QGate new_Gate;
			new_Gate.set_parameters(parameters);
			new_Gate.set_qubits_acting_on(new_qubits_acting_on);


			if (term == "RX")
			{
				new_Gate.set_term("CRX");
			}
			else if (term == "RY")
			{
				new_Gate.set_term("CRY");
			}
			else if (term == "RZ")
			{
				new_Gate.set_term("CRZ");
			}
			else if (term == "CNOT")
			{
				new_Gate.set_term("CCNOT");
			}

			new_seq.add_gate_to_sequence(new_Gate);
		}
		return new_seq;
	}

	QC::QGateSequence QC_advanced_operations::quantum_fourier_transformation(int lowest_qubit, int number_qubits)
	{
		QC::QGateSequence QFT;
		QFT.add_sequence_to_sequence(qft_rotations(QFT, (lowest_qubit + number_qubits - 1), lowest_qubit));

		int low_index = lowest_qubit;
		int high_index = lowest_qubit + number_qubits - 1;
		while (low_index < high_index)
		{
			QC::QGate SWAP;
			SWAP.set_term("SWAP");
			SWAP.set_qubits_acting_on({ low_index, high_index });
			low_index++;
			high_index--;
			QFT.add_gate_to_sequence(SWAP);
		}

		return QFT;
	}

	QC::QGateSequence QC_advanced_operations::inverse_quantum_fourier_transformation(int lowest_qubit, int number_qubits)
	{
		std::vector<QC::QGate> QFT = quantum_fourier_transformation(lowest_qubit, number_qubits).get_sequence();
		QC::QGateSequence iQFT;
		for (int i = QFT.size() - 1; i >= 0; i--)
		{
			iQFT.add_gate_to_sequence(QFT[i]);
		}
		return iQFT;
	}

	QC::QGateSequence QC_advanced_operations::qft_rotations(QC::QGateSequence seq, int act_qubit, int lowest_qubit)
	{
		//std::cout << act_qubit << " " << lowest_qubit << std::endl;
		const double pi = 3.1415926535;

		QC::QGate H_Gate;
		H_Gate.set_term("H");
		H_Gate.set_qubits_acting_on({ act_qubit });
		seq.add_gate_to_sequence(H_Gate);
		if (act_qubit > lowest_qubit)
		{
			for (int i = lowest_qubit; i < act_qubit; i++)
			{
				QC::QGate CP_Gate;
				CP_Gate.set_term("CP");
				CP_Gate.set_qubits_acting_on({ i, act_qubit });
				double angle = pi / std::pow(2, (act_qubit - i));
				CP_Gate.set_parameters({ angle });
				seq.add_gate_to_sequence(CP_Gate);
			}
			seq = qft_rotations(seq, act_qubit - 1, lowest_qubit);
		}
		return seq;
	}
}