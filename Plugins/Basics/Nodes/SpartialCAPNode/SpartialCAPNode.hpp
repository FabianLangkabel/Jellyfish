#pragma once
#include <QtWidgets>

#include <string>
#include "../../../../JellyfishCore/include/NodeSystem/node.hpp"
#include "../../../../JellyfishCore/include/json.hpp"

#include "../../Lib/header/integrator_libcint_interface_1e.h"
#include "../../Lib/header/transformation_integrals_basisfunction_to_hforbitals.h"
#include "../../Lib/header/transformation_integrals_hforbitals_to_spinorbitals.h"
#include "../../Lib/header/operator_function.h"
#include "../../Lib/header/integrator_operator_function_numerical.h"
#include "../../Lib/header/configuration_interaction_one_electron_operator.h"
#include "../../Lib/header/CIResults.h"

#include "../../save_load.h"

using json = nlohmann::json;


class SpartialCAPNode : public Node, public QObject
{
public:
    SpartialCAPNode(std::string InpNodeTypeID);
    ~SpartialCAPNode();
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
  
  std::shared_ptr<Eigen::MatrixXcd> _capmatrix;

  Eigen::MatrixXcd capmatrix;


private:
    int cap_type_index = 0;
    QComboBox* cap_type;
    QWidget* CAP0_settings;
    QWidget* CAP1_settings;
    QVBoxLayout* CAP0_Layout;
    QVBoxLayout* CAP1_Layout;
    void select_cap_type();

    //Only for CAP1
    QLineEdit* cap0_radius_lineedit;
    double cap0_radius = 0;
    void select_cap0_radius();

    //Only for CAP1
    QLineEdit* cap_strength_lineedit;
    double cap0_strength = 0;
    void select_cap_strength();

    //Only for CAP1
    QLineEdit* cap_order_lineedit;
    int cap0_order = 0;
    void select_cap_order();

    //Only for CAP2
    QLineEdit* cap1_radius_lineedit;
    double cap1_radius = 0;
    void select_cap1_radius();

    //Only for CAP2
    QLineEdit* cap_potential_max_lineedit;
    double cap1_potential_max = 0;
    void select_cap_potential_max();

    //Only for CAP2
    QLineEdit* cap_curvat_lineedit;
    double cap1_curvat = 0;
    void select_cap_curvat();

    QLineEdit* cap_integrator_range_lineedit;
    double cap_integrator_range = 0;
    void select_cap_integrator_range();

    QLineEdit* cap_integrator_points_lineedit;
    double cap_integrator_points = 0;
    void select_cap_integrator_points();
};