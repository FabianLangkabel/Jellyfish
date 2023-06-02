#pragma once
#include <QtWidgets>

#include <string>
#include "../../../../JellyfishCore/include/NodeSystem/node.hpp"
#include "../../../../JellyfishCore/include/json.hpp"

#include "../../Lib/header/integrator_libcint_interface_1e.h"
#include "../../Lib/header/transformation_integrals_basisfunction_to_hforbitals.h"
#include "../../Lib/header/transformation_integrals_hforbitals_to_spinorbitals.h"
#include "../../Lib/header/configuration_interaction_dipole_moment.h"
#include "../../Lib/header/CIResults.h"
#include "../../save_load.h"

#include "get_transitions.h"

using json = nlohmann::json;


class TransitionDipoleMomentsCINode : public Node, public QObject
{
public:
    TransitionDipoleMomentsCINode(std::string InpNodeTypeID);
    ~TransitionDipoleMomentsCINode();
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
  std::weak_ptr<QC::CIResults> _ci_results;
  std::weak_ptr<std::vector<QC::Pointcharge>> _pointcharges;
  
  std::shared_ptr<std::vector<Eigen::MatrixXd>> _transitiondipolemoments;


  std::vector<Eigen::MatrixXd> transitiondipolemomentsdata;

private:
    void start_dipole_analysis();
};