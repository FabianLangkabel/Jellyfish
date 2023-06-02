#pragma once
#include <QtWidgets>

#include <string>
#include "../../../../JellyfishCore/include/NodeSystem/node.hpp"
#include "../../../../JellyfishCore/include/json.hpp"
#include "../../save_load.h"

using json = nlohmann::json;


class InputElectronNumberNode : public Node, public QObject
{
public:
    InputElectronNumberNode(std::string InpNodeTypeID);
    ~InputElectronNumberNode();
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
  std::shared_ptr<int> _electrons;
  int electrons = 0;

private:
	QLineEdit* selectedelectrons;
	void selectelectrons();
};