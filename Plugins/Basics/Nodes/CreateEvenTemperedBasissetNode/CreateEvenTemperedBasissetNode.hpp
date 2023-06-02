#pragma once
#include <QtWidgets>

#include <string>
#include "../../../../JellyfishCore/include/NodeSystem/node.hpp"
#include "../../../../JellyfishCore/include/json.hpp"

#include "../../Lib/header/gaussian_basisset.h"

#include "../../save_load.h"

using json = nlohmann::json;


class CreateEvenTemperedBasissetNode : public Node, public QObject
{
public:
    CreateEvenTemperedBasissetNode(std::string InpNodeTypeID);
    ~CreateEvenTemperedBasissetNode();
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
	QC::Gaussian_Basisset basisset;
	std::shared_ptr<QC::Gaussian_Basisset> _basisset;

private:
	QTreeView* eventemperedview;
	QStandardItem* eventemperedviewrootnode;
	QComboBox* basiset_type_box;
	void add_eventempered();
	void select_basiset_type();
};