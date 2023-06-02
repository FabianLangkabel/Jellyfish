#pragma once
#include <QtWidgets>

#include <string>
#include "../../../../JellyfishCore/include/NodeSystem/node.hpp"
#include "../../../../JellyfishCore/include/json.hpp"

#include "../../Lib/header/configuration_interaction.h"
#include "../../Lib/header/CIResults.h"
#include "../../save_load.h"

using json = nlohmann::json;


class ConfigurationInteractionNode : public Node, public QObject
{
public:
    ConfigurationInteractionNode(std::string InpNodeTypeID);
    ~ConfigurationInteractionNode();
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
  std::weak_ptr<Eigen::MatrixXd> _oneelectronintegrals;
  std::weak_ptr<Eigen::Tensor<double, 4>> _twoelectronintegrals;
  std::weak_ptr<int> _electrons;
  std::weak_ptr<double> _energyshift;
  

  std::shared_ptr<QC::CIResults> _ci_results;

	int excitation_level = 0;
  QC::CIResults ci_results;

private:
    QLineEdit* selectedexcitation;
    void selectexcitation();

    QWidget* results_dialog;
    void show_results();
};