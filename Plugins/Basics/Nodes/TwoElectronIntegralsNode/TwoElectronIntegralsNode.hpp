#pragma once
#include <QtWidgets>

#include <string>
#include "../../../../JellyfishCore/include/NodeSystem/node.hpp"
#include "../../../../JellyfishCore/include/json.hpp"

#include "../../Lib/header/integrator_libcint_interface_2e.h"
#include "../../save_load.h"

using json = nlohmann::json;


class TwoElectronIntegralsNode : public Node, public QObject
{
public:
    TwoElectronIntegralsNode(std::string InpNodeTypeID);
    ~TwoElectronIntegralsNode();
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
  std::shared_ptr<Eigen::Tensor<double, 4>> _twoelectronintegrals;

  Eigen::Tensor<double, 4> integrals;

private:
    QComboBox* Select_Integral;
    int select_integral_id = 0;
    void selectintegral();
};