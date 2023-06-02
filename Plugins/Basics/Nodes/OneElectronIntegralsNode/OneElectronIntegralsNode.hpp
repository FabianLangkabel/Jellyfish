#pragma once
#include <QtWidgets>

#include <string>
#include "../../../../JellyfishCore/include/NodeSystem/node.hpp"
#include "../../../../JellyfishCore/include/json.hpp"

#include "../../Lib/header/integrator_libcint_interface_1e.h"
#include "../../save_load.h"

using json = nlohmann::json;


class OneElectronIntegralsNode : public Node, public QObject
{
public:
    OneElectronIntegralsNode(std::string InpNodeTypeID);
    ~OneElectronIntegralsNode();
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
	Eigen::MatrixXd integrals;

  std::weak_ptr<std::vector<QC::Pointcharge>> _pointcharges;
  std::weak_ptr<QC::Gaussian_Basisset> _basisset;
  std::shared_ptr<Eigen::MatrixXd> _oneelectronintegrals;

private:
    QComboBox* Select_Integral;
    int select_integral_id = 0;
    void selectintegral();
};
