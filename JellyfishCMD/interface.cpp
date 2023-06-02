#include "interface.hpp"

Interface::Interface()
{
    std::cout << "JellyfishCMD v1 started" << std::endl << std::endl;
    Core = std::make_shared<CoreSystem>();
    Core->LoadPlugins();

    std::cout << std::endl << "Command list:" << std::endl;
	std::cout << "help" << std::endl;
	std::cout << "load [*.jellyfish]" << std::endl;
	std::cout << "save [*.jellyfish]" << std::endl;
	std::cout << "unload" << std::endl;
	std::cout << "calculate [node id] (-r for redundant)" << std::endl;
	std::cout << "exit" << std::endl << std::endl;

    MainLoop();
}

Interface::~Interface()
{

}

void Interface::MainLoop()
{
    bool run_programm = true;
	while (run_programm)
	{
		std::cout << "|JellyfishCMD> ";
		std::string input_value;
		std::getline(std::cin, input_value);
		if (input_value == "exit")
		{
			break;
		}
		commands(input_value);
	}
}

void Interface::commands(std::string input)
{
	std::vector<std::string> input_splitted = split_string_in_strings(input);

	if (input_splitted.size() == 0)
	{
		return;
	}

	if (input_splitted[0] == "help") {
		std::cout << "Command list:" << std::endl;
		std::cout << "help" << std::endl;
		std::cout << "load [*.jellyfish]" << std::endl;
		std::cout << "save [*.jellyfish]" << std::endl;
		std::cout << "unload" << std::endl;
		std::cout << "calculate [node id] (-r for redundant)" << std::endl;
		std::cout << "exit" << std::endl;
	}
	else if (input_splitted[0] == "load" && input_splitted.size() > 1) {
        if(Core->Check_if_project_can_be_loaded(input_splitted[1]))
        {
            if(OpenProjectID == -1)
            {
                OpenProject = Core->loadProject(input_splitted[1], true);
				OpenProjectID = OpenProject->get_ProjectNumber();
            }
            else
            {
				std::cout << "There is already a project open. Unload it before loading a new one." << std::endl;
            }
        }
	}
	else if (input_splitted[0] == "unload") {
		std::cout << "Project: " << OpenProject->get_ProjectName() << " unloaded" << std::endl;
		OpenProject.reset();
		OpenProjectID = -1;
	}
	else if (input_splitted[0] == "save" && input_splitted.size() > 1) {
		OpenProject->save(input_splitted[1], true);
	}
	else if (input_splitted[0] == "calculate" && input_splitted.size() > 1) {
		if (input_splitted.size() > 2 && input_splitted[2] == "-r") { OpenProject->compute_node_and_dependence(input_splitted[1], true); }
		else { OpenProject->compute_node(input_splitted[1], true); }
	}
	else {
		std::cout << "Command unknown" << std::endl;
	}
}

std::vector<std::string> Interface::split_string_in_strings(std::string string)
{
	std::vector<std::string> return_vec;
	std::istringstream iss(string);
	std::string s;
	while (std::getline(iss, s, ' ')) {
		return_vec.push_back(s);
	}
	return return_vec;
}

bool Interface::file_exists(const std::string& name) {
    std::ifstream f(name.c_str());
    return f.good();
}
