#pragma once
#include <QtWidgets>

#include <string>
#include "../../../../JellyfishCore/include/NodeSystem/node.hpp"
#include "../../../../JellyfishCore/include/json.hpp"

#include "../../Lib/header/quantum_propagation_laser_cap.h"
#include "../../Lib/header/CIResults.h"
#include "../../save_load.h"

using json = nlohmann::json;


class TrotterPropagationNode : public Node, public QObject
{
public:
    TrotterPropagationNode(std::string InpNodeTypeID);
    ~TrotterPropagationNode();
    void LoadData(json Data, std::string file) override;
    json SaveData(std::string file) override;
    void calculate() override;
    void NodeInspector(QWidget* Inspector) override;
    bool InPortAllowMultipleConnections(int Port) override;
    unsigned int Ports(NodePortType PortType) override;
    bool IsCustomPortCaption(NodePortType PortType, int Port) override;
    std::string CustomPortCaption(NodePortType PortType, int Port) override;
    std::string DataTypeName(NodePortType PortType, int Port) override;
    std::any getOutData(int Port) override;
    void setInData(int Port, std::any data) override;

private:
  std::weak_ptr<QC::Pauli_Operator> _TIDPauliHamiltonian;
  std::weak_ptr<QC::Laser_Pauli_Operator> _LaserPauliOperator;
  std::weak_ptr<QC::Pauli_Operator> _CAPPauliOperator;
  std::weak_ptr<QC::QC_Simulator> _Simulator;
  std::weak_ptr<QC::QC_Statevector> _InitState;
  std::weak_ptr<QC::CIResults> _ci_results;

  std::shared_ptr<QC::QC_TD_Statevectors> _tdstatevectors;
  std::shared_ptr<QC::QC_TD_Norm> _tdnorm;

  QC::QC_TD_Statevectors dumped_statevectors;
  QC::QC_TD_Norm td_norm;

private:
    QLineEdit* trotter_order_lineedit;
    int trotter_order = 1;
    void select_trotter_order();
    QLineEdit* trotter_timestep_lineedit;
    double trotter_timestep = 1;
    void select_trotter_timestep();
    QLineEdit* trotter_numbertimesteps_lineedit;
    int trotter_numbertimesteps = 1;
    void select_trotter_numbertimesteps();
    QCheckBox* use_lowest_flipped_init_wavefunction_box;
    bool use_lowest_flipped_init_wavefunction = false;
    void select_use_lowest_flipped_init_wavefunction();
    QLineEdit* number_qubits_state1_lineedit;
    int number_qubits_state1 = 2;
    void select_number_qubits_state1();
    QCheckBox* dump_statevector_box;
    bool dump_statevector = false;
    void select_dump_statevector();
    QCheckBox* get_qite_expecation_values_without_measurement_checkbox;
    bool get_qite_expecation_values_without_measurement = false;
    void select_get_qite_expecation_values_without_measurement();
    QLineEdit* measurments_per_qite_expection_value_lineedit;
    int measurments_per_qite_expection_value = 100;
    void select_measurments_per_qite_expection_value();
};
