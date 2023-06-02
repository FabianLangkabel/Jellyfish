#include "../header/hadamard_test_pauli_sum_quest.h"


namespace QC
{

	void hadamard_test_pauli_sum_quest::create_circuits()
	{
		const double pi = 3.1415926535;

		QC::QGateSequence real_circuit;
		QC::QGateSequence imag_circuit;

		int number_spin_orbitals = this->Pauli_Operator.get_number_qubits();

		int ancilla_qbit = number_spin_orbitals;

		//H
		{
			QGate H;
			H.set_term("H");
			H.set_qubits_acting_on({ ancilla_qbit });
			real_circuit.add_gate_to_sequence(H);
			imag_circuit.add_gate_to_sequence(H);
		}

		//Rz(pi/2) for Im-Part
		{
			QGate Rz;
			Rz.set_term("RZ");
			Rz.set_parameters({ -pi / 2.0 });
			Rz.set_qubits_acting_on({ ancilla_qbit });
			imag_circuit.add_gate_to_sequence(Rz);
		}

		//CU
		{
			QC::QGateSequence temp_seq = advanced_operations.get_trotter_seq(this->Pauli_Operator, this->prefactor, 1);
			QC::QGateSequence temp_seq2 = advanced_operations.transform_seq_to_controlled_version(temp_seq, ancilla_qbit);
			real_circuit.add_sequence_to_sequence(temp_seq2);
			imag_circuit.add_sequence_to_sequence(temp_seq2);
		}

		//H
		{
			QGate H;
			H.set_term("H");
			H.set_qubits_acting_on({ ancilla_qbit });
			real_circuit.add_gate_to_sequence(H);
			imag_circuit.add_gate_to_sequence(H);
		}

		this->real_circuit = real_circuit;
		this->imag_circuit = imag_circuit;
	}


	void hadamard_test_pauli_sum_quest::compute()
	{
		int number_spin_orbitals = this->Pauli_Operator.get_number_qubits();
		int qubits = number_spin_orbitals + 1;
		create_circuits();
		double real = 0;
		double imag = 0;

		{
			this->Simulator.initialize_register(qubits);
			this->Simulator.prepare_Statevector(this->initial_statevector);
			this->Simulator.perform_circuit(this->real_circuit);

			for (int run = 0; run < this->runs; run++)
			{
				long long int measured_binary = this->Simulator.measure_without_collaps(std::vector<int>{ qubits - 1 });
				if(measured_binary == 0)
				{
					real += 1;
				}
				else
				{
					real -= 1;
				}
			}
			real = real / this->runs;
			this->Simulator.destroy_register();
		}

		{
			this->Simulator.initialize_register(qubits);
			this->Simulator.prepare_Statevector(this->initial_statevector);
			this->Simulator.perform_circuit(this->imag_circuit);

			for (int run = 0; run < this->runs; run++)
			{
				long long int measured_binary = this->Simulator.measure_without_collaps(std::vector<int>{ qubits - 1 });
				if (measured_binary == 0)
				{
					imag += 1;
				}
				else
				{
					imag -= 1;
				}
			}
			imag = imag / this->runs;
			this->Simulator.destroy_register();
		}
		
		std::complex<double> temp(real, imag);

		this->expectation_value = ((std::complex<double>(0,1) * log(temp)).real() / this->prefactor) + this->Pauli_Operator.get_Identity();

		//std::cout << "Result: " << this->expectation_value << std::endl;
		//std::cout << "Result: " << real << " + " << imag << "i" << std::endl;
		//std::cout << "Identity: " << this->Pauli_Operator.get_Identity() << std::endl;
	}
}