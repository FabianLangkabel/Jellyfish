#pragma once
#include <QtWidgets>

#include <string>
#include "../../../../JellyfishCore/include/NodeSystem/node.hpp"
#include "../../../../JellyfishCore/include/json.hpp"

#include "../../Lib/header/configuration_interaction_one_electron_operator.h"
#include "../../Lib/header/CIResults.h"
#include "../../save_load.h"

using json = nlohmann::json;


class CIEnergyCAPNode : public Node, public QObject
{
public:
    CIEnergyCAPNode(std::string InpNodeTypeID);
    ~CIEnergyCAPNode();
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
  std::weak_ptr<Eigen::VectorXd> _orbital_energys;
  std::weak_ptr<QC::CIResults> _ci_results;
  std::shared_ptr<Eigen::MatrixXcd> _capmatrix;

  Eigen::MatrixXcd capmatrix;

private:
    QLineEdit* escape_lenght_lineedit;
    double escape_lenght = 50;
    void select_escape_lenght();
};