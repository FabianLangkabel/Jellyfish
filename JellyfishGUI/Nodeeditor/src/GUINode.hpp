#pragma once


#include <QtCore/QObject>
#include <QtCore/QRectF>
#include <QtCore/QPointF>
#include <QtGui/QTransform>
#include <QtGui/QFontMetrics>
#include <QtWidgets>
#include <QtCore/QJsonObject>
#include <vector>
#include <unordered_map>
#include <any>

#include "PortType.hpp"
#include "NodeGraphicsObject.hpp"
#include "NodeStyle.hpp"
#include "NodePainterDelegate.hpp"
#include "ConnectionGraphicsObject.hpp"
#include "Serializable.hpp"
#include "memory.hpp"

#include "../../../JellyfishCore/Core.hpp"

namespace QtNodes
{

class GUIConnection;
class ConnectionState;
class NodeGraphicsObject;
using ConnectionPtrSet = std::unordered_map<std::string, GUIConnection*>;

enum class NodeValidationState
{
  Valid,
  Warning,
  Error
};

enum class ConnectionPolicy
{
  One,
  Many,
};

enum ReactToConnectionState
{
  REACTING,
  NOT_REACTING
};

class GUINode : public QObject, public Serializable
{
  Q_OBJECT

public:
  GUINode(std::shared_ptr<::Node>);
  virtual ~GUINode();

public:
  std::shared_ptr<::Node> _node;

public:
  QJsonObject save() const override;
  void restore(QJsonObject const &json) override;

public:
  std::string id() const;
  void reactToPossibleConnection(PortType, std::string const &, QPointF const & scenePoint);
  void resetReactionToConnection();

public:

  NodeGraphicsObject const & nodeGraphicsObject() const;
  NodeGraphicsObject & nodeGraphicsObject();
  void setGraphicsObject(std::unique_ptr<NodeGraphicsObject>&& graphics);

public Q_SLOTS: // data propagation
  void propagateData(std::any nodeData, PortIndex inPortIndex, const std::string& connectionId); // Propagates incoming data to the underlying model.
  void onDataUpdated(PortIndex index); // Fetches data from model's OUT #index port and propagates it to the connection
  void onNodeSizeUpdated(); // update the graphic part if the size of the embeddedwidget changes

  void inputConnectionCreated(GUIConnection const&){}
  void inputConnectionDeleted(GUIConnection const&){}
  void outputConnectionCreated(GUIConnection const&){}
  void outputConnectionDeleted(GUIConnection const&){}

private:
  std::unique_ptr<NodeGraphicsObject> _nodeGraphicsObject;
  NodeStyle _nodeStyle;

public:
  QString caption() const;
  bool captionVisible() const;
  QString portCaption(PortType, PortIndex) const;
  bool portCaptionVisible(PortType, PortIndex) const;
  QString name();

public:
  unsigned int nPorts(PortType portType) const;
  std::string dataType(PortType portType, PortIndex portIndex) const;

public:
  ConnectionPolicy portOutConnectionPolicy(PortIndex) const { return ConnectionPolicy::Many; }
  ConnectionPolicy portInConnectionPolicy(PortIndex);

  NodeStyle const& nodeStyle() const;
  void setNodeStyle(NodeStyle const& style);

public:
  void setInData(std::any nodeData, PortIndex port);
  void setInData(std::any nodeData, PortIndex port, const std::string& connectionId);  // Use this if portInConnectionPolicy returns ConnectionPolicy::Many

  std::any outData(PortIndex port);
  QWidget * embeddedWidget() const;
  bool resizable() const { return false; }
  NodeValidationState validationState() const { 
    NodeStatus NStatus = _node->GetNodeStatus();
    if(NStatus == NodeStatus::Valid){ return NodeValidationState::Valid; }
    else if(NStatus == NodeStatus::Warning){ return NodeValidationState::Warning; }
    else if(NStatus == NodeStatus::Error){ return NodeValidationState::Error; }
  }
  QString validationMessage() const { 
    return QString::fromStdString(_node->GetNodeStatusText()); 
  }
  NodePainterDelegate* painterDelegate() const { return nullptr; }

Q_SIGNALS:
  void dataInvalidated(PortIndex index);
  void computingStarted();
  void computingFinished();


//NEW FROM NodeGeometry
public:
  unsigned int height() const { return _height; }
  void setHeight(unsigned int h) { _height = h; }
  unsigned int width() const { return _width; }
  void setWidth(unsigned int w) { _width = w; }
  unsigned int entryHeight() const { return _entryHeight; }
  void setEntryHeight(unsigned int h) { _entryHeight = h; }
  unsigned int entryWidth() const { return _entryWidth; }
  void setEntryWidth(unsigned int w) { _entryWidth = w; }
  unsigned int spacing() const { return _spacing; }
  void setSpacing(unsigned int s) { _spacing = s; }
  bool hovered() const { return _hovered; }
  void setHovered(unsigned int h) { _hovered = h; }
  unsigned int nSources() const;
  unsigned int nSinks() const;
  QPointF const& draggingPos() const { return _draggingPos; }
  void setDraggingPosition(QPointF const& pos) { _draggingPos = pos; }
  QRectF entryBoundingRect() const;
  QRectF boundingRect() const;
  void recalculateSize() const; // Updates size unconditionally
  void recalculateSize(QFont const &font) const; // Updates size if the QFontMetrics is changed
  QPointF portScenePosition(PortIndex index, PortType portType, QTransform const & t = QTransform()) const;
  PortIndex checkHitScenePoint(PortType portType, QPointF point, QTransform const & t = QTransform()) const;
  QRect resizeRect() const;
  QPointF widgetPosition() const; // Returns the position of a widget on the Node surface
  int equivalentWidgetHeight() const; // Returns the maximum height a widget can be without causing the node to grow.
  unsigned int validationHeight() const;
  unsigned int validationWidth() const;
  static QPointF 
  calculateNodePositionBetweenNodePorts(PortIndex targetPortIndex, PortType targetPort, GUINode* targetNode,
                                        PortIndex sourcePortIndex, PortType sourcePort, GUINode* sourceNode,
                                        GUINode& newNode);
private:
  unsigned int captionHeight() const;
  unsigned int captionWidth() const;
  unsigned int portWidth(PortType portType) const;
  mutable unsigned int _width;
  mutable unsigned int _height;
  unsigned int _entryWidth;
  mutable unsigned int _inputPortWidth;
  mutable unsigned int _outputPortWidth;
  mutable unsigned int _entryHeight;
  unsigned int _spacing;
  bool _hovered;
  unsigned int _nSources;
  unsigned int _nSinks;
  QPointF _draggingPos;
  mutable QFontMetrics _fontMetrics;
  mutable QFontMetrics _boldFontMetrics;

//NEW FROM NodeState
public:
  std::vector<ConnectionPtrSet> const& getEntries(PortType) const; // Returns vector of connections ID. Some of them can be empty (null)
  std::vector<ConnectionPtrSet> & getEntries(PortType);
  ConnectionPtrSet connections(PortType portType, PortIndex portIndex) const;
  void setConnection(PortType portType, PortIndex portIndex, GUIConnection& connection);
  void eraseConnection(PortType portType, PortIndex portIndex, std::string id);
  ReactToConnectionState reaction() const;
  PortType reactingPortType() const;
  std::string reactingDataType() const;
  void setReaction(ReactToConnectionState reaction, PortType reactingPortType = PortType::None, std::string reactingDataType = "");
  bool isReacting() const;
  void setResizing(bool resizing);
  bool resizing() const;

private:
  std::vector<ConnectionPtrSet> _inConnections;
  std::vector<ConnectionPtrSet> _outConnections;
  ReactToConnectionState _reaction;
  PortType     _reactingPortType;
  std::string _reactingDataType;
  bool _resizing;

//New
public:
  void PositionChanged(QPointF pos);
};
}
