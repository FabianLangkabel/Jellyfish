#pragma once
#include <QtWidgets>

#include <string>
#include <iostream>
#include <fstream>
#include "../../../../JellyfishCore/include/NodeSystem/node.hpp"
#include "../../../../JellyfishCore/include/json.hpp"

#include "../../Lib/header/quantum_phase_estimation_quest.h"
#include "../../save_load.h"

using json = nlohmann::json;


class QPEQuESTNode : public Node, public QObject
{
public:
    QPEQuESTNode(std::string InpNodeTypeID);
    ~QPEQuESTNode();
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
  std::weak_ptr<QC::Pauli_Operator> _PauliOperator;
  std::weak_ptr<QC::QC_Statevector> _InitStateSeq;

  std::vector<double> phases;
  std::vector<double> positive_energys;
  std::vector<double> negative_energys;

private:
    QLineEdit* runs_lineedit;
    int runs = 10;
    void select_runs();
    QLineEdit* trotter_order_lineedit;
    int trotter_order = 1;
    void select_trotter_order();
    QLineEdit* trotter_timestep_lineedit;
    double trotter_timestep = 1;
    void select_trotter_timestep();
    QLineEdit* prec_qubits_lineedit;
    int prec_qubits = 5;
    void select_prec_qubits();
    QCheckBox* use_lowest_flipped_init_wavefunction_box;
    bool use_lowest_flipped_init_wavefunction = false;
    void select_use_lowest_flipped_init_wavefunction();
    QLineEdit* number_qubits_state1_lineedit;
    int number_qubits_state1 = 2;
    void select_number_qubits_state1();

    void copy_Uuid();
    void show_results();

    QWidget* results_dialog;
};
