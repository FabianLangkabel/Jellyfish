#pragma once
#include <QtWidgets>

#include <string>
#include "../../../../JellyfishCore/include/NodeSystem/node.hpp"
#include "../../../../JellyfishCore/include/json.hpp"

#include "../../Lib/header/propagator.h"
#include "../../Lib/header/laser.h"
#include "../../Lib/header/tdci_wf.h"
#include "../../Lib/header/CIResults.h"

#include "../../save_load.h"

#include "td_wf_analysis.h"

using json = nlohmann::json;


class PropagatorNode : public Node, public QObject
{
public:
    PropagatorNode(std::string InpNodeTypeID);
    ~PropagatorNode();
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
  std::weak_ptr<std::vector<Eigen::MatrixXd>> _TransitionDipoleMoments;
  std::weak_ptr<QC::CIResults> _ci_results;
  std::weak_ptr<QC::Laser> _laser;
  std::weak_ptr<Eigen::MatrixXcd> _cicapmatrix;
  
  std::shared_ptr<QC::TDCI_WF> _tdci_coeffs;

  QC::TDCI_WF tdci_wf;

private:
    QLineEdit* states_lineedit;
    std::string states_string = "";
    void select_states();

    QLineEdit* steps_lineedit;
    int steps = 0;
    void select_steps();

    QLineEdit* stepsize_lineedit;
    double stepsize = 0;
    void select_stepsize();

    QCheckBox* truncate_states_box;
    bool truncate_states = false;
    void select_truncate_states();
    QLineEdit* truncation_state_lineedit;
    int truncation_state = 100;
    void select_truncation_state();


    QCheckBox* use_laserfile_box;
    bool use_laserfile = false;
    void select_use_laserfile();
    QLabel* laserfile_label;
    std::string full_laserfile = "";
    std::string laserfile = "";
    void select_laserfile();

    void start_analyse_propagation();
};