#pragma once

#include <string>
#include "json.hpp"
#include "uuid.h"

class Node;
using json = nlohmann::json;

class Connection
{
public:
    Connection(std::shared_ptr<Node> OutNode, int OutPort, std::shared_ptr<Node> InNode, int InPort)
    {
        _OutNode = OutNode;
        _OutPort = OutPort;
        _InNode = InNode;
        _InPort = InPort;

        std::random_device rd;
        auto seed_data = std::array<int, std::mt19937::state_size> {};
        std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
        std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
        std::mt19937 generator(seq);
        uuids::uuid_random_generator gen{generator};
        _Id = uuids::to_string(gen());
    }

    Connection(std::shared_ptr<Node> OutNode, int OutPort, std::shared_ptr<Node> InNode, int InPort, std::string Id)
    {
        _OutNode = OutNode;
        _OutPort = OutPort;
        _InNode = InNode;
        _InPort = InPort;
        _Id = Id;
    }

public:
    std::shared_ptr<Node> _OutNode;
    int _OutPort;
    std::shared_ptr<Node> _InNode;
    int _InPort;
    std::string _Id;
};