#pragma once

#include "include/PluginSystem/plugins.hpp"
#include "include/NodeSystem/node.hpp"
#include "include/Connection.h"
#include "include/json.hpp"
#include <memory>
#include <iostream>
#include <vector>
#include <string>
#include <tuple>
#include <map>

using json = nlohmann::json;

class Project;

class CoreSystem
{
private: 
    std::shared_ptr<micro::plugins<>> PluginManager;
    std::vector<std::tuple<std::string,std::shared_ptr<micro::iplugin<>>>> all_plugins;
    std::map<std::string, int> PluginIntToName;
    std::map<std::string, std::map<std::string, json>> NodeTypeList;

private:
    std::unordered_map<int, std::shared_ptr<Project>> OpenProjects;
    int NextProjectNumber = 0;

public:
    CoreSystem();
    ~CoreSystem();

    void LoadPlugins();
    std::shared_ptr<Node> CreateNode(std::string Plugin, std::string NodeType);
    std::map<std::string, std::map<std::string, json>>* getNodeTypeList();
    std::shared_ptr<Project> createProject();
    std::shared_ptr<Project> loadProject(std::string ProjectFile, bool PrintConsoleOutput);
    bool Check_if_project_can_be_loaded(std::string ProjectFile);
    void saveProject(int ProjectNumber, std::string file, bool PrintConsoleOutput);
    void closeProject(int ProjectNumber);

private:
    void UpdateNodeList();
};