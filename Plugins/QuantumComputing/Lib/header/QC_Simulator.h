#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <stdio.h>
#include "basics.h"

namespace QC
{
	class QC_Simulator
	{
	public:
		virtual long long int perform_circuit(int number_qubits, QC::QGateSequence algorithm) { return 0; };
		int type_initial_State = 0; //0 = ZeroState, 1 = State from Amps
		QC::QC_Statevector initial_Statevector;

		std::vector<QC::QC_Statevector> dumped_statevectors;
		std::vector<double> dumped_probabilities;

	private:
		virtual void prepare_initial_Statevector_from_amps() {}
	};
}