#pragma once

#include <QtGui/QPainter>

namespace QtNodes
{

class GUINode;
class NodeGraphicsObject;
class FlowItemEntry;
class FlowScene;

class NodePainter
{
public:

  NodePainter();

public:

  static
  void
  paint(QPainter* painter,
        GUINode& node,
        FlowScene const& scene);

  static
  void
  drawNodeRect(QPainter* painter,
               GUINode* model,
               NodeGraphicsObject const & graphicsObject);

  static
  void
  drawModelName(QPainter* painter,
                GUINode * model);

  static
  void
  drawEntryLabels(QPainter* painter,
                  GUINode * model);

  static
  void
  drawConnectionPoints(QPainter* painter,
                       GUINode * model,
                       FlowScene const & scene);

  static
  void
  drawFilledConnectionPoints(QPainter* painter,
                             GUINode * model);

  static
  void
  drawResizeRect(QPainter* painter,
                 GUINode * model);

  static
  void
  drawValidationRect(QPainter * painter,
                     GUINode * model,
                     NodeGraphicsObject const & graphicsObject);
};
}
