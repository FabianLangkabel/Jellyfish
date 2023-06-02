#pragma once
#include <QtWidgets>

#include <string>
#include "../../../../JellyfishCore/include/NodeSystem/node.hpp"
#include "../../../../JellyfishCore/include/json.hpp"

#include "../../Lib/header/gaussian_basisset.h"
#include "../../Lib/header/pointcharge.h"
#include "../../Lib/header/read_orca_file.h"
#include "../../Lib/header/CIResults.h"

#include "../../save_load.h"

using json = nlohmann::json;


class ReadOrcaNode : public Node, public QObject
{
public:
    ReadOrcaNode(std::string InpNodeTypeID);
    ~ReadOrcaNode();
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
  std::shared_ptr<Eigen::MatrixXd> _hfcmatrix;
  std::shared_ptr<double> _hfenergy;
  std::shared_ptr<Eigen::VectorXd> _orbitalenergys;
  std::shared_ptr<QC::CIResults> _ci_results;

  //QC::ReadOrcaFile FileReader;

  bool has_coordinates_section;
  bool has_basisset_section;
  bool has_SCF_energy_section;
  bool has_orbital_energies_section;
  bool has_molecular_orbitals_section;
  bool has_tddft_excited_states_section;

  int electrons;
  std::vector<QC::Pointcharge> pointcharges;
  QC::Gaussian_Basisset basisset;
  Eigen::MatrixXd SCF_MO_Matrix;
  double scf_energy;
  Eigen::VectorXd orbital_energys;
  QC::CIResults ci_results;

private:
	QLabel *selectedorcafile_label;
	void selectedorcafile_dialog();
	std::string OrcaFilePath = "";

	void show_ci_energys();
	QWidget* show_ci_energys_dialog;
};