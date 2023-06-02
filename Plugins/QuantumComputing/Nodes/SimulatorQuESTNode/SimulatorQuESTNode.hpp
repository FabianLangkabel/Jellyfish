#pragma once
#include <QtWidgets>

#include <string>
#include "../../../../JellyfishCore/include/NodeSystem/node.hpp"
#include "../../../../JellyfishCore/include/json.hpp"

#include "../../Lib/header/QC_Simulator_QuEST.h"
#include "../../save_load.h"

using json = nlohmann::json;


class SimulatorQuESTNode : public Node, public QObject
{
public:
    SimulatorQuESTNode(std::string InpNodeTypeID);
    ~SimulatorQuESTNode();
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
  QC::QC_Simulator_QuEST Simulator;
  std::shared_ptr<QC::QC_Simulator> _simulator;

private:

};
