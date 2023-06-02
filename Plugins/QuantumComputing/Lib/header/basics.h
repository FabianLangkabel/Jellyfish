#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <complex>
#include "laser.h"
#include "gaussian_basisset.h"
#include "integrator_libcint_interface_1e.h"
#include "transformation_integrals_basisfunction_to_hforbitals.h"

namespace QC
{
	class QGate
	{
	private:
		std::string term;
		std::vector<int> qubits_acting_on;
		std::vector<double> parameters;
	public:
		void set_term(std::string term) { this->term = term; }
		void set_qubits_acting_on(std::vector<int> qubits_acting_on) { this->qubits_acting_on = qubits_acting_on; }
		void set_parameters(std::vector<double> parameters) { this->parameters = parameters; }

		std::string get_term() { return this->term; }
		std::vector<int> get_qubits_acting_on() { return this->qubits_acting_on; }
		std::vector<double> get_parameters() { return this->parameters; }
		void print() {
			std::cout << term << " acting on ";
			for (int i = 0; i < qubits_acting_on.size(); i++)
			{
				std::cout << qubits_acting_on[i] << " ";
			}
			std::cout << "with parameters ";
			for (int i = 0; i < parameters.size(); i++)
			{
				std::cout << parameters[i] << " ";
			}
			std::cout << std::endl;
		}

	};

	class QGateSequence
	{
	private:
		std::vector<QC::QGate> sequence;

	public:
		void add_gate_to_sequence(QC::QGate Gate) { this->sequence.push_back(Gate); }
		void add_sequence_to_sequence(QC::QGateSequence Gateseq_to_add) { 
			std::vector<QC::QGate> Gates_to_add = Gateseq_to_add.get_sequence();
			for (int i = 0; i < Gates_to_add.size(); i++)
			{
				QC::QGate Gate;
				Gate.set_parameters(Gates_to_add[i].get_parameters());
				Gate.set_qubits_acting_on(Gates_to_add[i].get_qubits_acting_on());
				Gate.set_term(Gates_to_add[i].get_term());
				this->sequence.push_back(Gate);
			}
		}
		std::vector<QC::QGate> get_sequence() { return this->sequence; }
		void print() {
			for (int i = 0; i < sequence.size(); i++) {
				sequence[i].print();
			}
		}
	};

	class Pauli_String
	{
	private:
		int number_qubits;
		std::vector<std::string> PauliOperations;
		std::vector<int> corresponding_Qubits;
		double coefficient;

	public:
		void set_number_qubits(int number_qubits) { this->number_qubits = number_qubits; }
		void set_PauliOperations(std::vector<std::string> PauliOperations) { this->PauliOperations = PauliOperations; }
		void set_corresponding_Qubits(std::vector<int> corresponding_Qubits) { this->corresponding_Qubits = corresponding_Qubits; }
		void set_coefficient(double coefficient) { this->coefficient = coefficient; }

		int get_number_qubits() {return this->number_qubits;}
		std::vector<std::string> get_PauliOperations() { return this->PauliOperations; }
		std::vector<int> get_corresponding_Qubits() { return this->corresponding_Qubits; }
		double get_coefficient() { return this->coefficient; }
		void print_Pauli_String()
		{
			for (int i = 0; i < this->PauliOperations.size(); i++)
			{
				std::cout << PauliOperations[i] << corresponding_Qubits[i] << " ";
			}
			std::cout << "= " << this->coefficient << std::endl;
		}
		void change_single_PauliOperatotion(int index, std::string pauli_operation)
		{
			this->PauliOperations[index] = pauli_operation;
		}

		//Needed for std::map to sort entrys
		bool operator<(const QC::Pauli_String& pauli2) const
		{
			if (number_qubits != pauli2.number_qubits)
			{
				return number_qubits < pauli2.number_qubits;
			}
			else
			{
				int pauli1_value = 0;
				int pauli2_value = 0;

				for (int i = 0; i < this->PauliOperations.size(); i++)
				{

					if (PauliOperations[i] == "I") { pauli1_value += pow(4, this->PauliOperations.size() - i - 1) * 0; }
					else if (PauliOperations[i] == "X") { pauli1_value += pow(4, this->PauliOperations.size() - i - 1) * 1; }
					else if (PauliOperations[i] == "Y") { pauli1_value += pow(4, this->PauliOperations.size() - i - 1) * 2; }
					else if (PauliOperations[i] == "Z") { pauli1_value += pow(4, this->PauliOperations.size() - i - 1) * 3; }

					if (pauli2.PauliOperations[i] == "I") { pauli2_value += pow(4, this->PauliOperations.size() - i - 1) * 0; }
					else if (pauli2.PauliOperations[i] == "X") { pauli2_value += pow(4, this->PauliOperations.size() - i - 1) * 1; }
					else if (pauli2.PauliOperations[i] == "Y") { pauli2_value += pow(4, this->PauliOperations.size() - i - 1) * 2; }
					else if (pauli2.PauliOperations[i] == "Z") { pauli2_value += pow(4, this->PauliOperations.size() - i - 1) * 3; }
				}

				return pauli1_value < pauli2_value;
			}
		}
		bool operator==(const QC::Pauli_String& pauli2) const
		{
			return (this->PauliOperations == pauli2.PauliOperations && this->corresponding_Qubits == pauli2.corresponding_Qubits);
		}
	};

	class Pauli_Operator
	{
	private:
		int number_qubits;
		double Identity;
		std::vector<QC::Pauli_String> Pauli_Strings;

	public:
		void set_number_qubits(int number_qubits) { this->number_qubits = number_qubits; }
		void set_Identity(double Identity) { this->Identity = Identity; }
		void add_pauli_string(QC::Pauli_String Pauli_String) { this->Pauli_Strings.push_back(Pauli_String); }
		std::vector<QC::Pauli_String> get_pauli_strings() { return this->Pauli_Strings; }
		double get_Identity() { return this->Identity; }
		int get_number_qubits() { return this->number_qubits; }
		void print_Pauli_Operator()
		{
			for (int i = 0; i < this->Pauli_Strings.size(); i++)
			{
				Pauli_Strings[i].print_Pauli_String();
			}
		}
		void clear() { this->Pauli_Strings.clear(); }
	};

	class Laser_Pauli_Operator
	{
	private:
		Eigen::MatrixXd transformation_matrix;
		QC::Gaussian_Basisset Basisset;

	protected:
		Eigen::MatrixXd transition_matrix_x;
		Eigen::MatrixXd transition_matrix_y;
		Eigen::MatrixXd transition_matrix_z;
		bool transition_matrices_uptodate = false;
		QC::Laser* Laser;
		void calculate_transition_matrices();

	public:
		void set_transformation_matrix(Eigen::MatrixXd transformation_matrix) { 
			this->transformation_matrix = transformation_matrix; 
			this->transition_matrices_uptodate = false;
		}
		void set_Laser(QC::Laser* Laser) { 
			this->Laser = Laser;
			this->transition_matrices_uptodate = false;
		}
		void set_Basisset(QC::Gaussian_Basisset Basisset) { 
			this->Basisset = Basisset;
			this->transition_matrices_uptodate = false;
		}
		virtual std::vector<QC::Pauli_Operator> get_Pauli_Operators_at_time(double time) {};
		int test;
	};

	class QC_Amplitude
	{
	public:
		long long int statenumber;
		double real_amp;
		double imag_amp;
		QC_Amplitude(int statenumber, double real_part_of_amp, double imag_part_of_amp)
		{
			this->statenumber = statenumber;
			this->real_amp = real_part_of_amp;
			this->imag_amp = imag_part_of_amp;
		}
	};

	class QC_Statevector
	{
	private:
		int number_qubits;
		std::vector<QC::QC_Amplitude> amplitudes;

	public:
		void set_number_qubits(int number_qubits) { this->number_qubits = number_qubits; }
		int get_number_qubits() { return this->number_qubits; }
		void clear_amplitudes() { this->amplitudes.clear(); }

		void add_amplitude(int statenumber, double real_part_of_amp, double imag_part_of_amp) { 
			
			this->amplitudes.push_back(QC::QC_Amplitude(statenumber, real_part_of_amp, imag_part_of_amp));
		}
		std::vector<QC::QC_Amplitude> get_amplitudes() { return this->amplitudes; }
	};

	class QC_TD_Statevectors
	{
	private:
		std::vector<double> times;
		std::vector<QC::QC_Statevector> td_statevector;

	public:
		void add_statevector(double time, QC_Statevector td_statevector)
		{ 
			this->times.push_back(time);
			this->td_statevector.push_back(td_statevector);
		}
		std::vector<double> get_times() { return times; }
		std::vector<QC::QC_Statevector> get_td_statevector() { return td_statevector; }
		void clear()
		{
			this->times.clear();
			this->td_statevector.clear();
		}
	};

	class QC_TD_Norm
	{
	private:
		std::vector<double> times;
		std::vector<double> td_norm;

	public:
		void add_norm(double time, double norm)
		{
			this->times.push_back(time);
			this->td_norm.push_back(norm);
		}
		std::vector<double> get_times() { return times; }
		std::vector<double> get_td_norm() { return td_norm; }
		void clear()
		{
			this->times.clear();
			this->td_norm.clear();
		}
	};
}