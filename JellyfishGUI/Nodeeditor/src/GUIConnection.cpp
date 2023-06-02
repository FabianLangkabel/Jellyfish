#include "GUIConnection.hpp"

#include <cmath>
#include <utility>

#include <QtWidgets/QtWidgets>
#include <QtGlobal>

#include "GUINode.hpp"
#include "FlowScene.hpp"
#include "FlowView.hpp"
#include "NodeGraphicsObject.hpp"

#include "ConnectionState.hpp"
#include "ConnectionGeometry.hpp"
#include "ConnectionGraphicsObject.hpp"

using QtNodes::GUIConnection;
using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::ConnectionState;
using QtNodes::GUINode;
using QtNodes::ConnectionGraphicsObject;
using QtNodes::ConnectionGeometry;

GUIConnection::
GUIConnection(PortType portType, GUINode& node, PortIndex portIndex, std::shared_ptr<::Connection> conn)
  : _outPortIndex(INVALID)
  , _inPortIndex(INVALID)
  , _connectionState()
{
  _connection = conn;
  _uid = conn->_Id;
  setNodeToPort(node, portType, portIndex);

  setRequiredPort(oppositePort(portType));
}


GUIConnection::
GUIConnection(GUINode& nodeIn, PortIndex portIndexIn, GUINode& nodeOut, PortIndex portIndexOut, std::shared_ptr<::Connection> conn)
  : _outNode(&nodeOut)
  , _inNode(&nodeIn)
  , _outPortIndex(portIndexOut)
  , _inPortIndex(portIndexIn)
  , _connectionState()
{
  _connection = conn;
  _uid = conn->_Id;
  setNodeToPort(nodeIn, PortType::In, portIndexIn);
  setNodeToPort(nodeOut, PortType::Out, portIndexOut);
}


GUIConnection::
~GUIConnection()
{
  if (complete())
  {
    connectionMadeIncomplete(*this);
  }

  if (_inNode)
  {
    _inNode->nodeGraphicsObject().update();
  }

  if (_outNode)
  {
    propagateEmptyData();
    _outNode->nodeGraphicsObject().update();
  }
}


QJsonObject
GUIConnection::
save() const
{
  /*
  QJsonObject connectionJson;

  if (_inNode && _outNode)
  {
    connectionJson["in_id"] = _inNode->id().toString();
    connectionJson["in_index"] = _inPortIndex;

    connectionJson["out_id"] = _outNode->id().toString();
    connectionJson["out_index"] = _outPortIndex;

    if (_converter)
    {
      auto getTypeJson = [this](PortType type)
      {
        QJsonObject typeJson;
        NodeDataType nodeType = this->dataType(type);
        typeJson["id"] = nodeType.id;
        typeJson["name"] = nodeType.name;

        return typeJson;
      };

      QJsonObject converterTypeJson;

      converterTypeJson["in"] = getTypeJson(PortType::In);
      converterTypeJson["out"] = getTypeJson(PortType::Out);

      connectionJson["converter"] = converterTypeJson;
    }
  }

  return connectionJson;
  */
}


std::string GUIConnection::id() const
{
  return _uid;
}


bool
GUIConnection::
complete() const
{
  return _inNode != nullptr && _outNode != nullptr;
}


void
GUIConnection::
setRequiredPort(PortType dragging)
{
  _connectionState.setRequiredPort(dragging);

  switch (dragging)
  {
    case PortType::Out:
      _outNode      = nullptr;
      _outPortIndex = INVALID;
      break;

    case PortType::In:
      _inNode      = nullptr;
      _inPortIndex = INVALID;
      break;

    default:
      break;
  }
}


PortType
GUIConnection::
requiredPort() const
{
  return _connectionState.requiredPort();
}


void
GUIConnection::
setGraphicsObject(std::unique_ptr<ConnectionGraphicsObject>&& graphics)
{
  _connectionGraphicsObject = std::move(graphics);

  // This function is only called when the ConnectionGraphicsObject
  // is newly created. At this moment both end coordinates are (0, 0)
  // in Connection G.O. coordinates. The position of the whole
  // Connection G. O. in scene coordinate system is also (0, 0).
  // By moving the whole object to the Node Port position
  // we position both connection ends correctly.

  if (requiredPort() != PortType::None)
  {

    PortType attachedPort = oppositePort(requiredPort());

    PortIndex attachedPortIndex = getPortIndex(attachedPort);

    auto node = getNode(attachedPort);

    QTransform nodeSceneTransform =
      node->nodeGraphicsObject().sceneTransform();

    QPointF pos = node->portScenePosition(attachedPortIndex,
                                                         attachedPort,
                                                         nodeSceneTransform);

    _connectionGraphicsObject->setPos(pos);
  }

  _connectionGraphicsObject->move();
}



PortIndex
GUIConnection::
getPortIndex(PortType portType) const
{
  PortIndex result = INVALID;

  switch (portType)
  {
    case PortType::In:
      result = _inPortIndex;
      break;

    case PortType::Out:
      result = _outPortIndex;

      break;

    default:
      break;
  }

  return result;
}


void
GUIConnection::
setNodeToPort(GUINode& node,
              PortType portType,
              PortIndex portIndex)
{
  bool wasIncomplete = !complete();

  auto& nodeWeak = getNode(portType);

  nodeWeak = &node;

  if (portType == PortType::Out)
    _outPortIndex = portIndex;
  else
    _inPortIndex = portIndex;

  _connectionState.setNoRequiredPort();

  updated(*this);
  if (complete() && wasIncomplete) {
    updateInternConnection();
    connectionCompleted(*this);
  }
}


void
GUIConnection::
removeFromNodes() const
{
  if (_inNode)
    _inNode->eraseConnection(PortType::In, _inPortIndex, id());

  if (_outNode)
    _outNode->eraseConnection(PortType::Out, _outPortIndex, id());
}


ConnectionGraphicsObject&
GUIConnection::
getConnectionGraphicsObject() const
{
  return *_connectionGraphicsObject;
}


ConnectionState&
GUIConnection::
connectionState()
{
  return _connectionState;
}


ConnectionState const&
GUIConnection::
connectionState() const
{
  return _connectionState;
}


ConnectionGeometry&
GUIConnection::
connectionGeometry()
{
  return _connectionGeometry;
}


ConnectionGeometry const&
GUIConnection::
connectionGeometry() const
{
  return _connectionGeometry;
}


GUINode*
GUIConnection::
getNode(PortType portType) const
{
  switch (portType)
  {
    case PortType::In:
      return _inNode;
      break;

    case PortType::Out:
      return _outNode;
      break;

    default:
      // not possible
      break;
  }
  return nullptr;
}


GUINode*&
GUIConnection::
getNode(PortType portType)
{
  switch (portType)
  {
    case PortType::In:
      return _inNode;
      break;

    case PortType::Out:
      return _outNode;
      break;

    default:
      // not possible
      break;
  }
  Q_UNREACHABLE();
}


void
GUIConnection::
clearNode(PortType portType)
{
  if (complete())
  {
    connectionMadeIncomplete(*this);
  }

  getNode(portType) = nullptr;

  if (portType == PortType::In)
    _inPortIndex = INVALID;
  else
    _outPortIndex = INVALID;
}


std::string
GUIConnection::
dataType(PortType portType) const
{
  if (_inNode && _outNode)
  {
    auto const & model = (portType == PortType::In) ?
                        _inNode :
                        _outNode;
    PortIndex index = (portType == PortType::In) ? 
                      _inPortIndex :
                      _outPortIndex;

    return model->dataType(portType, index);
  }
  else 
  {
    GUINode* validNode;
    PortIndex index = INVALID;

    if ((validNode = _inNode))
    {
      index    = _inPortIndex;
      portType = PortType::In;
    }
    else if ((validNode = _outNode))
    {
      index    = _outPortIndex;
      portType = PortType::Out;
    }

    if (validNode)
    {
      auto const &model = validNode;

      return model->dataType(portType, index);
    }
  }

  Q_UNREACHABLE();
}


void
GUIConnection::
setTypeConverter(TypeConverter converter)
{
  _converter = std::move(converter);
}


void
GUIConnection::
propagateData(std::any nodeData) const
{
  if (_inNode)
  {
    /*
    if (_converter)
    {
      nodeData = _converter(nodeData);
    }
    */

    _inNode->propagateData(nodeData, _inPortIndex, id());
  }
}


void
GUIConnection::
propagateEmptyData() const
{
  std::any emptyData;

  propagateData(emptyData);
}

void GUIConnection::updateInternConnection()
{
  _connection->_InNode = _inNode->_node;
  _connection->_InPort = _inPortIndex;
  _connection->_OutNode = _outNode->_node;
  _connection->_OutPort = _outPortIndex;
}
