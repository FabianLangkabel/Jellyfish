#pragma once

#include <QtCore/QObject>
#include <QtCore/QUuid>
#include <QtCore/QVariant>

#include "PortType.hpp"

#include "Serializable.hpp"
#include "ConnectionState.hpp"
#include "ConnectionGeometry.hpp"
#include "TypeConverter.hpp"
#include "QUuidStdHash.hpp"
#include "memory.hpp"

#include "../../../JellyfishCore/Core.hpp"

class QPointF;

namespace QtNodes
{

class GUINode;
class ConnectionGraphicsObject;

///
class GUIConnection : public QObject, public Serializable
{

  Q_OBJECT

public:

  /// New Connection is attached to the port of the given Node.
  /// The port has parameters (portType, portIndex).
  /// The opposite connection end will require anothre port.
  GUIConnection(PortType portType,
             GUINode& node,
             PortIndex portIndex,
             std::shared_ptr<::Connection> conn);

  GUIConnection(GUINode& nodeIn,
             PortIndex portIndexIn,
             GUINode& nodeOut,
             PortIndex portIndexOut,
             std::shared_ptr<::Connection> conn);

  GUIConnection(const GUIConnection&) = delete;
  GUIConnection operator=(const GUIConnection&) = delete;

  ~GUIConnection();

public:
  std::shared_ptr<::Connection> _connection;

public:
  void updateInternConnection();
  std::shared_ptr<::Connection> getInternConnection() const { return _connection; }

public:

  QJsonObject
  save() const override;

public:

  std::string id() const;

  /// Remembers the end being dragged.
  /// Invalidates Node address.
  /// Grabs mouse.
  void
  setRequiredPort(PortType portType);
  PortType
  requiredPort() const;

  void
  setGraphicsObject(std::unique_ptr<ConnectionGraphicsObject>&& graphics);

  /// Assigns a node to the required port.
  /// It is assumed that there is a required port, no extra checks
  void
  setNodeToPort(GUINode& node,
                PortType portType,
                PortIndex portIndex);

  void
  removeFromNodes() const;

public:

  ConnectionGraphicsObject&
  getConnectionGraphicsObject() const;

  ConnectionState const &
  connectionState() const;
  ConnectionState&
  connectionState();

  ConnectionGeometry&
  connectionGeometry();

  ConnectionGeometry const&
  connectionGeometry() const;

  GUINode*
  getNode(PortType portType) const;

  GUINode*&
  getNode(PortType portType);

  PortIndex
  getPortIndex(PortType portType) const;

  void
  clearNode(PortType portType);

  std::string dataType(PortType portType) const;

  void
  setTypeConverter(TypeConverter converter);

  bool
  complete() const;

public: // data propagation

  void
  propagateData(std::any nodeData) const;

  void
  propagateEmptyData() const;

Q_SIGNALS:
  void connectionCompleted(GUIConnection const&) const;
  void connectionMadeIncomplete(GUIConnection const&) const;

private:
  std::string _uid;


private:

  GUINode* _outNode = nullptr;
  GUINode* _inNode  = nullptr;

  PortIndex _outPortIndex;
  PortIndex _inPortIndex;

private:

  ConnectionState    _connectionState;
  ConnectionGeometry _connectionGeometry;

  std::unique_ptr<ConnectionGraphicsObject>_connectionGraphicsObject;

  TypeConverter _converter;

Q_SIGNALS:

  void
  updated(GUIConnection& conn) const;
};
}
