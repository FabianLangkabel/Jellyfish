#pragma once
#include <QtWidgets>

#include <string>
#include "../../../../JellyfishCore/include/NodeSystem/node.hpp"
#include "../../../../JellyfishCore/include/json.hpp"

#include "../../Lib/header/hartree_fock_closed_shell.h"
#include "../../save_load.h"

using json = nlohmann::json;


class RestrictedHartreeFockNode : public Node, public QObject
{
public:
    RestrictedHartreeFockNode(std::string InpNodeTypeID);
    ~RestrictedHartreeFockNode();
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
  std::weak_ptr<Eigen::MatrixXd> _overlapintegrals;
  std::weak_ptr<Eigen::MatrixXd> _oneelectronoperatorintegrals;
  std::weak_ptr<Eigen::Tensor<double, 4>> _twoelectronoperatorintegrals;
  std::weak_ptr<int> _electrons;
  std::weak_ptr<double> _energy0;

  std::shared_ptr<Eigen::MatrixXd> _hfcmatrix;
  std::shared_ptr<Eigen::MatrixXd> _hfdensitymatrix;
  std::shared_ptr<double> _hfenergy;
  std::shared_ptr<Eigen::VectorXd> _orbitalenergys;

	int max_iteration_steps = 100000;
	double energy_threshold = 0.0000001;
	Eigen::MatrixXd c_matrix;
	Eigen::MatrixXd density_matrix;
	Eigen::VectorXd orbital_energys;
	double energy;

private:
    QLineEdit* selectedmaxiter;
    QLineEdit* selectedthreshold;
    void selectmaxiter();
    void selectthreshold();

    QWidget* results_dialog;
    void show_results();
};