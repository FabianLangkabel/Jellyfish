#include "NodePainter.hpp"

#include <cmath>

#include <QtCore/QMargins>

#include "StyleCollection.hpp"
#include "PortType.hpp"
#include "NodeGraphicsObject.hpp"
#include "GUINode.hpp"
#include "FlowScene.hpp"

using QtNodes::NodePainter;
using QtNodes::NodeGraphicsObject;
using QtNodes::GUINode;
using QtNodes::FlowScene;

void
NodePainter::
paint(QPainter* painter,
      GUINode & node,
      FlowScene const& scene)
{
  NodeGraphicsObject const & graphicsObject = node.nodeGraphicsObject();

  node.recalculateSize(painter->font());

  //--------------------------------------------
  //GUINode const * model = node.nodeDataModel();

  drawNodeRect(painter, &node, graphicsObject);

  drawConnectionPoints(painter, &node, scene);

  drawFilledConnectionPoints(painter, &node);

  drawModelName(painter, &node);

  drawEntryLabels(painter, &node);

  drawResizeRect(painter, &node);

  drawValidationRect(painter, &node, graphicsObject);

  /// call custom painter
  /*
  if (auto painterDelegate = node.painterDelegate())
  {
    painterDelegate->paint(painter, &node);
  }
  */
}


void
NodePainter::
drawNodeRect(QPainter* painter,
             GUINode* model,
             NodeGraphicsObject const & graphicsObject)
{
  NodeStyle const& nodeStyle = model->nodeStyle();

  auto color = graphicsObject.isSelected()
               ? nodeStyle.SelectedBoundaryColor
               : nodeStyle.NormalBoundaryColor;

  if (model->hovered())
  {
    QPen p(color, nodeStyle.HoveredPenWidth);
    painter->setPen(p);
  }
  else
  {
    QPen p(color, nodeStyle.PenWidth);
    painter->setPen(p);
  }

  QLinearGradient gradient(QPointF(0.0, 0.0),
                           QPointF(2.0, model->height()));

  gradient.setColorAt(0.0, nodeStyle.GradientColor0);
  gradient.setColorAt(0.03, nodeStyle.GradientColor1);
  gradient.setColorAt(0.97, nodeStyle.GradientColor2);
  gradient.setColorAt(1.0, nodeStyle.GradientColor3);

  painter->setBrush(gradient);

  float diam = nodeStyle.ConnectionPointDiameter;

  QRectF boundary( -diam, -diam, 2.0 * diam + model->width(), 2.0 * diam + model->height());

  double const radius = 3.0;

  painter->drawRoundedRect(boundary, radius, radius);
}


void
NodePainter::
drawConnectionPoints(QPainter* painter,
                     GUINode* model,
                     FlowScene const & scene)
{
  NodeStyle const& nodeStyle      = model->nodeStyle();
  auto const     &connectionStyle = StyleCollection::connectionStyle();

  float diameter = nodeStyle.ConnectionPointDiameter;
  auto  reducedDiameter = diameter * 0.6;

  for(PortType portType: {PortType::Out, PortType::In})
  {
    size_t n = model->getEntries(portType).size();

    for (unsigned int i = 0; i < n; ++i)
    {
      QPointF p = model->portScenePosition(i, portType);

      auto const & dataType = 
        model->dataType(portType, static_cast<int>(i));

      bool canConnect = (model->getEntries(portType)[i].empty() ||
                         (portType == PortType::Out &&
                          model->portOutConnectionPolicy(i) == ConnectionPolicy::Many) );

      double r = 1.0;
      if (model->isReacting() &&
          canConnect &&
          portType == model->reactingPortType())
      {

        auto   diff = model->draggingPos() - p;
        double dist = std::sqrt(QPointF::dotProduct(diff, diff));
        bool   typeConvertable = false;

        {
          if (portType == PortType::In)
          {
            //typeConvertable = scene.registry().getTypeConverter(state.reactingDataType(), dataType) != nullptr;
          }
          else
          {
            //typeConvertable = scene.registry().getTypeConverter(dataType, state.reactingDataType()) != nullptr;
          }
        }

        if (model->reactingDataType() == dataType || typeConvertable)
        {
          double const thres = 40.0;
          r = (dist < thres) ?
                (2.0 - dist / thres ) :
                1.0;
        }
        else
        {
          double const thres = 80.0;
          r = (dist < thres) ?
                (dist / thres) :
                1.0;
        }
      }

      if (connectionStyle.useDataDefinedColors())
      {
        painter->setBrush(connectionStyle.normalColor(dataType));
      }
      else
      {
        painter->setBrush(nodeStyle.ConnectionPointColor);
      }

      painter->drawEllipse(p,
                           reducedDiameter * r,
                           reducedDiameter * r);
    }
  };
}


void
NodePainter::
drawFilledConnectionPoints(QPainter * painter,
                           GUINode* model)
{
  NodeStyle const& nodeStyle       = model->nodeStyle();
  auto const     & connectionStyle = StyleCollection::connectionStyle();

  auto diameter = nodeStyle.ConnectionPointDiameter;

  for(PortType portType: {PortType::Out, PortType::In})
  {
    size_t n = model->getEntries(portType).size();

    for (size_t i = 0; i < n; ++i)
    {
      QPointF p = model->portScenePosition(
          static_cast<PortIndex>(i),
          static_cast<PortType>(portType));

      if (!model->getEntries(portType)[i].empty())
      {
        auto const & dataType =
          model->dataType(portType, static_cast<int>(i));

        if (connectionStyle.useDataDefinedColors())
        {
          QColor const c = connectionStyle.normalColor(dataType);
          painter->setPen(c);
          painter->setBrush(c);
        }
        else
        {
          painter->setPen(nodeStyle.FilledConnectionPointColor);
          painter->setBrush(nodeStyle.FilledConnectionPointColor);
        }

        painter->drawEllipse(p,
                             diameter * 0.4,
                             diameter * 0.4);
      }
    }
  }
}


void
NodePainter::
drawModelName(QPainter * painter,
              GUINode* model)
{
  NodeStyle const& nodeStyle = model->nodeStyle();

 // Q_UNUSED(state);

  if (!model->captionVisible())
    return;

  QString const &name = model->caption();

  QFont f = painter->font();

  f.setBold(true);

  QFontMetrics metrics(f);

  auto rect = metrics.boundingRect(name);

  QPointF position((model->width() - rect.width()) / 2.0,
                   (model->spacing() + model->entryHeight()) / 3.0);

  painter->setFont(f);
  painter->setPen(nodeStyle.FontColor);
  painter->drawText(position, name);

  f.setBold(false);
  painter->setFont(f);
}


void
NodePainter::
drawEntryLabels(QPainter * painter,
                GUINode* model)
{
  QFontMetrics const & metrics =
    painter->fontMetrics();

  for(PortType portType: {PortType::Out, PortType::In})
  {
    auto const &nodeStyle = model->nodeStyle();

    auto& entries = model->getEntries(portType);

    size_t n = entries.size();

    for (size_t i = 0; i < n; ++i)
    {
      QPointF p = model->portScenePosition(static_cast<PortIndex>(i), portType);

      if (entries[i].empty())
        painter->setPen(nodeStyle.FontColorFaded);
      else
        painter->setPen(nodeStyle.FontColor);

      QString s;

      if (model->portCaptionVisible(portType, static_cast<PortIndex>(i)))
      {
        s = model->portCaption(portType, static_cast<PortIndex>(i));
      }
      else
      {
        s = QString::fromStdString(model->dataType(portType, static_cast<int>(i)));
      }

      auto rect = metrics.boundingRect(s);

      p.setY(p.y() + rect.height() / 4.0);

      switch (portType)
      {
      case PortType::In:
        p.setX(5.0);
        break;

      case PortType::Out:
        p.setX(model->width() - 5.0 - rect.width());
        break;

      default:
        break;
      }

      painter->drawText(p, s);
    }
  }
}


void
NodePainter::
drawResizeRect(QPainter * painter,
               GUINode* model)
{
  if (model->resizable())
  {
    painter->setBrush(Qt::gray);

    painter->drawEllipse(model->resizeRect());
  }
}


void
NodePainter::
drawValidationRect(QPainter * painter,
                   GUINode* model,
                   NodeGraphicsObject const & graphicsObject)
{
  auto modelValidationState = model->validationState();

  if (modelValidationState != NodeValidationState::Valid)
  {
    NodeStyle const& nodeStyle = model->nodeStyle();

    auto color = graphicsObject.isSelected()
                 ? nodeStyle.SelectedBoundaryColor
                 : nodeStyle.NormalBoundaryColor;

    if (model->hovered())
    {
      QPen p(color, nodeStyle.HoveredPenWidth);
      painter->setPen(p);
    }
    else
    {
      QPen p(color, nodeStyle.PenWidth);
      painter->setPen(p);
    }

    //Drawing the validation message background
    if (modelValidationState == NodeValidationState::Error)
    {
      painter->setBrush(nodeStyle.ErrorColor);
    }
    else
    {
      painter->setBrush(nodeStyle.WarningColor);
    }

    double const radius = 3.0;

    float diam = nodeStyle.ConnectionPointDiameter;

    QRectF boundary(-diam,
                    -diam + model->height() - model->validationHeight(),
                    2.0 * diam + model->width(),
                    2.0 * diam + model->validationHeight());

    painter->drawRoundedRect(boundary, radius, radius);

    painter->setBrush(Qt::gray);

    //Drawing the validation message itself
    QString const &errorMsg = model->validationMessage();

    QFont f = painter->font();

    QFontMetrics metrics(f);

    auto rect = metrics.boundingRect(errorMsg);

    QPointF position((model->width() - rect.width()) / 2.0,
                     model->height() - (model->validationHeight() - diam) / 2.0);

    painter->setFont(f);
    painter->setPen(nodeStyle.FontColor);
    painter->drawText(position, errorMsg);
  }
}
