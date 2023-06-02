#pragma once
#include <QtWidgets>

#include <string>
#include "../../../../JellyfishCore/include/NodeSystem/node.hpp"
#include "../../../../JellyfishCore/include/json.hpp"

#include "../../Lib/header/one_electron_system.h"
#include "../../save_load.h"

using json = nlohmann::json;


class SolveOneElectronSystemNode : public Node, public QObject
{
public:
    SolveOneElectronSystemNode(std::string InpNodeTypeID);
    ~SolveOneElectronSystemNode();
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
  std::weak_ptr<Eigen::MatrixXd> _overlapintegrals;
  std::weak_ptr<Eigen::MatrixXd> _oneelectronoperatorintegrals;
  std::weak_ptr<double> _energy0;

  std::shared_ptr<Eigen::MatrixXd> _cmatrix;
  std::shared_ptr<Eigen::VectorXd> _orbitalenergys;

  Eigen::MatrixXd CMatrix;
  Eigen::VectorXd OrbitalEnergys;

private:
    void show_results();
    QWidget* results_dialog;
};