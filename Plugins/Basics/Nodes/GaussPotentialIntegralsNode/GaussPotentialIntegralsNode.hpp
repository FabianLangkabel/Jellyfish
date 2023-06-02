#pragma once
#include <QtWidgets>

#include <string>
#include "../../../../JellyfishCore/include/NodeSystem/node.hpp"
#include "../../../../JellyfishCore/include/json.hpp"

#include "../../Lib/header/potential_gaussian.h"
#include "../../Lib/header/integrator_gaussian_potential_gaussian.h"
#include "../../Lib/header/gaussian_basisset.h"

#include "../../save_load.h"

using json = nlohmann::json;


class GaussPotentialIntegralsNode : public Node
{
public:
    GaussPotentialIntegralsNode(std::string InpNodeTypeID);
    ~GaussPotentialIntegralsNode();
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
  std::weak_ptr<std::vector<QC::Potential_Gaussian>> _gausspotentials;
  std::weak_ptr<QC::Gaussian_Basisset> _basisset;
  std::shared_ptr<Eigen::MatrixXd> _oneelectronintegrals;

	Eigen::MatrixXd integrals;
};