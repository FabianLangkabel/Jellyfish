#pragma once

#include <QPainter>

namespace QtNodes {

/// Class to allow for custom painting
class NodePainterDelegate
{

public:

  virtual
  ~NodePainterDelegate() = default;

  virtual void
  paint(QPainter* painter,
        GUINode const& geom,
        GUINode const * model) = 0;
};
}
