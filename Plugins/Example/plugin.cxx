#include "../../JellyfishCore/include/PluginSystem/iplugins.hpp"
#include "../../JellyfishCore/include/NodeSystem/plugin_node_manager.hpp"
#include "../../JellyfishCore/include/json.hpp"
#include "testnode.hpp"
#include "testnode2.hpp"

#include <iostream>

using json = nlohmann::json;

class exampleplugin final : public micro::iplugin<>, public std::enable_shared_from_this<exampleplugin> {
public:

  plugin_node_manager* NodeManager;
  json plugin_infos;

  exampleplugin(int v, const std::string& nm):micro::iplugin<>(v, nm), std::enable_shared_from_this<exampleplugin>() {

    //Set all Plugin-Infos
    plugin_infos["major_version"] = micro::get_major(v);
    plugin_infos["minor_version"] = micro::get_minor(v);
    plugin_infos["name"] = nm;
    plugin_infos["min_core_major_version"] = 1;
    plugin_infos["min_core_minor_version"] = 0;
    plugin_infos["description"] = "";
    plugin_infos["dependencies"] = {}; // {{plugin, min_major_version, min_minor_version}, {plugin2, min_major_version, min_minor_version}...}
    plugin_infos["has_plugin_node_manager"] = true;

    //Create a Nodemanager for Nodes in this Plugin
    NodeManager = new plugin_node_manager();

    //Register all Nodes in Nodemanager
    NodeManager->RegisteredNodes->registerClass<Testnode>("Testnode");
    NodeManager->RegisteredNodes->registerClass<Testnode2>("Testnode2");

    //Register all functions needed for Programm-Plugin communication
    subscribe<1>("ConstructNode", std::bind(&plugin_node_manager::ConstructNode, NodeManager, std::placeholders::_1));
    subscribe<0>("GetInfosToAllNodes", std::bind(&plugin_node_manager::GetInfosToAllNodes, NodeManager));
    
    //Register additional functions for the Core-Programm or dependent nodes etc.
    subscribe<0>("GetPluginInfos", std::bind(&exampleplugin::GetPluginInfos, this));
  }

  ~exampleplugin() override {}

  std::shared_ptr<micro::iplugin<>> get_shared_ptr() override {
    return std::shared_ptr<micro::iplugin<>>(shared_from_this());
  }
  
  //Define all additional functions needed for Programm-Plugin communication
  std::any GetPluginInfos() {
    return std::make_any<json>(plugin_infos);
  }
};


// instance of the plugin
static std::shared_ptr<exampleplugin> instance = nullptr;


// extern function, that declared in "iplugin.hpp", for export the plugin from dll
std::shared_ptr<micro::iplugin<>> import_plugin() {
  int plugin_version = micro::make_version(1,0); //Set the Version of the Plugin
  return instance ? instance : (instance = std::make_shared<exampleplugin>(plugin_version, "Example"));
}