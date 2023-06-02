#include "NodeGraphicsObject.hpp"

#include <iostream>
#include <cstdlib>

#include <QtWidgets/QtWidgets>
#include <QtWidgets/QGraphicsEffect>

#include "ConnectionGraphicsObject.hpp"
#include "ConnectionState.hpp"

#include "FlowScene.hpp"
#include "NodePainter.hpp"

#include "GUINode.hpp"
#include "NodeConnectionInteraction.hpp"

#include "StyleCollection.hpp"

using QtNodes::NodeGraphicsObject;
using QtNodes::GUINode;
using QtNodes::FlowScene;

NodeGraphicsObject::
NodeGraphicsObject(FlowScene &scene,
                   GUINode& node)
  : _scene(scene)
  , _node(node)
  , _locked(false)
  , _proxyWidget(nullptr)
{
  _scene.addItem(this);

  setFlag(QGraphicsItem::ItemDoesntPropagateOpacityToChildren, true);
  setFlag(QGraphicsItem::ItemIsMovable, true);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setFlag(QGraphicsItem::ItemIsSelectable, true);
  setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);

  setCacheMode( QGraphicsItem::DeviceCoordinateCache );

  auto const &nodeStyle = node.nodeStyle();

  {
    auto effect = new QGraphicsDropShadowEffect;
    effect->setOffset(4, 4);
    effect->setBlurRadius(20);
    effect->setColor(nodeStyle.ShadowColor);

    setGraphicsEffect(effect);
  }

  setOpacity(nodeStyle.Opacity);

  setAcceptHoverEvents(true);

  setZValue(0);

  embedQWidget();

  // connect to the move signals to emit the move signals in FlowScene
  auto onMoveSlot = [this] {
    _scene.nodeMoved(_node, pos());
  };
  connect(this, &QGraphicsObject::xChanged, this, onMoveSlot);
  connect(this, &QGraphicsObject::yChanged, this, onMoveSlot);

  connect(this, &QGraphicsObject::xChanged, this, &NodeGraphicsObject::PositionChanged);
  connect(this, &QGraphicsObject::yChanged, this, &NodeGraphicsObject::PositionChanged);
}


NodeGraphicsObject::
~NodeGraphicsObject()
{
  _scene.removeItem(this);
}


GUINode& NodeGraphicsObject::node()
{
  return _node;
}


GUINode const& NodeGraphicsObject::node() const
{
  return _node;
}


void
NodeGraphicsObject::
embedQWidget()
{
  if (auto w = _node.embeddedWidget())
  {
    _proxyWidget = new QGraphicsProxyWidget(this);

    _proxyWidget->setWidget(w);

    _proxyWidget->setPreferredWidth(5);

    _node.recalculateSize();

    if (w->sizePolicy().verticalPolicy() & QSizePolicy::ExpandFlag)
    {
      // If the widget wants to use as much vertical space as possible, set it to have the geom's equivalentWidgetHeight.
      _proxyWidget->setMinimumHeight(_node.equivalentWidgetHeight());
    }

    _proxyWidget->setPos(_node.widgetPosition());

    update();

    _proxyWidget->setOpacity(1.0);
    _proxyWidget->setFlag(QGraphicsItem::ItemIgnoresParentOpacity);
  }
}


QRectF
NodeGraphicsObject::
boundingRect() const
{
  return _node.boundingRect();
}


void
NodeGraphicsObject::
setGeometryChanged()
{
  prepareGeometryChange();
}


void
NodeGraphicsObject::
moveConnections() const
{
  for (PortType portType: {PortType::In, PortType::Out})
  {
    auto const & connectionEntries =
      _node.getEntries(portType);

    for (auto const & connections : connectionEntries)
    {
      for (auto & con : connections)
        con.second->getConnectionGraphicsObject().move();
    }
  }
}


void
NodeGraphicsObject::
lock(bool locked)
{
  _locked = locked;

  setFlag(QGraphicsItem::ItemIsMovable, !locked);
  setFlag(QGraphicsItem::ItemIsFocusable, !locked);
  setFlag(QGraphicsItem::ItemIsSelectable, !locked);
}


void
NodeGraphicsObject::
paint(QPainter * painter,
      QStyleOptionGraphicsItem const* option,
      QWidget* )
{
  painter->setClipRect(option->exposedRect);

  NodePainter::paint(painter, _node, _scene);
}


QVariant
NodeGraphicsObject::
itemChange(GraphicsItemChange change, const QVariant &value)
{
  if (change == ItemPositionChange && scene())
  {
    moveConnections();
  }

  return QGraphicsItem::itemChange(change, value);
}


void
NodeGraphicsObject::
mousePressEvent(QGraphicsSceneMouseEvent * event)
{
  if (_locked)
    return;

  // deselect all other items after this one is selected
  if (!isSelected() &&
      !(event->modifiers() & Qt::ControlModifier))
  {
    _scene.clearSelection();
  }

  for (PortType portToCheck: {PortType::In, PortType::Out})
  {
    // TODO do not pass sceneTransform
    int const portIndex = _node.checkHitScenePoint(portToCheck,
                                                    event->scenePos(),
                                                    sceneTransform());

    if (portIndex != INVALID)
    {
      std::unordered_map<std::string, GUIConnection*> connections =
        _node.connections(portToCheck, portIndex);

      // start dragging existing connection
      if (!connections.empty() && portToCheck == PortType::In)
      {
        auto con = connections.begin()->second;

        NodeConnectionInteraction interaction(_node, *con, _scene);

        interaction.disconnect(portToCheck);
      }
      else // initialize new Connection
      {
        if (portToCheck == PortType::Out)
        {
          auto const outPolicy = _node.portOutConnectionPolicy(portIndex);
          if (!connections.empty() &&
              outPolicy == ConnectionPolicy::One)
          {
            _scene.deleteConnection( *connections.begin()->second );
          }
        }

        // todo add to FlowScene
        auto connection = _scene.createConnection(portToCheck,
                                                  _node,
                                                  portIndex);

        _node.setConnection(portToCheck,
                                        portIndex,
                                        *connection);

        connection->getConnectionGraphicsObject().grabMouse();
      }
    }
  }

  auto pos     = event->pos();
  if (_node.resizable() &&
      _node.resizeRect().contains(QPoint(pos.x(),
                                        pos.y())))
  {
    _node.setResizing(true);
  }
}


void
NodeGraphicsObject::
mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
  if (_node.resizing())
  {
    auto diff = event->pos() - event->lastPos();

    if (auto w = _node.embeddedWidget())
    {
      prepareGeometryChange();

      auto oldSize = w->size();

      oldSize += QSize(diff.x(), diff.y());

      w->setFixedSize(oldSize);

      _proxyWidget->setMinimumSize(oldSize);
      _proxyWidget->setMaximumSize(oldSize);
      _proxyWidget->setPos(_node.widgetPosition());

      _node.recalculateSize();
      update();

      moveConnections();

      event->accept();
    }
  }
  else
  {
    QGraphicsObject::mouseMoveEvent(event);

    if (event->lastPos() != event->pos())
      moveConnections();

    event->ignore();
  }

  QRectF r = scene()->sceneRect();

  r = r.united(mapToScene(boundingRect()).boundingRect());

  scene()->setSceneRect(r);
}


void
NodeGraphicsObject::
mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  _node.setResizing(false);

  QGraphicsObject::mouseReleaseEvent(event);

  // position connections precisely after fast node move
  moveConnections();

  _scene.nodeClicked(node());
}


void
NodeGraphicsObject::
hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
  // bring all the colliding nodes to background
  QList<QGraphicsItem *> overlapItems = collidingItems();

  for (QGraphicsItem *item : overlapItems)
  {
    if (item->zValue() > 0.0)
    {
      item->setZValue(0.0);
    }
  }

  // bring this node forward
  setZValue(1.0);

  _node.setHovered(true);
  update();
  _scene.nodeHovered(node(), event->screenPos());
  event->accept();
}


void
NodeGraphicsObject::
hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
  _node.setHovered(false);
  update();
  _scene.nodeHoverLeft(node());
  event->accept();
}


void
NodeGraphicsObject::
hoverMoveEvent(QGraphicsSceneHoverEvent * event)
{
  auto pos    = event->pos();

  if (_node.resizable() &&
      _node.resizeRect().contains(QPoint(pos.x(), pos.y())))
  {
    setCursor(QCursor(Qt::SizeFDiagCursor));
  }
  else
  {
    setCursor(QCursor());
  }

  event->accept();
}


void
NodeGraphicsObject::
mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
  QGraphicsItem::mouseDoubleClickEvent(event);

  _scene.nodeDoubleClicked(node());
}


void
NodeGraphicsObject::
contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
  _scene.nodeContextMenu(node(), mapToScene(event->pos()));
}

void NodeGraphicsObject::PositionChanged()
{
  _node.PositionChanged(pos());
}