#include "GUINode.hpp"

#include <QtCore/QObject>
#include <QtGlobal>

#include <utility>
#include <iostream>
#include <cmath>

#include "PortType.hpp"
#include "StyleCollection.hpp"
#include "FlowScene.hpp"
#include "NodeGraphicsObject.hpp"
#include "ConnectionGraphicsObject.hpp"
#include "ConnectionState.hpp"

using QtNodes::GUINode;
using QtNodes::NodeGraphicsObject;
using QtNodes::PortIndex;
using QtNodes::PortType;
using QtNodes::NodeStyle;

GUINode::GUINode(std::shared_ptr<::Node> n)
  : _nodeGraphicsObject(nullptr)
  , _node(n)
  , _width(100)
  , _height(150)
  , _inputPortWidth(70)
  , _outputPortWidth(70)
  , _entryHeight(20)
  , _spacing(20)
  , _hovered(false)
  , _nSources(nPorts(PortType::Out))
  , _nSinks(nPorts(PortType::In))
  , _draggingPos(-1000, -1000)
  , _fontMetrics(QFont())
  , _boldFontMetrics(QFont())
  , _inConnections(nPorts(PortType::In))
  , _outConnections(nPorts(PortType::Out))
  , _reaction(NOT_REACTING)
  , _reactingPortType(PortType::None)
  , _resizing(false)
{
  //_node = n;
  QFont f; f.setBold(true);
  _boldFontMetrics = QFontMetrics(f);
  recalculateSize();

  // propagate data: model => node
  //connect(_nodeDataModel.get(), &NodeDataModel::dataUpdated, this, &GUINode::onDataUpdated);
  //connect(_nodeDataModel.get(), &NodeDataModel::embeddedWidgetSizeUpdated, this, &GUINode::onNodeSizeUpdated );
}


GUINode::~GUINode() = default;

QJsonObject GUINode::save() const{}
void GUINode::restore(QJsonObject const& json){}


std::string GUINode::id() const
{
  return _node->id;
}


void GUINode::reactToPossibleConnection(PortType reactingPortType, std::string const &reactingNode, QPointF const &scenePoint)
{
  QTransform const t = _nodeGraphicsObject->sceneTransform();
  QPointF p = t.inverted().map(scenePoint);
  setDraggingPosition(p);
  _nodeGraphicsObject->update();
  setReaction(ReactToConnectionState::REACTING, reactingPortType, reactingNode);
}


void GUINode::resetReactionToConnection()
{
  setReaction(ReactToConnectionState::NOT_REACTING);
  _nodeGraphicsObject->update();
}


NodeGraphicsObject const & GUINode::nodeGraphicsObject() const
{
  return *_nodeGraphicsObject.get();
}


NodeGraphicsObject & GUINode::nodeGraphicsObject()
{
  return *_nodeGraphicsObject.get();
}


void GUINode::setGraphicsObject(std::unique_ptr<NodeGraphicsObject>&& graphics)
{
  _nodeGraphicsObject = std::move(graphics);
  recalculateSize();
}

void GUINode::propagateData(std::any nodeData, PortIndex inPortIndex, const std::string& connectionId)
{
  setInData(nodeData, inPortIndex, connectionId);
  //Recalculate the nodes visuals. A data change can result in the node taking more space than before, so this forces a recalculate+repaint on the affected node
  _nodeGraphicsObject->setGeometryChanged();
  recalculateSize();
  _nodeGraphicsObject->update();
  _nodeGraphicsObject->moveConnections();
}


void GUINode::onDataUpdated(PortIndex index)
{
  std::any nodeData = outData(index);
  QtNodes::ConnectionPtrSet conn = connections(PortType::Out, index);
  for (auto const & c : conn)
    c.second->propagateData(nodeData);
}

void GUINode::onNodeSizeUpdated()
{
    if( embeddedWidget() )
    {
        //embeddedWidget()->adjustSize();
    }
    recalculateSize();
    for(PortType type: {PortType::In, PortType::Out})
    {
        for(auto& conn_set : getEntries(type))
        {
            for(auto& pair: conn_set)
            {
                GUIConnection* conn = pair.second;
                conn->getConnectionGraphicsObject().move();
            }
        }
    }
}


//************************************ Ehemals NodeDataType

NodeStyle const& GUINode::nodeStyle() const
{
  return _nodeStyle;
}


void GUINode::setNodeStyle(NodeStyle const& style)
{
  _nodeStyle = style;
}

QString GUINode::caption() const 
{ 
  return QString::fromStdString(_node->NodeName); 
}
QString GUINode::name() 
{
  return QString::fromStdString(_node->NodeName); 
}

bool GUINode::captionVisible() const { return true; }

QtNodes::ConnectionPolicy GUINode::portInConnectionPolicy(PortIndex idx)
{
  if(_node->InPortAllowMultipleConnections(idx))
  {
    return QtNodes::ConnectionPolicy::Many;
  }
  else
  {
    return QtNodes::ConnectionPolicy::One;
  }
}

unsigned int GUINode::nPorts(PortType portType) const
{
  if(portType == PortType::In)
  {
    return _node->Ports(NodePortType::In);
  }
  else
  {
    return _node->Ports(NodePortType::Out);
  }
}

bool GUINode::portCaptionVisible(PortType Type, PortIndex Index) const
{
  if(Type == PortType::In)
  {
    return _node->IsCustomPortCaption(NodePortType::In, Index);
  }
  else
  {
    return _node->IsCustomPortCaption(NodePortType::Out, Index);
  }
}

QString GUINode::portCaption(PortType Type, PortIndex Index) const
{ 
  if(Type == PortType::In)
  {
    return QString::fromStdString(_node->CustomPortCaption(NodePortType::In, Index));
  }
  else
  {
    return QString::fromStdString(_node->CustomPortCaption(NodePortType::Out, Index));
  }
}

std::string GUINode::dataType(PortType portType, PortIndex portIndex) const
{
  if(portType == PortType::In)
  {
    return _node->DataTypeName(NodePortType::In, portIndex);
  }
  else
  {
    return _node->DataTypeName(NodePortType::Out, portIndex);
  }
}

std::any GUINode::outData(PortIndex port) 
{
  return _node->getOutData(port);
}

void GUINode::setInData(std::any data, int port)
{
  _node->setInData(port, data);
}

void GUINode::setInData(std::any data, int port, const std::string& connectionId)
{
  Q_UNUSED(connectionId);
  setInData(data, port);
}

QWidget* GUINode::embeddedWidget() const
{
  return 0;
}



//NEW FROM NodeGeometry
/*
unsigned int GUINode::nSources() const
{
  return nPorts(PortType::Out);
}

unsigned int GUINode::nSinks() const
{
  return nPorts(PortType::In);
}
*/

QRectF GUINode::entryBoundingRect() const
{
  double const addon = 0.0;
  return QRectF(0 - addon,
                0 - addon,
                _entryWidth + 2 * addon,
                _entryHeight + 2 * addon);
}

QRectF GUINode::boundingRect() const
{
  auto const &nodeStyle = StyleCollection::nodeStyle();
  double addon = 4 * nodeStyle.ConnectionPointDiameter;
  return QRectF(0 - addon,
                0 - addon,
                _width + 2 * addon,
                _height + 2 * addon);
}

void GUINode::recalculateSize() const
{
  _entryHeight = _fontMetrics.height();

  {
    unsigned int maxNumOfEntries = std::max(_nSinks, _nSources);
    unsigned int step = _entryHeight + _spacing;
    _height = step * maxNumOfEntries;
  }

  if (auto w = embeddedWidget())
  {
    _height = std::max(_height, static_cast<unsigned>(w->height()));
  }

  _height += captionHeight();

  _inputPortWidth  = portWidth(PortType::In);
  _outputPortWidth = portWidth(PortType::Out);

  _width = _inputPortWidth +
           _outputPortWidth +
           2 * _spacing;

  if (auto w = embeddedWidget())
  {
    _width += w->width();
  }

  _width = std::max(_width, captionWidth());

  if (validationState() != NodeValidationState::Valid)
  {
    _width   = std::max(_width, validationWidth());
    _height += validationHeight() + _spacing;
  }
}

void GUINode::recalculateSize(QFont const & font) const
{
  QFontMetrics fontMetrics(font);
  QFont boldFont = font;
  boldFont.setBold(true);
  QFontMetrics boldFontMetrics(boldFont);

  if (_boldFontMetrics != boldFontMetrics)
  {
    _fontMetrics     = fontMetrics;
    _boldFontMetrics = boldFontMetrics;

    recalculateSize();
  }
}


QPointF GUINode:: portScenePosition(PortIndex index, PortType portType, QTransform const & t) const
{
  auto const &nodeStyle = StyleCollection::nodeStyle();
  unsigned int step = _entryHeight + _spacing;
  QPointF result;
  double totalHeight = 0.0;
  totalHeight += captionHeight();
  totalHeight += step * index;
  // TODO: why?
  totalHeight += step / 2.0;
  switch (portType)
  {
    case PortType::Out:
    {
      double x = _width + nodeStyle.ConnectionPointDiameter;

      result = QPointF(x, totalHeight);
      break;
    }

    case PortType::In:
    {
      double x = 0.0 - nodeStyle.ConnectionPointDiameter;

      result = QPointF(x, totalHeight);
      break;
    }

    default:
      break;
  }
  return t.map(result);
}

PortIndex GUINode::checkHitScenePoint(PortType portType, QPointF const scenePoint, QTransform const & sceneTransform) const
{
  auto const &nodeStyle = StyleCollection::nodeStyle();
  PortIndex result = INVALID;
  if (portType == PortType::None)
    return result;

  double const tolerance = 2.0 * nodeStyle.ConnectionPointDiameter;
  unsigned int const nItems = nPorts(portType);
  for (unsigned int i = 0; i < nItems; ++i)
  {
    auto pp = portScenePosition(i, portType, sceneTransform);

    QPointF p = pp - scenePoint;
    auto    distance = std::sqrt(QPointF::dotProduct(p, p));

    if (distance < tolerance)
    {
      result = PortIndex(i);
      break;
    }
  }
  return result;
}


QRect GUINode::resizeRect() const
{
  unsigned int rectSize = 7;
  return QRect(_width - rectSize,
               _height - rectSize,
               rectSize,
               rectSize);
}


QPointF GUINode::widgetPosition() const
{
  if (auto w = embeddedWidget())
  {
    if (w->sizePolicy().verticalPolicy() & QSizePolicy::ExpandFlag)
    {
      // If the widget wants to use as much vertical space as possible, place it immediately after the caption.
      return QPointF(_spacing + portWidth(PortType::In), captionHeight());
    }
    else
    {
      if (validationState() != NodeValidationState::Valid)
      {
        return QPointF(_spacing + portWidth(PortType::In),
                      (captionHeight() + _height - validationHeight() - _spacing - w->height()) / 2.0);
      }

      return QPointF(_spacing + portWidth(PortType::In), 
                    (captionHeight() + _height - w->height()) / 2.0);
    }
  }
  return QPointF();
}

int GUINode::equivalentWidgetHeight() const
{
  if (validationState() != NodeValidationState::Valid)
  {
    return height() - captionHeight() + validationHeight();
  }

  return height() - captionHeight();
}

unsigned int GUINode::captionHeight() const
{
  if (!captionVisible())
    return 0;

  QString name = caption();

  return _boldFontMetrics.boundingRect(name).height();
}

unsigned int GUINode::captionWidth() const
{
  if (!captionVisible())
    return 0;

  QString name = caption();

  return _boldFontMetrics.boundingRect(name).width();
}

unsigned int GUINode::validationHeight() const
{
  QString msg = validationMessage();

  return _boldFontMetrics.boundingRect(msg).height();
}

unsigned int GUINode::validationWidth() const
{
  QString msg = validationMessage();

  return _boldFontMetrics.boundingRect(msg).width();
}

QPointF GUINode::calculateNodePositionBetweenNodePorts(PortIndex targetPortIndex, PortType targetPort, GUINode* targetNode, 
                                                            PortIndex sourcePortIndex, PortType sourcePort, GUINode* sourceNode, 
                                                            GUINode& newNode)
{
  //Calculating the nodes position in the scene. It'll be positioned half way between the two ports that it "connects". 
  //The first line calculates the halfway point between the ports (node position + port position on the node for both nodes averaged).
  //The second line offsets this coordinate with the size of the new node, so that the new nodes center falls on the originally
  //calculated coordinate, instead of it's upper left corner.
  auto converterNodePos = (sourceNode->nodeGraphicsObject().pos() + sourceNode->portScenePosition(sourcePortIndex, sourcePort) +
    targetNode->nodeGraphicsObject().pos() + targetNode->portScenePosition(targetPortIndex, targetPort)) / 2.0f;
  converterNodePos.setX(converterNodePos.x() - newNode.width() / 2.0f);
  converterNodePos.setY(converterNodePos.y() - newNode.height() / 2.0f);
  return converterNodePos;
}

unsigned int GUINode::portWidth(PortType portType) const
{
  unsigned width = 0;

  for (auto i = 0ul; i < nPorts(portType); ++i)
  {
    QString name;

    if (portCaptionVisible(portType, i))
    {
      name = portCaption(portType, i);
    }
    else
    {
      name = QString::fromStdString(dataType(portType, i));
    }

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    width = std::max(unsigned(_fontMetrics.horizontalAdvance(name)),
                     width);
#else
    width = std::max(unsigned(_fontMetrics.width(name)),
                     width);
#endif
  }

  return width;
}

//NEW FROM NodeState
std::vector<QtNodes::ConnectionPtrSet> const & GUINode::getEntries(PortType portType) const
{
  if (portType == PortType::In)
    return _inConnections;
  else
    return _outConnections;
}

std::vector<QtNodes::ConnectionPtrSet> & GUINode::getEntries(PortType portType)
{
  if (portType == PortType::In)
    return _inConnections;
  else
    return _outConnections;
}

QtNodes::ConnectionPtrSet GUINode::connections(PortType portType, PortIndex portIndex) const
{
  auto const &connections = getEntries(portType);

  return connections[portIndex];
}

void GUINode::setConnection(PortType portType, PortIndex portIndex, GUIConnection& connection)
{
  auto &connections = getEntries(portType);
  connections.at(portIndex).insert(std::make_pair(connection.id(),
                                               &connection));
}

void GUINode::eraseConnection(PortType portType, PortIndex portIndex, std::string id)
{
  getEntries(portType)[portIndex].erase(id);
}

QtNodes::ReactToConnectionState GUINode::reaction() const
{
  return _reaction;
}

PortType GUINode::reactingPortType() const
{
  return _reactingPortType;
}

std::string GUINode::reactingDataType() const
{
  return _reactingDataType;
}

void GUINode::setReaction(ReactToConnectionState reaction, PortType reactingPortType, std::string reactingDataType)
{
  _reaction = reaction;
  _reactingPortType = reactingPortType;
  _reactingDataType = std::move(reactingDataType);
}

bool GUINode::isReacting() const
{
  return _reaction == REACTING;
}

void GUINode::setResizing(bool resizing)
{
  _resizing = resizing;
}

bool GUINode::resizing() const
{
  return _resizing;
}

void GUINode::PositionChanged(QPointF pos)
{
  _node->x_position_on_grid = pos.x();
  _node->y_position_on_grid = pos.y();
}