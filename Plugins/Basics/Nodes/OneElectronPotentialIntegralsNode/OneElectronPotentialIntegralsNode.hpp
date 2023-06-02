#pragma once
#include <QtWidgets>

#include <string>
#include "../../../../JellyfishCore/include/NodeSystem/node.hpp"
#include "../../../../JellyfishCore/include/json.hpp"

#include "../../Lib/header/operator_function.h"
#include "../../Lib/header/integrator_operator_function_numerical.h"

#include "../../save_load.h"

using json = nlohmann::json;


class OneElectronPotentialIntegralsNode : public Node, public QObject
{
public:
    OneElectronPotentialIntegralsNode(std::string InpNodeTypeID);
    ~OneElectronPotentialIntegralsNode();
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
  std::shared_ptr<Eigen::MatrixXd> _oneelectronintegrals;
  Eigen::MatrixXd Integrals;

private:
    QLineEdit* potential_formular_lineedit;
    std::string potential_formular = "";
    void select_potential_formular();

    QLineEdit* integrator_range_lineedit;
    double integrator_range = 0;
    void select_integrator_range();

    QLineEdit* integrator_points_lineedit;
    double integrator_points = 0;
    void select_integrator_points();
};