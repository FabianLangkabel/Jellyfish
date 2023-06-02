#pragma once
#include <QtWidgets>

#include <string>
#include "../../../../JellyfishCore/include/NodeSystem/node.hpp"
#include "../../../../JellyfishCore/include/json.hpp"

#include "../../Lib/header/integrator_libcint_interface_1e.h"
#include "../../Lib/header/one_electron_system_dipole_moment.h"
#include "../../Lib/header/transformation_integrals_basisfunction_to_hforbitals.h"

#include "../TransitionDipoleMomentsCINode/get_transitions.h"
#include "../../save_load.h"

using json = nlohmann::json;


class TransitionDipoleMomentsOneElectronNode : public Node, public QObject
{
public:
    TransitionDipoleMomentsOneElectronNode(std::string InpNodeTypeID);
    ~TransitionDipoleMomentsOneElectronNode();
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
  std::weak_ptr<QC::Gaussian_Basisset> _basisset;
  std::weak_ptr<Eigen::MatrixXd> _hfcmatrix;
  
  std::shared_ptr<std::vector<Eigen::MatrixXd>> _transitiondipolemoments;

  std::vector<Eigen::MatrixXd> transitiondipolemomentsdata;

private:
    void start_dipole_analysis();
};