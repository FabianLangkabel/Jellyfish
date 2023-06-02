#pragma once

#include "memory.hpp"

#include <functional>
#include <any>

namespace QtNodes
{

using SharedNodeData = std::any;

// a function taking in NodeData and returning NodeData
using TypeConverter =
  std::function<SharedNodeData(SharedNodeData)>;

// data-type-in, data-type-out
using TypeConverterId =
  std::pair<std::string, std::string>;

struct TypeConverterIdHash
{
    std::size_t operator()(const QtNodes::TypeConverterId& converter) const noexcept
    {
        //return qHash(converter.first.id)
        //    ^ qHash(converter.second.id);
        return 0;
    }
};

}
