#include "FlowScene.hpp"

#include <stdexcept>
#include <utility>

#include <QtWidgets/QGraphicsSceneMoveEvent>
#include <QtWidgets/QFileDialog>
#include <QtCore/QByteArray>
#include <QtCore/QBuffer>
#include <QtCore/QDataStream>
#include <QtCore/QFile>

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QtGlobal>
#include <QtCore/QDebug>

#include "GUINode.hpp"
#include "NodeGraphicsObject.hpp"
#include "ConnectionGraphicsObject.hpp"
#include "GUIConnection.hpp"
#include "FlowView.hpp"

using QtNodes::FlowScene;
using QtNodes::GUINode;
using QtNodes::NodeGraphicsObject;
using QtNodes::GUIConnection;
using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::TypeConverter;


FlowScene::FlowScene(std::shared_ptr<CoreSystem> CoreSystem, QObject* parent) : QGraphicsScene(parent)
{
  //_CoreSystem = std::move(CoreSystem);
  _CoreSystem = CoreSystem;
  _project = _CoreSystem->createProject();
  setItemIndexMethod(QGraphicsScene::NoIndex);

  // This connection should come first
  connect(this, &FlowScene::connectionCreated, this, &FlowScene::setupConnectionSignals);
  connect(this, &FlowScene::connectionCreated, this, &FlowScene::sendConnectionCreatedToNodes);
  connect(this, &FlowScene::connectionDeleted, this, &FlowScene::sendConnectionDeletedToNodes);

  connect(this, &FlowScene::connectionCreated, this, &FlowScene::sendConnectionCreatedToProject);
  connect(this, &FlowScene::connectionDeleted, this, &FlowScene::sendConnectionDeletedToProject);
}

FlowScene::FlowScene(std::shared_ptr<CoreSystem> CoreSystem, std::string ProjectFile, QObject* parent) : QGraphicsScene(parent)
{
  //_CoreSystem = std::move(CoreSystem);
  _CoreSystem = CoreSystem;
  _project = _CoreSystem->loadProject(ProjectFile, false);
  setItemIndexMethod(QGraphicsScene::NoIndex);

  // This connection should come first
  connect(this, &FlowScene::connectionCreated, this, &FlowScene::setupConnectionSignals);
  connect(this, &FlowScene::connectionCreated, this, &FlowScene::sendConnectionCreatedToNodes);
  connect(this, &FlowScene::connectionDeleted, this, &FlowScene::sendConnectionDeletedToNodes);

  connect(this, &FlowScene::connectionCreated, this, &FlowScene::sendConnectionCreatedToProject);
  connect(this, &FlowScene::connectionDeleted, this, &FlowScene::sendConnectionDeletedToProject);

  connect(this, &FlowScene::connectionLoaded, this, &FlowScene::setupConnectionSignals);
  connect(this, &FlowScene::connectionLoaded, this, &FlowScene::sendConnectionCreatedToNodes);

  restoreFromProject();
}

FlowScene::~FlowScene()
{
  clearScene();
  _CoreSystem->closeProject(_project->get_ProjectNumber());
}


//------------------------------------------------------------------------------

std::shared_ptr<GUIConnection>FlowScene::createConnection(PortType connectedPort,GUINode& node,PortIndex portIndex)
{
  std::shared_ptr<::Connection> conn = std::make_shared<::Connection>(node._node, portIndex, node._node, portIndex);

  auto connection = std::make_shared<GUIConnection>(connectedPort, node, portIndex, conn);
  auto cgo = detail::make_unique<ConnectionGraphicsObject>(*this, *connection);
  // after this function connection points are set to node port
  connection->setGraphicsObject(std::move(cgo));
  _connections[connection->id()] = connection;

  // Note: this connection isn't truly created yet. It's only partially created.
  // Thus, don't send the connectionCreated(...) signal.
  connect(connection.get(), &GUIConnection::connectionCompleted, this, [this](GUIConnection const& c) {connectionCreated(c);});
  return connection;
}


std::shared_ptr<GUIConnection> FlowScene::createConnection(GUINode& nodeIn, PortIndex portIndexIn, GUINode& nodeOut, PortIndex portIndexOut, TypeConverter const &converter)
{
  std::shared_ptr<::Connection> conn = std::make_shared<::Connection>(nodeOut._node, portIndexOut, nodeIn._node, portIndexIn);

  auto connection = std::make_shared<GUIConnection>(nodeIn, portIndexIn, nodeOut, portIndexOut, conn);
  auto cgo = detail::make_unique<ConnectionGraphicsObject>(*this, *connection);

  nodeIn.setConnection(PortType::In, portIndexIn, *connection);
  nodeOut.setConnection(PortType::Out, portIndexOut, *connection);

  // after this function connection points are set to node port
  connection->setGraphicsObject(std::move(cgo));

  // trigger data propagation
  nodeOut.onDataUpdated(portIndexOut);
  _connections[connection->id()] = connection;
  connectionCreated(*connection);

  return connection;
}

std::shared_ptr<GUIConnection> FlowScene::loadConnection(std::shared_ptr<::Connection> Connection)
{
  std::string InNodeID = Connection->_InNode->id;
  GUINode& nodeIn = *_nodes[InNodeID].get();
  PortIndex portIndexIn = Connection->_InPort;

  std::string OutNodeID = Connection->_OutNode->id;
  GUINode& nodeOut = *_nodes[OutNodeID].get();
  PortIndex portIndexOut = Connection->_OutPort;

  auto connection = std::make_shared<GUIConnection>(nodeIn, portIndexIn, nodeOut, portIndexOut, Connection);
  auto cgo = detail::make_unique<ConnectionGraphicsObject>(*this, *connection);

  nodeIn.setConnection(PortType::In, portIndexIn, *connection);
  nodeOut.setConnection(PortType::Out, portIndexOut, *connection);

  // after this function connection points are set to node port
  connection->setGraphicsObject(std::move(cgo));

  // trigger data propagation
  nodeOut.onDataUpdated(portIndexOut);
  _connections[connection->id()] = connection;
  connectionLoaded(*connection);

  return connection;
}



std::shared_ptr<GUIConnection> FlowScene::restoreConnection(QJsonObject const &connectionJson)
{
  /*
  QUuid nodeInId  = QUuid(connectionJson["in_id"].toString());
  QUuid nodeOutId = QUuid(connectionJson["out_id"].toString());

  PortIndex portIndexIn  = connectionJson["in_index"].toInt();
  PortIndex portIndexOut = connectionJson["out_index"].toInt();

  auto nodeIn  = _nodes[nodeInId].get();
  auto nodeOut = _nodes[nodeOutId].get();

  auto getConverter = [&]()
  {
    QJsonValue converterVal = connectionJson["converter"];

    if (!converterVal.isUndefined())
    {
      QJsonObject converterJson = converterVal.toObject();

      NodeDataType inType { converterJson["in"].toObject()["id"].toString(),
                            converterJson["in"].toObject()["name"].toString() };

      NodeDataType outType { converterJson["out"].toObject()["id"].toString(),
                             converterJson["out"].toObject()["name"].toString() };

      auto converter = registry().getTypeConverter(outType, inType);
      if (converter)
        return converter;
    }
    return TypeConverter{};
  };

  std::shared_ptr<Connection> connection = createConnection(*nodeIn, portIndexIn, *nodeOut, portIndexOut, getConverter());

  // Note: the connectionCreated(...) signal has already been sent
  // by createConnection(...)

  return connection;
  */
}


void FlowScene::deleteConnection(GUIConnection const& connection)
{
  auto it = _connections.find(connection.id());
  if (it != _connections.end())
  {
    std::string ConnectionID = connection.id();
    connection.removeFromNodes();
    _connections.erase(it);
  }
}

/*
GUINode& FlowScene::createNode(std::unique_ptr<NodeDataModel> && dataModel)
{
  auto node = detail::make_unique<GUINode>(std::move(dataModel));
  auto ngo  = detail::make_unique<NodeGraphicsObject>(*this, *node);

  node->setGraphicsObject(std::move(ngo));

  auto nodePtr = node.get();
  _nodes[node->id()] = std::move(node);

  nodeCreated(*nodePtr);
  return *nodePtr;
}
*/
GUINode& FlowScene::createNode(std::string plugin, std::string modelName) //NEW
{
  std::shared_ptr<::Node> n = _CoreSystem->CreateNode(plugin, modelName);
  this->_project->add_node(n);
  //std::cout << n->id << std::endl;
  auto node = detail::make_unique<GUINode>(n);
  auto ngo  = detail::make_unique<NodeGraphicsObject>(*this, *node);

  node->setGraphicsObject(std::move(ngo));

  auto nodePtr = node.get();
  _nodes[node->id()] = std::move(node);

  nodeCreated(*nodePtr);
  
  return *nodePtr;
}

GUINode& FlowScene::restoreNode(QJsonObject const& nodeJson)
{
  /*
  QString modelName = nodeJson["model"].toObject()["name"].toString();

  auto dataModel = registry().create(modelName);

  if (!dataModel)
    throw std::logic_error(std::string("No registered model with name ") +
                           modelName.toLocal8Bit().data());

  auto node = detail::make_unique<Node>(std::move(dataModel));
  auto ngo  = detail::make_unique<NodeGraphicsObject>(*this, *node);
  node->setGraphicsObject(std::move(ngo));

  node->restore(nodeJson);

  auto nodePtr = node.get();
  _nodes[node->id()] = std::move(node);

  nodePlaced(*nodePtr);
  nodeCreated(*nodePtr);
  return *nodePtr;
  */
}


void FlowScene::removeNode(GUINode& node)
{
  std::string NodeID = node.id();
  // call signal
  nodeDeleted(node);
  for(auto portType: {PortType::In,PortType::Out})
  {
    auto nodeEntries = node.getEntries(portType);
    for (auto &connections : nodeEntries)
    {
      for (auto const &pair : connections)
      {
        deleteConnection(*pair.second);
      }
    }
  }
  _nodes.erase(node.id());
  this->_project->remove_node(NodeID);
}

void FlowScene::iterateOverNodes(std::function<void(GUINode*)> const & visitor)
{
  for (const auto& _node : _nodes)
  {
    visitor(_node.second.get());
  }
}


void FlowScene::iterateOverNodeData(std::function<void(GUINode*)> const & visitor)
{
  /*
  for (const auto& _node : _nodes)
  {
    visitor(_node.second->nodeDataModel());
  }
  */
}


void FlowScene::iterateOverNodeDataDependentOrder(std::function<void(GUINode*)> const & visitor)
{
  /*
  std::set<QUuid> visitedNodesSet;

  //A leaf node is a node with no input ports, or all possible input ports empty
  auto isNodeLeaf =
    [](GUINode const &node, NodeDataModel const &model)
    {
      for (unsigned int i = 0; i < model.nPorts(PortType::In); ++i)
      {
        auto connections = node.nodeState().connections(PortType::In, i);
        if (!connections.empty())
        {
          return false;
        }
      }

      return true;
    };

  //Iterate over "leaf" nodes
  for (auto const &_node : _nodes)
  {
    auto const &node = _node.second;
    auto model       = node->nodeDataModel();

    if (isNodeLeaf(*node, *model))
    {
      visitor(model);
      visitedNodesSet.insert(node->id());
    }
  }

  auto areNodeInputsVisitedBefore =
    [&](GUINode const &node, NodeDataModel const &model)
    {
      for (size_t i = 0; i < model.nPorts(PortType::In); ++i)
      {
        auto connections = node.nodeState().connections(PortType::In, static_cast<PortIndex>(i));

        for (auto& conn : connections)
        {
          if (visitedNodesSet.find(conn.second->getNode(PortType::Out)->id()) == visitedNodesSet.end())
          {
            return false;
          }
        }
      }

      return true;
    };

  //Iterate over dependent nodes
  while (_nodes.size() != visitedNodesSet.size())
  {
    for (auto const &_node : _nodes)
    {
      auto const &node = _node.second;
      if (visitedNodesSet.find(node->id()) != visitedNodesSet.end())
        continue;

      auto model = node->nodeDataModel();

      if (areNodeInputsVisitedBefore(*node, *model))
      {
        visitor(model);
        visitedNodesSet.insert(node->id());
      }
    }
  }*/
}


QPointF FlowScene::getNodePosition(const GUINode& node) const
{
  return node.nodeGraphicsObject().pos();
}


void FlowScene::setNodePosition(GUINode& node, const QPointF& pos) const
{
  node.nodeGraphicsObject().setPos(pos);
  node.nodeGraphicsObject().moveConnections();
}


QSizeF FlowScene::getNodeSize(const GUINode& node) const
{
  return QSizeF(node.width(), node.height());
}


std::unordered_map<std::string, std::unique_ptr<GUINode> > const & FlowScene::nodes() const
{
  return _nodes;
}


std::unordered_map<std::string, std::shared_ptr<GUIConnection> > const & FlowScene::connections() const
{
  return _connections;
}


std::vector<GUINode*> FlowScene::allNodes() const
{
  std::vector<GUINode*> nodes;

  std::transform(_nodes.begin(),
                 _nodes.end(),
                 std::back_inserter(nodes),
                 [](std::pair<std::string const, std::unique_ptr<GUINode>> const & p) { return p.second.get(); });

  return nodes;
}


std::vector<GUINode*> FlowScene::selectedNodes() const
{
  QList<QGraphicsItem*> graphicsItems = selectedItems();

  std::vector<GUINode*> ret;
  ret.reserve(graphicsItems.size());

  for (QGraphicsItem* item : graphicsItems)
  {
    auto ngo = qgraphicsitem_cast<NodeGraphicsObject*>(item);

    if (ngo != nullptr)
    {
      ret.push_back(&ngo->node());
    }
  }

  return ret;
}


void FlowScene::restoreFromProject()
{
  for (auto const& [key, val] : _project->get_all_nodes())
  {
    auto node = detail::make_unique<GUINode>(val);
    auto ngo  = detail::make_unique<NodeGraphicsObject>(*this, *node);
    node->setGraphicsObject(std::move(ngo));
    auto nodePtr = node.get();
    _nodes[node->id()] = std::move(node);
    nodeCreated(*nodePtr);

    QPointF pos;
    pos.setX(val->x_position_on_grid);
    pos.setY(val->y_position_on_grid);
    nodePtr->nodeGraphicsObject().setPos(pos);
    nodePlaced(*nodePtr);
  }

  for (auto const& [key, val] : _project->get_all_connections())
  { 
    loadConnection(val);
  }
}

//------------------------------------------------------------------------------

void FlowScene::clearScene()
{
  //Manual node cleanup. Simply clearing the holding datastructures doesn't work, the code crashes when
  // there are both nodes and connections in the scene. (The data propagation internal logic tries to propagate
  // data through already freed connections.)
  while (_connections.size() > 0)
  {
    deleteConnection( *_connections.begin()->second );
  }

  while (_nodes.size() > 0)
  {
    removeNode( *_nodes.begin()->second );
  }
}


void FlowScene::setupConnectionSignals(GUIConnection const& c)
{
  connect(&c,
          &GUIConnection::connectionMadeIncomplete,
          this,
          &FlowScene::connectionDeleted,
          Qt::UniqueConnection);
}


void FlowScene::sendConnectionCreatedToNodes(GUIConnection const& c)
{
  GUINode* from = c.getNode(PortType::Out);
  GUINode* to   = c.getNode(PortType::In);

  Q_ASSERT(from != nullptr);
  Q_ASSERT(to != nullptr);

  from->outputConnectionCreated(c);
  to->inputConnectionCreated(c);
}


void FlowScene::sendConnectionDeletedToNodes(GUIConnection const& c)
{
  GUINode* from = c.getNode(PortType::Out);
  GUINode* to   = c.getNode(PortType::In);

  Q_ASSERT(from != nullptr);
  Q_ASSERT(to != nullptr);

  from->outputConnectionDeleted(c);
  to->inputConnectionDeleted(c);
}

void FlowScene::sendConnectionCreatedToProject(GUIConnection const& c)
{
  this->_project->add_connection(c.getInternConnection());
}


void FlowScene::sendConnectionDeletedToProject(GUIConnection const& c)
{
  this->_project->remove_connection(c.id());
}

//------------------------------------------------------------------------------
namespace QtNodes
{
  GUINode* locateNodeAt(QPointF scenePoint, FlowScene &scene, QTransform const & viewTransform)
  {
    // items under cursor
    QList<QGraphicsItem*> items =
      scene.items(scenePoint,
                  Qt::IntersectsItemShape,
                  Qt::DescendingOrder,
                  viewTransform);

    //// items convertable to NodeGraphicsObject
    std::vector<QGraphicsItem*> filteredItems;

    std::copy_if(items.begin(),
                items.end(),
                std::back_inserter(filteredItems),
                [] (QGraphicsItem * item)
      {
        return (dynamic_cast<NodeGraphicsObject*>(item) != nullptr);
      });

    GUINode* resultNode = nullptr;

    if (!filteredItems.empty())
    {
      QGraphicsItem* graphicsItem = filteredItems.front();
      auto ngo = dynamic_cast<NodeGraphicsObject*>(graphicsItem);

      resultNode = &ngo->node();
    }

    return resultNode;
  }
}
