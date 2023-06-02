#include "ConnectionState.hpp"

#include <iostream>

#include <QtCore/QPointF>

#include "FlowScene.hpp"
#include "GUINode.hpp"

using QtNodes::ConnectionState;
using QtNodes::GUINode;

ConnectionState::
~ConnectionState()
{
  resetLastHoveredNode();
}


void
ConnectionState::
interactWithNode(GUINode* node)
{
  if (node)
  {
    _lastHoveredNode = node;
  }
  else
  {
    resetLastHoveredNode();
  }
}


void
ConnectionState::
setLastHoveredNode(GUINode* node)
{
  _lastHoveredNode = node;
}


void
ConnectionState::
resetLastHoveredNode()
{
  if (_lastHoveredNode)
    _lastHoveredNode->resetReactionToConnection();

  _lastHoveredNode = nullptr;
}
