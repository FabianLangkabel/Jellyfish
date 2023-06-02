#include "../header/quantum_propagation_laser_cap_quest.h"

namespace QC
{
	void quantum_propagation_laser_cap_quest::create_init_state_as_lowest_flipped(int number_qubits_to_flip)
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

	void quantum_propagation_laser_cap_quest::compute()
	{
		//Preparations
		double norm = 1;
		TD_Norm.clear();
		this->dumped_statevectors.clear();

		this->qbits_for_circuit = this->TimeIndependent_Pauli_Operator.get_number_qubits();
		this->Simulator.initialize_register(this->qbits_for_circuit);
		//State Prep
		if (initial_state_type == 0)
		{
			create_init_state_as_lowest_flipped(this->number_qubits_to_flipp_initial);
		}
		else if (initial_state_type == 1)
		{
			this->Simulator.prepare_Statevector(this->initial_statevector);
		}

		//QITE-Preparation
		std::vector<QC::Pauli_String> pauli_strings_temp = CAP_Operator.get_pauli_strings();

		std::vector<QC::Pauli_String> pauli_strings;
		for (int i = 0; i < pauli_strings_temp.size(); i++)
		{
			pauli_strings_temp[i].set_number_qubits(CAP_Operator.get_number_qubits());
			pauli_strings.push_back(pauli_strings_temp[i]);
		}

		//CAP Identity Preparation
		double cap_identity_norm_change = exp(-2 * this->trotter_step_size * CAP_Operator.get_Identity());

		//Loop over Timesteps
		QC::QC_advanced_operations advanced_operations;
		for (int i = 0; i < this->timesteps; i++)
		{
			double time = i * this->trotter_step_size;

			//Do Laser + TID timestep
			add_laser_and_tid_operator_to_circuit(time);

			//Determine QITE-Parameter and add to full-Circuit
			if (this->use_CAP_Operator)
			{
				//Loop over Hamilton-Terms in CAP Operator, calculate action and Norm-change
				for (int j = 0; j < pauli_strings.size(); j++)
				{
					norm = add_qite_parameter_for_pauli_string(pauli_strings[j], norm);
				}
				//Include Identity of CAP-Operator
				norm = norm * cap_identity_norm_change;
			}
			TD_Norm.add_norm(time, norm);

			if (dump_statevectors)
			{
				this->dumped_statevectors.add_statevector(time, this->Simulator.get_partial_statevector(this->amplitudes_to_dump));
			}
			std::cout << "Finished Timestep: " << i << " from " << this->timesteps << std::endl;
		}
		this->Simulator.destroy_register();
	}

	void quantum_propagation_laser_cap_quest::add_laser_and_tid_operator_to_circuit(double time)
	{
		QC::QC_advanced_operations advanced_operations;
		this->Simulator.perform_circuit(advanced_operations.get_trotter_seq(this->TimeIndependent_Pauli_Operator, this->trotter_step_size, this->trotter_order));
		if (this->use_Laser_Operator)
		{
			std::vector<QC::Pauli_Operator> Laser_Ops = this->Laser_Pauli_Operator->get_Pauli_Operators_at_time(time);
			this->Simulator.perform_circuit(advanced_operations.get_trotter_seq(Laser_Ops[0], this->trotter_step_size, this->trotter_order));
			this->Simulator.perform_circuit(advanced_operations.get_trotter_seq(Laser_Ops[1], this->trotter_step_size, this->trotter_order));
			this->Simulator.perform_circuit(advanced_operations.get_trotter_seq(Laser_Ops[2], this->trotter_step_size, this->trotter_order));
		}
	}


	double quantum_propagation_laser_cap_quest::add_qite_parameter_for_pauli_string(QC::Pauli_String Pauli_String, double norm)
	{
		int number_qubits = Pauli_String.get_number_qubits();

		//Fix Pauli String 
		{
			std::vector<int> new_pauli_idx;
			std::vector<int> old_pauli_idx = Pauli_String.get_corresponding_Qubits();
			std::vector<std::string> new_pauli_ops;
			std::vector<std::string> old_pauli_ops = Pauli_String.get_PauliOperations();
			for (int i = 0; i < number_qubits; i++)
			{
				new_pauli_idx.push_back(i);
				new_pauli_ops.push_back("I");
			}
			Pauli_String.set_PauliOperations(new_pauli_ops);
			Pauli_String.set_corresponding_Qubits(new_pauli_idx);
			for (int i = 0; i < old_pauli_idx.size(); i++)
			{
				Pauli_String.change_single_PauliOperatotion(old_pauli_idx[i], old_pauli_ops[i]);
			}
		}

		//Get exp values
		std::vector<QC::Pauli_String> all_relevant_pauli_strings = get_all_pauli_strings(Pauli_String.get_number_qubits(), Pauli_String);
		get_expectation_values_for_pauli_strings(all_relevant_pauli_strings);

		//Calculate S, c and b
		Eigen::MatrixXcd S = calculate_S_Matrix();
		
		double c = 1 - 2 * this->trotter_step_size * Pauli_String.get_coefficient() * get_single_expectation_value(Pauli_String);


		Eigen::VectorXcd b = calculate_b_vector(Pauli_String, c);

		Eigen::MatrixXcd Solve = S + S.transpose();
		for(int i = 0; i < Solve.cols(); i++)
		{
			Solve(i, i) += std::complex<double>(0.1, 0);
		}

		
		//Eigen::ConjugateGradient<Eigen::MatrixXcd, Eigen::Upper> cg;
		Eigen::LeastSquaresConjugateGradient<Eigen::MatrixXcd> cg;
		//Eigen::BiCGSTAB<Eigen::MatrixXcd> cg;
		cg.setMaxIterations(100000);
		cg.compute(Solve);
		Eigen::VectorXcd a = cg.solve(-b);
		//std::cout << "#iterations:     " << cg.iterations() << std::endl;
		//std::cout << "estimated error: " << cg.error()      << std::endl;
		

		//Eigen::VectorXcd a = Solve.fullPivHouseholderQr().solve(-b);

		QC::QC_advanced_operations ops;
		int i = 0;
		for (int i = 0; i < this->expection_values.pauli_strings.size(); i++)
		{
			if (abs(a[i]) > 0.00001)
			{
				//std::cout << a[i].real() << std::endl;
				this->Simulator.perform_circuit(ops.get_exp_paulistring(this->expection_values.pauli_strings[i], 1 * a[i].real() * this->trotter_step_size));
			}
		}
		

		return (norm * exp(c - 1));
	}

	void quantum_propagation_laser_cap_quest::get_expectation_values_for_pauli_strings(std::vector<QC::Pauli_String> Pauli_Strings)
	{
		QC::expection_values_data_quest data;

		//Get expectation values
		if (get_qite_expecation_values_without_measurement)
		{
			for (int i = 0; i < Pauli_Strings.size(); i++)
			{
				data.add_expection_value(Pauli_Strings[i], this->Simulator.get_pauli_expectation_value(Pauli_Strings[i]));
			}
		}
		else
		{
			for (int i = 0; i < Pauli_Strings.size(); i++)
			{
				data.add_expection_value(Pauli_Strings[i], this->Simulator.get_pauli_expectation_value_with_measurement(Pauli_Strings[i], this->measurments_per_qite_expection_value));
			}
		}

		this->expection_values = data;
	}

	Eigen::MatrixXcd quantum_propagation_laser_cap_quest::calculate_S_Matrix()
	{
		Eigen::MatrixXcd S(this->expection_values.pauli_strings.size(), this->expection_values.pauli_strings.size());

		for (int i = 0; i < this->expection_values.pauli_strings.size(); i++)
		{
			for (int j = 0; j < this->expection_values.pauli_strings.size(); j++)
			{
				std::tuple<std::complex<double>, QC::Pauli_String> prod = multiply_two_pauli_strings(this->expection_values.pauli_strings[i], this->expection_values.pauli_strings[j]);
				double single_expec_value = get_single_expectation_value(std::get<1>(prod));
				S(i, j) = single_expec_value * std::get<0>(prod);
			}
		}
		return S;
	}

	Eigen::VectorXcd quantum_propagation_laser_cap_quest::calculate_b_vector(Pauli_String pauli, double c)
	{
		//Method: N. Gomes et. al. new
		Eigen::VectorXcd b(this->expection_values.pauli_strings.size());

		for (int i = 0; i < this->expection_values.pauli_strings.size(); i++)
		{
			std::tuple<std::complex<double>, QC::Pauli_String> prod = multiply_two_pauli_strings(pauli, this->expection_values.pauli_strings[i]);
			double single_expec_value = get_single_expectation_value(std::get<1>(prod));
			std::complex<double> b_temp = std::complex<double>(0, -1) * pow(c, -0.5) * pauli.get_coefficient() * std::get<0>(prod) * single_expec_value;

			b[i] = b_temp + std::conj(b_temp);
		}
		return b;
		
	}

	std::tuple<int, std::complex<double>> quantum_propagation_laser_cap_quest::pauli_matrix_multiplication(std::string PauliMatrix1, std::string PauliMatrix2)
	{
		int ProduktMatrix = 0;
		std::complex<double> coeff;

		if      (PauliMatrix1 == "I" && PauliMatrix2 == "I") { ProduktMatrix = 0; coeff = std::complex<double>(1, 0);}
		else if (PauliMatrix1 == "I" && PauliMatrix2 == "X") { ProduktMatrix = 1; coeff = std::complex<double>(1, 0); }
		else if (PauliMatrix1 == "I" && PauliMatrix2 == "Y") { ProduktMatrix = 2; coeff = std::complex<double>(1, 0); }
		else if (PauliMatrix1 == "I" && PauliMatrix2 == "Z") { ProduktMatrix = 3; coeff = std::complex<double>(1, 0); }
		else if (PauliMatrix1 == "X" && PauliMatrix2 == "I") { ProduktMatrix = 1; coeff = std::complex<double>(1, 0); }
		else if (PauliMatrix1 == "X" && PauliMatrix2 == "X") { ProduktMatrix = 0; coeff = std::complex<double>(1, 0); }
		else if (PauliMatrix1 == "X" && PauliMatrix2 == "Y") { ProduktMatrix = 3; coeff = std::complex<double>(0, 1); }
		else if (PauliMatrix1 == "X" && PauliMatrix2 == "Z") { ProduktMatrix = 2; coeff = std::complex<double>(0, -1); }
		else if (PauliMatrix1 == "Y" && PauliMatrix2 == "I") { ProduktMatrix = 2; coeff = std::complex<double>(1, 0); }
		else if (PauliMatrix1 == "Y" && PauliMatrix2 == "X") { ProduktMatrix = 3; coeff = std::complex<double>(0, -1); }
		else if (PauliMatrix1 == "Y" && PauliMatrix2 == "Y") { ProduktMatrix = 0; coeff = std::complex<double>(1, 0); }
		else if (PauliMatrix1 == "Y" && PauliMatrix2 == "Z") { ProduktMatrix = 1; coeff = std::complex<double>(0, 1); }
		else if (PauliMatrix1 == "Z" && PauliMatrix2 == "I") { ProduktMatrix = 3; coeff = std::complex<double>(1, 0); }
		else if (PauliMatrix1 == "Z" && PauliMatrix2 == "X") { ProduktMatrix = 2; coeff = std::complex<double>(0, 1); }
		else if (PauliMatrix1 == "Z" && PauliMatrix2 == "Y") { ProduktMatrix = 1; coeff = std::complex<double>(0, -1); }
		else if (PauliMatrix1 == "Z" && PauliMatrix2 == "Z") { ProduktMatrix = 0; coeff = std::complex<double>(1, 0); }

		return std::make_tuple(ProduktMatrix, coeff);
	}

	std::vector<Pauli_String> quantum_propagation_laser_cap_quest::get_all_pauli_strings(int number_qubits, QC::Pauli_String Ref_Pauli)
	{
		std::vector<Pauli_String> all_strings;
		int amount_strings = pow(4, number_qubits);

		std::vector<int> cor_qubits;
		for (int i = 0; i < number_qubits; i++)
		{
			cor_qubits.push_back(i);
		}

		for (int i = 0; i < amount_strings; i++)
		{
			int temp = i;
			std::vector<std::string> pauli_seq;

			for (int j = 0; j < number_qubits; j++)
			{
				int pauli_as_int = temp / pow(4, (number_qubits - j - 1));
				temp = temp % (int)pow(4, (number_qubits - j - 1));
				if (pauli_as_int == 0) { pauli_seq.push_back("I"); }
				else if (pauli_as_int == 1) { pauli_seq.push_back("X"); }
				else if (pauli_as_int == 2) { pauli_seq.push_back("Y"); }
				else if (pauli_as_int == 3) { pauli_seq.push_back("Z"); }
			}
			QC::Pauli_String string;
			string.set_PauliOperations(pauli_seq);
			string.set_coefficient(1);
			string.set_corresponding_Qubits(cor_qubits);
			all_strings.push_back(string);
		}

		return all_strings;
	}

	std::tuple<std::complex<double>, QC::Pauli_String> quantum_propagation_laser_cap_quest::multiply_two_pauli_strings(QC::Pauli_String pauli_string1, QC::Pauli_String pauli_string2)
	{
		QC::Pauli_String result_string;
		result_string.set_number_qubits(pauli_string1.get_number_qubits());
		result_string.set_corresponding_Qubits(pauli_string1.get_corresponding_Qubits());
		result_string.set_coefficient(pauli_string1.get_coefficient() * pauli_string2.get_coefficient());

		std::vector<std::string> paulis_1 = pauli_string1.get_PauliOperations();
		std::vector<std::string> paulis_2 = pauli_string2.get_PauliOperations();

		std::vector<std::string> paulis_result;
		std::complex<double> prefactor = std::complex<double>(1, 0);
		for (int k = 0; k < paulis_1.size(); k++)
		{
			std::tuple<int, std::complex<double>> prod = pauli_matrix_multiplication(paulis_1[k], paulis_2[k]);
			prefactor = prefactor * std::get<1>(prod);
			if (std::get<0>(prod) == 0)
			{
				paulis_result.push_back("I");
			}
			else if (std::get<0>(prod) == 1)
			{
				paulis_result.push_back("X");
			}
			else if (std::get<0>(prod) == 2)
			{
				paulis_result.push_back("Y");
			}
			else if (std::get<0>(prod) == 3)
			{
				paulis_result.push_back("Z");
			}
		}

		result_string.set_PauliOperations(paulis_result);
		return std::make_tuple(prefactor, result_string);
	}

	double quantum_propagation_laser_cap_quest::get_single_expectation_value(QC::Pauli_String PauliString)
	{
		return this->expection_values.get_expection_value(PauliString);
	}

}