#pragma once
#include <QtWidgets>

#include <string>
#include "../../../../JellyfishCore/include/NodeSystem/node.hpp"
#include "../../../../JellyfishCore/include/json.hpp"

#include "../../Lib/header/gaussian_basisset.h"
#include "../../save_load.h"

using json = nlohmann::json;


class CombineBasissetsNode : public Node, public QObject
{
public:
    CombineBasissetsNode(std::string InpNodeTypeID);
    ~CombineBasissetsNode();
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
  std::weak_ptr<QC::Gaussian_Basisset> _basissetin1;
  std::weak_ptr<QC::Gaussian_Basisset> _basissetin2;
  std::shared_ptr<QC::Gaussian_Basisset> _basissetout;
  QC::Gaussian_Basisset combined_basisset;
};