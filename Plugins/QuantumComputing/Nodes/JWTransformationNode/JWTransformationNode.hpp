#pragma once
#include <QtWidgets>

#include <string>
#include "../../../../JellyfishCore/include/NodeSystem/node.hpp"
#include "../../../../JellyfishCore/include/json.hpp"

#include "../../Lib/header/basics.h"
#include "../../Lib/header/jordan_wigner.h"
#include "../../save_load.h"

using json = nlohmann::json;


class JWTransformationNode : public Node, public QObject
{
public:
    JWTransformationNode(std::string InpNodeTypeID);
    ~JWTransformationNode();
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
  std::weak_ptr<Eigen::MatrixXd> _oneelectronintegralsin;
  std::weak_ptr<Eigen::Tensor<double, 4>> _twoelectronintegralsin;
  std::weak_ptr<double> _e0;

  std::shared_ptr<QC::Pauli_Operator> _paulioperator;

  QC::Pauli_Operator Operator;

private:
    QLineEdit* integral_threshold_lineedit;
    double integral_threshold = 0.0000001;
    void select_threshold();
};
