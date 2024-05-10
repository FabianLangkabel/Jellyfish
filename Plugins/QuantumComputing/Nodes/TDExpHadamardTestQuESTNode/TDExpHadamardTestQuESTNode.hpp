#pragma once
#include <QtWidgets>

#include <string>
#include <iostream>
#include <fstream>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include "../../../../JellyfishCore/include/NodeSystem/node.hpp"
#include "../../../../JellyfishCore/include/json.hpp"

#include "../../Lib/header/hadamard_test_pauli_sum_quest.h"
#include "../../save_load.h"

using json = nlohmann::json;


class TDExpHadamardTestQuESTNode : public Node, public QObject
{
public:
    TDExpHadamardTestQuESTNode(std::string InpNodeTypeID);
    ~TDExpHadamardTestQuESTNode();
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
  std::weak_ptr<QC::QC_TD_Statevectors> _tdstatevectors;

  Eigen::VectorXd times;
  Eigen::VectorXd td_vals;

private:
    QLineEdit* skipped_PropaTimeSteps_lineedit;
    int skipped_PropaTimeSteps = 0;
    void select_skipped_PropaTimeSteps();
    QLineEdit* runs_lineedit;
    int runs = 1000;
    void select_runs();

    QLineEdit* prefactor_lineedit;
    double prefactor = 0.2;
    void select_prefactor();
    void show_results();

    QWidget* results_dialog;
    void save_results();
};
