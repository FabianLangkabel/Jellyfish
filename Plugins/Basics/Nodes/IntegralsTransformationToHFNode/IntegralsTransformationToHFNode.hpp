#pragma once
#include <QtWidgets>

#include <string>
#include "../../../../JellyfishCore/include/NodeSystem/node.hpp"
#include "../../../../JellyfishCore/include/json.hpp"

#include "../../Lib/header/transformation_integrals_basisfunction_to_hforbitals.h"
#include "../../save_load.h"

using json = nlohmann::json;


class IntegralsTransformationToHFNode : public Node
{
public:
    IntegralsTransformationToHFNode(std::string InpNodeTypeID);
    ~IntegralsTransformationToHFNode();
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
  std::weak_ptr<Eigen::MatrixXd> _HFCMatrix;

  std::shared_ptr<Eigen::MatrixXd> _oneelectronintegralsout;
  std::shared_ptr<Eigen::Tensor<double, 4>> _twoelectronintegralsout;

	Eigen::MatrixXd one_e_integrals;
	Eigen::Tensor<double, 4> two_e_integrals;
};