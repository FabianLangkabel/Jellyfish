#pragma once

#include <QtCore/QObject>
#include <QtWidgets>
#include <nodes/NodeDataModel>
#include <nodes/Node>
#include <nodes/Connection>
#include <nodes/NodeState>
#include <iostream>
#include <fstream>
#include <math.h>

#include "../../QClib/header_quantum_computing/basics.h"

#include "../save_load.h"
#include "../log.h"

#include "../gui/td_wf_analysis.h"

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::NodeValidationState;
using QtNodes::Node;
using QtNodes::Connection;
using QtNodes::NodeState;

class QCTDStatevectorData;
class CIMatrixData;
class TDCICoefficientsData;
class QCTDNormData;

class Dumped_TDStatevectors_to_TDCIWF : public NodeDataModel
{
  Q_OBJECT

public:
    Dumped_TDStatevectors_to_TDCIWF() = default;

  virtual
  ~Dumped_TDStatevectors_to_TDCIWF() = default;

public:

  QString caption() const override
  { 
      return QStringLiteral("Dumped TDStatevectors to TDCI-WF"); 
  }
  bool captionVisible() const override
  { 
      return true; 
  }
  QString name() const override
  { 
      return QStringLiteral("Dumped TDStatevectors to TDCI-WF"); 
  }

public:
  QJsonObject save() override;
  void restore(QJsonObject const& p) override;

public:
  unsigned int nPorts(PortType portType) const override;
  NodeDataType dataType(PortType portType, PortIndex portIndex) const override;
  std::shared_ptr<NodeData> outData(PortIndex port) override;
  void setInData(std::shared_ptr<NodeData>, int) override;
  QWidget * embeddedWidget() override { return nullptr; }
  NodeValidationState validationState() const override;
  QString validationMessage() const override;

  void input_or_parameter_changed() override;
  void compute_node() override;
  void compute_node_and_dependence() override;


private:
  std::weak_ptr<QCTDStatevectorData> _qctdstatevector;
  std::weak_ptr<CIMatrixData> _cimatrix;
  std::weak_ptr<QCTDNormData> _tdnorm;

  std::shared_ptr<TDCICoefficientsData> _tdci_coeffs;

  QC::TDCI_WF tdci_wf;


  NodeValidationState modelValidationState = NodeValidationState::Warning;
  QString modelValidationError = QStringLiteral("Node not computed");

public:
	void updateparameterlist(QVBoxLayout* parameterlistlayout) override;
	void updateoperationlist(QVBoxLayout* operationlistlayout) override;

private:
    void start_analyse_propagation();
    void copy_Uuid();
};
