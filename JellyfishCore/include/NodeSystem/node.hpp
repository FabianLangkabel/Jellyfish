#pragma once

#include <QtWidgets>

#include <string>
#include <iostream>
#include <any>
#include <vector>
#include <map>
#include "../json.hpp"
#include "../uuid.h"
#include "../Connection.h"

using json = nlohmann::json;

enum NodeState
{
    Calculated,
    notCalculated
};

enum NodeStatus
{
  Valid,
  Warning,
  Error
};

enum NodePortType
{
    In,
    Out
};

class Node
{
public:
    Node()
    {
        std::random_device rd;
        auto seed_data = std::array<int, std::mt19937::state_size> {};
        std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
        std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
        std::mt19937 generator(seq);
        uuids::uuid_random_generator gen{generator};
        id = uuids::to_string(gen());
    };
    std::string NodeTypeID;
    std::string NodeName;
    std::string NodeDescription;
    std::string NodeCategory;
    std::string Plugin;
    bool HasCalulateFunction;

    std::string id;

    int x_position_on_grid;
    int y_position_on_grid;
    NodeState State = NodeState::notCalculated;

    std::unordered_map<std::string, std::shared_ptr<Connection>> in_connections;
    std::unordered_map<std::string, std::shared_ptr<Connection>> out_connections;

    virtual void calculate(){};
    virtual json SaveData(std::string file){};
    virtual void LoadData(json Data, std::string file){};
    virtual NodeStatus GetNodeStatus()
    {
        if(HasCalulateFunction && State == NodeState::notCalculated)
        { return NodeStatus::Warning; }
        else
        { return NodeStatus::Valid; }
    };

    virtual std::string GetNodeStatusText()
    {
        if(HasCalulateFunction && State == NodeState::notCalculated)
        { return "Node not calculated"; }
        else
        { return ""; }
    }

    virtual void NodeInspector(QWidget*){};

    virtual bool InPortAllowMultipleConnections(int Port){ return false; }
    virtual unsigned int Ports(NodePortType PortType) { return 0; }
    virtual bool IsCustomPortCaption(NodePortType PortType, int Port){ return false; }
    virtual std::string CustomPortCaption(NodePortType PortType, int Port){ return ""; }
    virtual std::string DataTypeName(NodePortType PortType, int Port) {return "";}
    virtual std::any getOutData(int Port) { return std::make_any<int>(0); }
    virtual void setInData(int Port, std::any data){};
    virtual void PropertiesChanged() //Called when properties or input connections are changed
    {
        if(HasCalulateFunction && State == NodeState::Calculated)
        {
            State = NodeState::notCalculated;
        }
        for (auto const& [key, val] : out_connections)
        {
            val->_InNode->PropertiesChanged();
        }
    }

    void add_connection(bool IsInConnection, std::shared_ptr<Connection> connection)
    {
        if(IsInConnection){in_connections[connection->_Id] = connection;}
        else{
            out_connections[connection->_Id] = connection;
            connection->_InNode->PropertiesChanged();
        }
    }
    void remove_connection(bool IsInConnection, std::string connectionID)
    {
        if(IsInConnection){in_connections.erase(connectionID);}
        else{
            out_connections[connectionID]->_InNode->PropertiesChanged();
            out_connections.erase(connectionID);
        }
    }
    void load_connection(bool IsInConnection, std::shared_ptr<Connection> connection) //like add_connection but does not reset the NodeState of dependent nodes
    {
        if(IsInConnection){in_connections[connection->_Id] = connection;}
        else{out_connections[connection->_Id] = connection;}
    }
    void update_PortData(int Port)
    {
        for (auto const& [key, val] : out_connections)
        {
            if(val->_OutPort == Port)
            {
                val->_InNode->setInData(val->_InPort, getOutData(Port));
            }
        }
    }
    void update_AllPortData()
    {
        for (auto const& [key, val] : out_connections)
        {
            val->_InNode->setInData(val->_InPort, getOutData(val->_OutPort));
        }
    }
};