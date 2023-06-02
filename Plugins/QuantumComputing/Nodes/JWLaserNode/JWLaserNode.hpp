#pragma once
#include <QtWidgets>

#include <string>
#include "../../../../JellyfishCore/include/NodeSystem/node.hpp"
#include "../../../../JellyfishCore/include/json.hpp"

#include "../../Lib/header/basics.h"
#include "../../Lib/header/laser.h"
#include "../../Lib/header/jordan_wigner.h"
#include "../../save_load.h"

using json = nlohmann::json;


class JWLaserNode : public Node, public QObject
{
public:
    JWLaserNode(std::string InpNodeTypeID);
    ~JWLaserNode();
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
  std::weak_ptr<QC::Gaussian_Basisset> _basisset;
  std::weak_ptr<QC::Laser> _laser;
  std::weak_ptr<Eigen::MatrixXd> _hfcmatrix;

  std::shared_ptr<QC::Laser_Pauli_Operator> _laserpaulioperator;

  QC::JW_Laser_Pauli_Operator laseroperator;

private:
    QLineEdit* integral_threshold_lineedit;
    double integral_threshold = 0.0000001;
    void select_threshold();
};
