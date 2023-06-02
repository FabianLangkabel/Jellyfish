#pragma once

#include <string>
#include "../json.hpp"

using json = nlohmann::json;

enum NodeState
{
    Calculated,
    notCalculated
};

class Node
{
public:
    std::string NodeTypeID;
    std::string NodeName;
    std::string NodeDescription;

    int id;
    double x_position_on_grid;
    double y_position_on_grid;
    NodeState State;

    virtual void calculate(){};
    virtual json SaveData(){};
    virtual void LoadData(json Data){};
};