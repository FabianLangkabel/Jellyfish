#pragma once

#include <QtGui/QPainter>

namespace QtNodes
{

class ConnectionGeometry;
class ConnectionState;
class GUIConnection;

class ConnectionPainter
{
public:

  static
  void
  paint(QPainter* painter,
        GUIConnection const& connection);

  static
  QPainterPath
  getPainterStroke(ConnectionGeometry const& geom);
};
}
