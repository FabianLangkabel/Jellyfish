#pragma once

#include <memory>
#include <iostream>
#include <vector>
#include <string>
#include <tuple>
#include <map>

#include "NodeSystem/node.hpp"
#include "json.hpp"

using json = nlohmann::json;

class CoreSystem
{
public:
    CoreSystem();
    ~CoreSystem();
//    void LoadPlugins();
//    std::shared_ptr<Node> CreateNode(int Plugin, std::string NodeType);
//    std::map<std::string, std::map<std::string, json>>* getNodeTypeList();
};