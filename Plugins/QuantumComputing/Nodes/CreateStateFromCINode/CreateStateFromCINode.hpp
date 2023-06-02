#pragma once
#include <QtWidgets>

#include <string>
#include "../../../../JellyfishCore/include/NodeSystem/node.hpp"
#include "../../../../JellyfishCore/include/json.hpp"

#include "../../Lib/header/basics.h"
#include "../../Lib/header/CIResults.h"
#include "../../save_load.h"

using json = nlohmann::json;


class CreateStateFromCINode : public Node, public QObject
{
public:
    CreateStateFromCINode(std::string InpNodeTypeID);
    ~CreateStateFromCINode();
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
  std::weak_ptr<QC::CIResults> _ci_results;
  
  std::shared_ptr<QC::QC_Statevector> _initial_state;

  QC::QC_Statevector Statevector;

private:
    QLineEdit* state_lineedit;
    int state = 0;
    void select_state();
};
