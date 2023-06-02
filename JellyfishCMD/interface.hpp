#include <string>
#include <vector>

#include <iostream>
#include <sstream>
#include <ostream>
#include <istream>
#include <fstream>

#include "../JellyfishCore/Core.hpp"
#include "../JellyfishCore/Project.h"

class Interface{
public:
    Interface();
    ~Interface();

public:
    void MainLoop();

private:
    std::shared_ptr<CoreSystem> Core;
    std::shared_ptr<Project> OpenProject;
    int OpenProjectID = -1;

    void commands(std::string input);
    std::vector<std::string> split_string_in_strings(std::string string);
    bool file_exists(const std::string& name);
};