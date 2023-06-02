#pragma once

#include <map>
#include <string>
#include <memory>
#include <iostream>
#include <vector>
#include <map>

#include "registry.hpp"
#include "../json.hpp"
#include "node.hpp"

using json = nlohmann::json;

class plugin_node_manager
{
public:
    Registry<Node, std::string>* RegisteredNodes;

public:
    plugin_node_manager()
    {
        RegisteredNodes = new Registry<Node, std::string>();
    }

    std::any ConstructNode(std::any NodeTypeID)
    {
        std::string NodeTypeIDString = std::any_cast<std::string>(NodeTypeID);
        std::shared_ptr<Node> ConstuctedNode = RegisteredNodes->construct(NodeTypeIDString, NodeTypeIDString);
        return std::make_any<std::shared_ptr<Node>>(ConstuctedNode);
    }

    std::any GetInfosToAllNodes()
    {
        std::map<std::string, json> InfosToAllNodes;
        std::vector<std::string> AllNodeTypeIDs = RegisteredNodes->GetAllKeys();
        for(int i = 0; i < AllNodeTypeIDs.size(); i++)
        {
            std::shared_ptr<Node> Node = RegisteredNodes->construct(AllNodeTypeIDs[i], AllNodeTypeIDs[i]);
            json node_infos;
            node_infos["NodeName"] = Node->NodeName;
            node_infos["NodeDescription"] = Node->NodeDescription;
            node_infos["NodeCategory"] = Node->NodeCategory;
            InfosToAllNodes[AllNodeTypeIDs[i]] = node_infos;
        }
        return std::make_any<std::map<std::string, json>>(InfosToAllNodes);
    }
};