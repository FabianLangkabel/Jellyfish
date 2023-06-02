#include "qc_dumped_tdstatevecs_to_tdciwf.hpp"

#include "data_types/QCTDStatevectorData.hpp"
#include "data_types/CIMatrix.hpp"
#include "data_types/TDCICoefficients.hpp"
#include "data_types/QCTDNormData.hpp"

#include "data_types/TransitionDipoleMoments.hpp"
#include "data_types/Laser.hpp"

QJsonObject Dumped_TDStatevectors_to_TDCIWF::save()
{
    
    if (is_node_computed)
    {
        save_load::save(this->id.toString().toStdString(), "tdci_wf", this->tdci_wf);
    }

  QJsonObject modelJson;
  modelJson["name"] = name();
  modelJson["computed"] = this->is_node_computed;

  return modelJson;
}

void Dumped_TDStatevectors_to_TDCIWF::restore(QJsonObject const& p)
{
    QString name = p["name"].toString();
    is_node_computed = p["computed"].toBool();

    if (is_node_computed)
    {
        this->tdci_wf = save_load::load(this->id.toString().toStdString(), "tdci_wf", new QC::TDCI_WF);
        _tdci_coeffs = std::make_shared<TDCICoefficientsData>(this->tdci_wf);

        modelValidationState = NodeValidationState::Valid;
        modelValidationError = QStringLiteral("Node computed");
    }
}

unsigned int Dumped_TDStatevectors_to_TDCIWF::nPorts(PortType portType) const
{
  unsigned int result = 1;
  switch (portType)
  {
    case PortType::In:
      result = 3;
      break;

    case PortType::Out:
      result = 1;

    default:
      break;
  }
  return result;
}


NodeDataType Dumped_TDStatevectors_to_TDCIWF::dataType(PortType portType, PortIndex portIndex) const
{
    switch (portType)
    {
    case PortType::In:
        if (portIndex == 0) { return QCTDStatevectorData().type(); }
        if (portIndex == 1) { return CIMatrixData().type(); }
        if (portIndex == 2) { return QCTDNormData().type(); }
    case PortType::Out:
        if (portIndex == 0) { return TDCICoefficientsData().type(); }
    }
}


std::shared_ptr<NodeData> Dumped_TDStatevectors_to_TDCIWF::outData(PortIndex portIndex)
{
    if (portIndex == 0) { return _tdci_coeffs; }
}


void Dumped_TDStatevectors_to_TDCIWF::setInData(std::shared_ptr<NodeData> data, PortIndex portIndex)
{
  if (portIndex == 0)
  {
      auto Data = std::dynamic_pointer_cast<QCTDStatevectorData>(data);
      _qctdstatevector = Data;
  }
  if (portIndex == 1)
  {
      auto Data = std::dynamic_pointer_cast<CIMatrixData>(data);
      _cimatrix = Data;
  }
  if (portIndex == 2)
  {
      auto Data = std::dynamic_pointer_cast<QCTDNormData>(data);
      _tdnorm = Data;
  }
}


NodeValidationState Dumped_TDStatevectors_to_TDCIWF::validationState() const
{
  return modelValidationState;
}


QString Dumped_TDStatevectors_to_TDCIWF::validationMessage() const
{
  return modelValidationError;
}

void Dumped_TDStatevectors_to_TDCIWF::input_or_parameter_changed()
{
    for (auto const& c : this->parentnode->nodeState().connections(PortType::Out, 0)) { c.second->getNode(PortType::In)->nodeDataModel()->input_or_parameter_changed(); }
    this->is_node_computed = false;
    this->modelValidationState = NodeValidationState::Warning;
    this->modelValidationError = QStringLiteral("Node not computed");
}

void Dumped_TDStatevectors_to_TDCIWF::compute_node()
{
    global_generallog->appendPlainText("Start Calculation of 'Dumped TDStatevectors to TDCI-WF' Node");

    tdci_wf.clear();
    auto qctdstatevector = _qctdstatevector.lock()->get();
    auto cimatrix = _cimatrix.lock()->get();
    auto tdnorms = _tdnorm.lock()->get();

    tdci_wf.add_state(0);
    std::vector<QC::QC_Statevector> Statevectors = qctdstatevector.get_td_statevector();
    std::vector<double> times = qctdstatevector.get_times();
    std::vector<double> norms = tdnorms.get_td_norm();
    int entrys = cimatrix.rows();

    for (int i = 0; i < times.size(); i++)
    {
        tdci_wf.add_time(times[i]);

        Eigen::MatrixXcd ci_vector_det_basis(entrys, 1);
        std::vector<QC::QC_Amplitude> Amps = Statevectors[i].get_amplitudes();
        for (int j = 0; j < entrys; j++)
        {
            ci_vector_det_basis(j, 0).real(Amps[j].real_amp);
            ci_vector_det_basis(j, 0).imag(Amps[j].imag_amp);
            ci_vector_det_basis(j, 0) = ci_vector_det_basis(j, 0) * sqrt(norms[i]);
        }
        Eigen::MatrixXcd cimatrix_complex = cimatrix;
        Eigen::VectorXcd x = cimatrix_complex.colPivHouseholderQr().solve(ci_vector_det_basis);
        tdci_wf.add_ci_vectors(x);
    }

    Q_EMIT dataUpdated(0);

    is_node_computed = true;
    modelValidationState = NodeValidationState::Valid;
    modelValidationError = QStringLiteral("Node computed");
    global_generallog->appendPlainText("End Calculation of 'Dumped TDStatevectors to TDCI-WF' Node");
}

void Dumped_TDStatevectors_to_TDCIWF::compute_node_and_dependence()
{
    for (auto const& c : this->parentnode->nodeState().connections(PortType::In, 0)) { c.second->getNode(PortType::Out)->nodeDataModel()->compute_node_and_dependence(); }
    for (auto const& c : this->parentnode->nodeState().connections(PortType::In, 1)) { c.second->getNode(PortType::Out)->nodeDataModel()->compute_node_and_dependence(); }
    for (auto const& c : this->parentnode->nodeState().connections(PortType::In, 2)) { c.second->getNode(PortType::Out)->nodeDataModel()->compute_node_and_dependence(); }
    if (this->is_node_computed == false) { Dumped_TDStatevectors_to_TDCIWF::compute_node(); }
}

void Dumped_TDStatevectors_to_TDCIWF::updateparameterlist(QVBoxLayout* parameterlistlayout)
{

}

void Dumped_TDStatevectors_to_TDCIWF::updateoperationlist(QVBoxLayout* operationlistlayout)
{
    QPushButton* Copy_Uuid_Button = new QPushButton("Copy Node-Uuid to Clipboard");
    connect(Copy_Uuid_Button, &QPushButton::clicked, this, &Dumped_TDStatevectors_to_TDCIWF::copy_Uuid);
    operationlistlayout->addWidget(Copy_Uuid_Button);
    QPushButton* Calculate_Single_Node = new QPushButton("Calculate single Node");
    connect(Calculate_Single_Node, &QPushButton::clicked, this, &Dumped_TDStatevectors_to_TDCIWF::compute_node);
    operationlistlayout->addWidget(Calculate_Single_Node);
    QPushButton* Calculate_connected_Node = new QPushButton("Calculate these and all dependent Nodes");
    connect(Calculate_connected_Node, &QPushButton::clicked, this, &Dumped_TDStatevectors_to_TDCIWF::compute_node_and_dependence);
    operationlistlayout->addWidget(Calculate_connected_Node);
    QPushButton* Propagation_Analysis = new QPushButton("Analyse TDCI-WF");
    connect(Propagation_Analysis, &QPushButton::clicked, this, &Dumped_TDStatevectors_to_TDCIWF::start_analyse_propagation);
    operationlistlayout->addWidget(Propagation_Analysis);
}

void Dumped_TDStatevectors_to_TDCIWF::start_analyse_propagation()
{
    Eigen::VectorXd test;
    TDWF_Analysis_Window* tdwf_analysis_window = new TDWF_Analysis_Window(&this->tdci_wf, test, std::weak_ptr<TransitionDipoleMomentsData>(), std::weak_ptr<LaserData>());
    tdwf_analysis_window->show();
}

void Dumped_TDStatevectors_to_TDCIWF::copy_Uuid()
{
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(this->id.toString().mid(1, id.toString().length() - 2), QClipboard::Clipboard);
}
