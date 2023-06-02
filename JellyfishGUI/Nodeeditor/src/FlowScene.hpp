#pragma once

#include <QtCore/QUuid>
#include <QtWidgets/QGraphicsScene>

#include <unordered_map>
#include <tuple>
#include <functional>
#include <set>

#include "QUuidStdHash.hpp"
#include "TypeConverter.hpp"
#include "PortType.hpp"
#include "memory.hpp"

//#include "../../include/Core.hpp"
#include "../../../JellyfishCore/Core.hpp"
#include "../../../JellyfishCore/Project.h"

namespace QtNodes
{

class FlowItemInterface;
class GUINode;
class NodeGraphicsObject;
class GUIConnection;
class ConnectionGraphicsObject;
class NodeStyle;

/// Scene holds connections and nodes.
class FlowScene
  : public QGraphicsScene
{
  Q_OBJECT
public:

  FlowScene(std::shared_ptr<CoreSystem> CoreSystem, QObject * parent = Q_NULLPTR);
  FlowScene(std::shared_ptr<CoreSystem> CoreSystem, std::string ProjectFile, QObject * parent = Q_NULLPTR);
  ~FlowScene();

private:
  std::shared_ptr<Project> _project;

public:
  std::shared_ptr<Project> get_Project() {return _project;}

public:
  std::shared_ptr<GUIConnection> createConnection(PortType connectedPort, GUINode& node, PortIndex portIndex);
  std::shared_ptr<GUIConnection> createConnection(GUINode& nodeIn, PortIndex portIndexIn, GUINode& nodeOut, PortIndex portIndexOut, TypeConverter const & converter = TypeConverter{});
  std::shared_ptr<GUIConnection> loadConnection(std::shared_ptr<Connection> connection);
  std::shared_ptr<GUIConnection> restoreConnection(QJsonObject const &connectionJson);
  void deleteConnection(GUIConnection const& connection);
  GUINode&createNode(std::string plugin, std::string modelName); 
  GUINode&restoreNode(QJsonObject const& nodeJson);
  void removeNode(GUINode& node);
  void iterateOverNodes(std::function<void(GUINode*)> const & visitor);
  void iterateOverNodeData(std::function<void(GUINode*)> const & visitor);
  void iterateOverNodeDataDependentOrder(std::function<void(GUINode*)> const & visitor);
  QPointF getNodePosition(GUINode const& node) const;
  void setNodePosition(GUINode& node, QPointF const& pos) const;
  QSizeF getNodeSize(GUINode const& node) const;

public:
  std::unordered_map<std::string, std::unique_ptr<GUINode> > const & nodes() const;
  std::unordered_map<std::string, std::shared_ptr<GUIConnection> > const & connections() const;
  std::vector<GUINode*> allNodes() const;
  std::vector<GUINode*> selectedNodes() const;

public:
  void clearScene();

private:
  void restoreFromProject();

Q_SIGNALS:

  /**
   * @brief Node has been created but not on the scene yet.
   * @see nodePlaced()
   */
  void nodeCreated(GUINode &n);

  /**
   * @brief Node has been added to the scene.
   * @details Connect to this signal if need a correct position of node.
   * @see nodeCreated()
   */
  void nodePlaced(GUINode &n);
  void nodeDeleted(GUINode &n);
  void connectionCreated(GUIConnection const &c);
  void connectionLoaded(GUIConnection const &c);
  void connectionDeleted(GUIConnection const &c);
  void nodeMoved(GUINode& n, const QPointF& newLocation);
  void nodeDoubleClicked(GUINode& n);
  void nodeClicked(GUINode& n);
  void connectionHovered(GUIConnection& c, QPoint screenPos);
  void nodeHovered(GUINode& n, QPoint screenPos);
  void connectionHoverLeft(GUIConnection& c);
  void nodeHoverLeft(GUINode& n);
  void nodeContextMenu(GUINode& n, const QPointF& pos);

private:

  using SharedConnection = std::shared_ptr<GUIConnection>;
  using UniqueNode       = std::unique_ptr<GUINode>;

  // DO NOT reorder this member to go after the others.
  // This should outlive all the connections and nodes of
  // the graph, so that nodes can potentially have pointers into it,
  // which is why it comes first in the class.
//  std::shared_ptr<DataModelRegistry> _registry;
  std::shared_ptr<CoreSystem> _CoreSystem;
  std::unordered_map<std::string, SharedConnection> _connections;
  std::unordered_map<std::string, UniqueNode>       _nodes;

private Q_SLOTS:

  void setupConnectionSignals(GUIConnection const& c);
  void sendConnectionCreatedToNodes(GUIConnection const& c);
  void sendConnectionDeletedToNodes(GUIConnection const& c);
  void sendConnectionCreatedToProject(GUIConnection const& c);
  void sendConnectionDeletedToProject(GUIConnection const& c);

};

GUINode* locateNodeAt(QPointF scenePoint, FlowScene &scene, QTransform const & viewTransform);
}
