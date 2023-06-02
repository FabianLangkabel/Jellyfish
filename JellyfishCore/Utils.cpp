#include "Utils.h"

std::vector<std::string> Utils::split_string_in_strings(std::string string, char delimiter)
{
    std::vector<std::string> return_vec;
    std::istringstream iss(string);
    std::string s;
    while (std::getline(iss, s, delimiter)) {
        return_vec.push_back(s);
    }
    return return_vec;
}