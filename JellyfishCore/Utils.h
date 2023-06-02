#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <iostream>

class Utils
{
public:
    static std::vector<std::string> split_string_in_strings(std::string string, char delimiter);
};