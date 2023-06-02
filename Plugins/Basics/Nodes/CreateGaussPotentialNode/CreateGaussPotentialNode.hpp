#pragma once
#include <QtWidgets>

#include <string>
#include "../../../../JellyfishCore/include/NodeSystem/node.hpp"
#include "../../../../JellyfishCore/include/json.hpp"

#include "../../Lib/header/potential_gaussian.h"
#include "../../save_load.h"

using json = nlohmann::json;


class CreateGaussPotentialNode : public Node, public QObject
{
public:
    CreateGaussPotentialNode(std::string InpNodeTypeID);
    ~CreateGaussPotentialNode();
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
  std::shared_ptr<std::vector<QC::Potential_Gaussian>> _gausspotentials;
  std::vector<QC::Potential_Gaussian> gausspotentials;

private:
	QTreeView *gausspotentialview;
	QStandardItem* gausspotentialviewrootnode;
	void add_gauss_potential();
};