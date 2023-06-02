#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <stdio.h>
#include "basics.h"
#include "QC_Simulator.h"
#include <QuEST.h>

namespace QC
{
	class QC_Simulator_QuEST : public QC_Simulator
	{
	public:
		long long int perform_circuit(int number_qubits, QC::QGateSequence algorithm) override;

	private:
		Qureg qbitregister;
		void prepare_initial_Statevector_from_amps() override;
	};
}