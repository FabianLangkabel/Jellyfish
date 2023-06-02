#include "Core.hpp"
#include "Project.h"
#include <zip.h>
#include <zipconf.h>


CoreSystem::CoreSystem()
{
    std::cout << "[JellyfishCore] Create CoreSystem" << std::endl;
    PluginManager = micro::plugins<>::get();
}
CoreSystem::~CoreSystem()
{

}

void CoreSystem::LoadPlugins()
{
    PluginIntToName.clear();
    PluginManager->load_all_plugins();
    all_plugins = PluginManager->get_all_plugins();
    for(auto i = 0; i < all_plugins.size(); i++)
    {
        PluginIntToName[std::get<1>(all_plugins[i])->name()] = i;
        //Check for all plugins if prerequisites are met and unload them if not
    }
    UpdateNodeList();
}

std::shared_ptr<Node> CoreSystem::CreateNode(std::string Plugin, std::string NodeType)
{
    std::any r1 = std::get<1>(all_plugins[PluginIntToName[Plugin]])->run<1>("ConstructNode", NodeType);
    return std::any_cast<std::shared_ptr<Node>>(r1);
}

std::map<std::string, std::map<std::string, json>>* CoreSystem::getNodeTypeList()
{
    return &NodeTypeList;
}

std::shared_ptr<Project> CoreSystem::createProject()
{
    NextProjectNumber++;
    std::shared_ptr<Project> Proj = std::make_shared<Project>(NextProjectNumber, this, "New Project");
    OpenProjects[NextProjectNumber] = Proj;
    return Proj;
}



std::shared_ptr<Project> CoreSystem::loadProject(std::string ProjectFile, bool PrintConsoleOutput)
{
    NextProjectNumber++;
    std::shared_ptr<Project> Proj = std::make_shared<Project>(NextProjectNumber, this, "New Project", ProjectFile, PrintConsoleOutput);
    OpenProjects[NextProjectNumber] = Proj;
    return Proj;
}

bool CoreSystem::Check_if_project_can_be_loaded(std::string ProjectFile)
{
    int error(0);
    zip* zipfile = zip_open(ProjectFile.c_str(), 0, &error);
    if (error != 0)
    {
        std::cout << "File could not be read or is not available" << std::endl;
        return false;
    }

    return true;
}

void CoreSystem::saveProject(int ProjectNumber, std::string file, bool PrintConsoleOutput)
{
    OpenProjects[ProjectNumber]->save(file, PrintConsoleOutput);
}

void CoreSystem::closeProject(int ProjectNumber)
{
    OpenProjects.erase(ProjectNumber);
}

void CoreSystem::UpdateNodeList()
{
    NodeTypeList.clear();
    for(auto i = 0; i < all_plugins.size(); i++)
    {
        std::any r1 = std::get<1>(all_plugins[i])->run<0>("GetPluginInfos");
        json plugininfos = std::any_cast<json>(r1);
        if(plugininfos["has_plugin_node_manager"])
        {
            r1 = std::get<1>(all_plugins[i])->run<0>("GetInfosToAllNodes");
            std::map<std::string, json> NodeInfos = std::any_cast<std::map<std::string, json>>(r1);

            NodeTypeList[std::get<1>(all_plugins[i])->name()] = NodeInfos;
            /*
            for (auto const& [key, val] : NodeInfos)
            {
                std::cout << val["NodeName"] << std::endl;
            }
            */
        }

        /*
        std::shared_future<std::any> r2;
        r2 = std::get<1>(all_plugins[i])->run<1>("ConstructNode", std::string("Testnode"));
        r2.wait();
        std::shared_ptr<Node> test = std::any_cast<std::shared_ptr<Node>>(r2.get());
        test->calculate();
        */
    }
}