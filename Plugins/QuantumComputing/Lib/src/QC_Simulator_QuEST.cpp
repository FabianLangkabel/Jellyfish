#include "../header/QC_Simulator_QuEST.h"
namespace QC
{
	long long int QC_Simulator_QuEST::perform_circuit(int number_qubits, QC::QGateSequence algorithm)
	{
		//Measured Int
		std::string measure_result;

		//Start QuEST-Env
		QuESTEnv env = createQuESTEnv();
		Qureg qbitregister = createQureg(number_qubits, env);

		//State Preparation
		initZeroState(qbitregister);

		if (this->type_initial_State == 1)
		{
			this->qbitregister = qbitregister;
			prepare_initial_Statevector_from_amps();
			qbitregister = this->qbitregister;
		}

		std::vector<QC::QGate> Sequence = algorithm.get_sequence();
		for (int i = 0; i < Sequence.size(); i++)
		{
			QC::QGate Gate = Sequence[i];
			if (Gate.get_term() == "X")
			{
				pauliX(qbitregister, Gate.get_qubits_acting_on()[0]);
			}
			else if (Gate.get_term() == "Y")
			{
				pauliY(qbitregister, Gate.get_qubits_acting_on()[0]);
			}
			else if (Gate.get_term() == "Z")
			{
				pauliZ(qbitregister, Gate.get_qubits_acting_on()[0]);
			}
			else if (Gate.get_term() == "RX")
			{
				rotateX(qbitregister, Gate.get_qubits_acting_on()[0], Gate.get_parameters()[0]);
			}
			else if (Gate.get_term() == "RY")
			{
				rotateY(qbitregister, Gate.get_qubits_acting_on()[0], Gate.get_parameters()[0]);
			}
			else if (Gate.get_term() == "RZ")
			{
				rotateZ(qbitregister, Gate.get_qubits_acting_on()[0], Gate.get_parameters()[0]);
			}
			else if (Gate.get_term() == "H")
			{
				hadamard(qbitregister, Gate.get_qubits_acting_on()[0]);
			}
			else if (Gate.get_term() == "CNOT")
			{
				controlledNot(qbitregister, Gate.get_qubits_acting_on()[0], Gate.get_qubits_acting_on()[1]);
			}
			else if (Gate.get_term() == "CRX")
			{
				controlledRotateX(qbitregister, Gate.get_qubits_acting_on()[0], Gate.get_qubits_acting_on()[1], Gate.get_parameters()[0]);
			}
			else if (Gate.get_term() == "CRY")
			{
				controlledRotateY(qbitregister, Gate.get_qubits_acting_on()[0], Gate.get_qubits_acting_on()[1], Gate.get_parameters()[0]);
			}
			else if (Gate.get_term() == "CRZ")
			{
				controlledRotateZ(qbitregister, Gate.get_qubits_acting_on()[0], Gate.get_qubits_acting_on()[1], Gate.get_parameters()[0]);
			}
			else if (Gate.get_term() == "CCNOT")
			{
				ComplexMatrix2 u;
				u.real[0][0] = 0.0; u.real[0][1] = 1.0; u.real[1][0] = 1.0; u.real[1][1] = 0.0;
				u.imag[0][0] = 0.0; u.imag[0][1] = 0.0; u.imag[1][0] = 0.0; u.imag[1][1] = 0.0;
				int controll_qbits[2] = { Gate.get_qubits_acting_on()[0], Gate.get_qubits_acting_on()[1] };
				multiControlledUnitary(qbitregister, controll_qbits, 2, Gate.get_qubits_acting_on()[2], u);
			}
			else if (Gate.get_term() == "CP")
			{
				controlledPhaseShift(qbitregister, Gate.get_qubits_acting_on()[0], Gate.get_qubits_acting_on()[1], Gate.get_parameters()[0]);
			}
			else if (Gate.get_term() == "SWAP")
			{
				swapGate(qbitregister, Gate.get_qubits_acting_on()[0], Gate.get_qubits_acting_on()[1]);
			}
			else if (Gate.get_term() == "MEASURE")
			{
				int single_qbit_result = measure(qbitregister, Gate.get_qubits_acting_on()[0]);
				measure_result = std::to_string(single_qbit_result) + measure_result;
			}
			else if (Gate.get_term() == "DUMP PARTIAL STATEVECTOR")
			{
				//Dump all Amplitudes  whose number are stored in qubits_acting_on[:-1], last element of qubits_acting_on is number of Qubits
				QC::QC_Statevector dumpedvec;
				std::vector<int> amps_to_dump = Gate.get_qubits_acting_on();
				dumpedvec.set_number_qubits(amps_to_dump[amps_to_dump.size() - 1]);

				for (int i = 0; i < amps_to_dump.size() - 1; i++)
				{
					Complex amp = getAmp(qbitregister, amps_to_dump[i]);
					dumpedvec.add_amplitude(amps_to_dump[i], amp.real, amp.imag);
				}
				this->dumped_statevectors.push_back(dumpedvec);
			}
			else if (Gate.get_term() == "GET ONE QUBIT PROP")
			{
				qreal prop = calcProbOfOutcome(qbitregister, Gate.get_qubits_acting_on()[0], 1);
				this->dumped_probabilities.push_back(prop);
			}
			else if (Gate.get_term() == "GET PAULI EXPECTATION")
			{
				std::vector<int> qubits_acting_on = Gate.get_qubits_acting_on();
				std::vector<double> pauli_as_double = Gate.get_parameters();
				std::vector<pauliOpType> paulis;
				for (int i = 0; i < pauli_as_double.size(); i++)
				{
					if (pauli_as_double[i] == 0) { paulis.push_back(PAULI_I); }
					else if (pauli_as_double[i] == 1) { paulis.push_back(PAULI_X); }
					else if (pauli_as_double[i] == 2) { paulis.push_back(PAULI_Y); }
					else if (pauli_as_double[i] == 3) { paulis.push_back(PAULI_Z); }
				}

				Qureg tempregister = createQureg(number_qubits, env);
				qreal prop = calcExpecPauliProd(qbitregister, &qubits_acting_on[0], &paulis[0], qubits_acting_on.size(), tempregister);
				destroyQureg(tempregister, env);
				this->dumped_probabilities.push_back(prop);
			}
			else
			{
				std::cout << Gate.get_term() << " not found !!!" << std::endl;
			}
		}

		//Destroy
		destroyQureg(qbitregister, env);
		destroyQuESTEnv(env);
		
		long long int measure_bin;
		if (measure_result == "")
		{
			std::cout << "Quantum circuit performed but nothing was measured" << std::endl;
			measure_bin = 0;
		}
		else 
		{
			measure_bin = std::stoll(measure_result);
		}
		return measure_bin;
	}

	void QC_Simulator_QuEST::prepare_initial_Statevector_from_amps()
	{
		double null = 0;
		setAmps(this->qbitregister, 0, &null, &null, 1);

		std::vector<QC::QC_Amplitude> Amps = this->initial_Statevector.get_amplitudes();
		for (int i = 0; i < Amps.size(); i++)
		{
			double real = Amps[i].real_amp;
			double imag = Amps[i].imag_amp;
			setAmps(this->qbitregister, Amps[i].statenumber, &real, &imag, 1);
		}
	}
}