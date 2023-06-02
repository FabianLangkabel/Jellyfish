#pragma once
#include <QtCore/QObject>
#include <QtWidgets>

#include <string>
#include "../../../../JellyfishCore/include/NodeSystem/node.hpp"
#include "../../../../JellyfishCore/include/json.hpp"

#include "../../Lib/header/read_molecule.h"
#include "../../save_load.h"

using json = nlohmann::json;


class ReadMoleculeNode : public Node, public QObject
{
public:
    ReadMoleculeNode(std::string InpNodeTypeID);
    ~ReadMoleculeNode();
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
  std::shared_ptr<std::vector<QC::Pointcharge>> _pointcharges;
  std::shared_ptr<QC::Gaussian_Basisset> _basisset;

  std::string moleculefilename;
	std::string basissetname;
	int charge = 0;

  int electrons;
	std::vector<QC::Pointcharge> pointcharges;
	QC::Gaussian_Basisset basisset;

private:
	QLabel *selectedmolecule_label;
	QLabel* selectedbasis_label;
	QComboBox *Select_Basisset;
	QLineEdit *selectedcharge;
	void selectmolecule_dialog();
	void selectbasissetfile();
	void selectcharge();
};