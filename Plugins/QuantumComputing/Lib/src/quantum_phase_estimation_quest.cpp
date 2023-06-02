#include "../header/quantum_phase_estimation_quest.h"


namespace QC
{
	void quantum_phase_estimation_quest::create_init_state_as_lowest_flipped(int number_qubits_to_flip)
	{
		QC::QGateSequence init_state_seq;
		for (int i = 0; i < number_qubits_to_flip; i++)
		{
			QC::QGate flipp;
			flipp.set_term("X");
			flipp.set_qubits_acting_on({ i });
			init_state_seq.add_gate_to_sequence(flipp);
		}
		this->Simulator.perform_circuit(init_state_seq);
	}

	void quantum_phase_estimation_quest::create_qpe_circuit()
	{
		QC::QGateSequence circuit;
		int number_spin_orbitals = this->Pauli_Operator.get_number_qubits();

		//Init Ancilla Register in even Superposition
		for (int i = 0; i < this->prec_qubits; i++)
		{
			int ancilla_qbit = number_spin_orbitals + i;
			QGate H;
			H.set_term("H");
			H.set_qubits_acting_on({ ancilla_qbit });
			circuit.add_gate_to_sequence(H);
		}

		//Calculate Trotter-Step and add controlled Version
		QC::QGateSequence trotter_seq = advanced_operations.get_trotter_seq(this->Pauli_Operator, this->trotter_step_size, this->trotter_order);
		for (int i = 0; i < this->prec_qubits; i++)
		{
			int controll_qbit = number_spin_orbitals + i;
			int iterations = std::pow(2, i);
			QC::QGateSequence trotter_part = advanced_operations.transform_seq_to_controlled_version(trotter_seq, controll_qbit);
			for (int iter = 0; iter < iterations; iter++)
			{
				circuit.add_sequence_to_sequence(trotter_part);
			}
		}

		//Apply inversQFT
		QC::QGateSequence inverse_QFT = advanced_operations.inverse_quantum_fourier_transformation(number_spin_orbitals, this->prec_qubits);
		circuit.add_sequence_to_sequence(inverse_QFT);

		//Measure
		/*
		for (int i = number_spin_orbitals; i < number_spin_orbitals + this->prec_qubits; i++)
		{
			QC::QGate measure;
			measure.set_term("MEASURE");
			measure.set_qubits_acting_on({ i });
			circuit.add_gate_to_sequence(measure);
		}
		*/

		this->qpe_circuit = circuit;
	}

	void quantum_phase_estimation_quest::compute()
	{
		int number_spin_orbitals = this->Pauli_Operator.get_number_qubits();

		const double pi = 3.1415926535;
		this->phases.clear();
		this->positive_energys.clear();
		this->negative_energys.clear();

		create_qpe_circuit();
		int qubits = this->Pauli_Operator.get_number_qubits();
		qubits += this->prec_qubits;

		this->Simulator.initialize_register(qubits);
		if (initial_state_type == 0)
		{
			create_init_state_as_lowest_flipped(this->number_qubits_to_flipp_initial);
		}
		else if (initial_state_type == 1)
		{
			this->Simulator.prepare_Statevector(this->initial_statevector);
		}
		this->Simulator.perform_circuit(this->qpe_circuit);

		std::vector<int> measure;
		for (int i = number_spin_orbitals; i < number_spin_orbitals + this->prec_qubits; i++)
		{
			measure.push_back(i);
		}

		for (int run = 0; run < this->runs; run++)
		{

			long long int measured_binary = this->Simulator.measure_without_collaps(measure);

			//Binary to Decimal
			int measured_decimal = 0;
			{
				int base = 1;
				long long int temp = measured_binary;
				while (temp) {
					int lastDigit = temp % 10;
					temp = temp / 10;
					measured_decimal += lastDigit * base;
					base = base * 2;
				}
			}

			double phase = measured_decimal / std::pow(2, this->prec_qubits);
			double positive_energy = ((2 * pi * phase) * this->trotter_step_size) + this->Pauli_Operator.get_Identity();
			double negative_energy = ((2 * pi * (phase - 1)) * this->trotter_step_size) + this->Pauli_Operator.get_Identity();
			this->phases.push_back(phase);
			this->positive_energys.push_back(positive_energy);
			this->negative_energys.push_back(negative_energy);

			std::cout << "Finished Run: " << run + 1 << "/" << this->runs << std::endl;
		}

	}
}