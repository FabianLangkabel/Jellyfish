#pragma once

#include <string>
#include <string_view>
#include <map>
#include <ctime>
#include "include/json.hpp"
#include "include/uuid.h"
#include "include/NodeSystem/node.hpp"
#include "include/Connection.h"
#include "Utils.h"

using json = nlohmann::json;

class CoreSystem;

class Project
{
public:
    Project(int Projectnumber, CoreSystem* Core, std::string Projectname);
    Project(int Projectnumber, CoreSystem* Core, std::string Projectname, std::string Projectfile, bool PrintConsoleOutput);
    ~Project(){}

private:
    int _ProjectNumber;
    CoreSystem* _Core;
    std::string _ProjectName;
    std::unordered_map<std::string, std::shared_ptr<Node>> all_nodes;
    std::unordered_map<std::string, std::shared_ptr<Connection>> all_connections;
    std::vector<std::string> _Log;

public:
    int get_ProjectNumber() {return _ProjectNumber;}
    std::string get_ProjectName() {return _ProjectName;}
    std::unordered_map<std::string, std::shared_ptr<Node>> get_all_nodes() {return all_nodes;}
    std::unordered_map<std::string, std::shared_ptr<Connection>> get_all_connections() {return all_connections;}
    std::vector<std::string> get_Log() {return _Log;}
    void addToLog(std::string);

    void add_node(std::shared_ptr<Node> node);
    void remove_node(std::string nodeID);
    void add_connection(std::shared_ptr<Connection> connection);
    void load_connection(std::shared_ptr<Connection> connection);
    void remove_connection(std::string connectionID);
    void save(std::string file, bool PrintConsoleOutput);
    void load(std::string file, bool PrintConsoleOutput);

    void compute_node(std::string nodeID, bool PrintConsoleOutput);
    void compute_node_and_dependence(std::string nodeID, bool PrintConsoleOutput);
};