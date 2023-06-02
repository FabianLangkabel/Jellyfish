#pragma once
#include <QtWidgets>

#include <string>
#include "../../JellyfishCore/include/NodeSystem/node.hpp"
#include "../../JellyfishCore/include/json.hpp"

using json = nlohmann::json;


class Testnode : public Node
{
public:
    Testnode(std::string InpNodeTypeID);
    ~Testnode();
    void calculate() override;
    void NodeInspector(QWidget* Inspector) override;
    bool InPortAllowMultipleConnections(int Port) override;
    unsigned int Ports(NodePortType PortType) override;
    bool IsCustomPortCaption(NodePortType PortType, int Port) override;
    std::string CustomPortCaption(NodePortType PortType, int Port) override;
    std::string DataTypeName(NodePortType PortType, int Port) override;
    std::any getOutData(int Port) override;
    void setInData(int Port, std::any data) override;
};