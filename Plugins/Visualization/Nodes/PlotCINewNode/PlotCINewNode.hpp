#pragma once
#include <QtWidgets>

#include <string>
#include "../../../../JellyfishCore/include/NodeSystem/node.hpp"
#include "../../../../JellyfishCore/include/json.hpp"

#include "../../Lib/header/tdci_wf.h"
#include "../../Lib/header/gaussian_basisset.h"
#include "../../Lib/header/pointcharge.h"
#include "../../Lib/header/CIResults.h"

#include "plot_ci_new_window.h"

using json = nlohmann::json;


class PlotCINewNode : public Node, public QObject
{
public:
    PlotCINewNode(std::string InpNodeTypeID);
    ~PlotCINewNode();
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
  std::weak_ptr<Eigen::MatrixXd> _hfcmatrix;
  std::weak_ptr<QC::Gaussian_Basisset> _basissetdata;
  std::weak_ptr<std::vector<QC::Pointcharge>> _pointcharges;

private:
    void plot();
};