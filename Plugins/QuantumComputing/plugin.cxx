#include "../../JellyfishCore/include/PluginSystem/iplugins.hpp"
#include "../../JellyfishCore/include/NodeSystem/plugin_node_manager.hpp"
#include "../../JellyfishCore/include/json.hpp"

#include "Nodes/CreateStateFromCINode/CreateStateFromCINode.hpp"
#include "Nodes/JWEnergyCAPNode/JWEnergyCAPNode.hpp"
#include "Nodes/JWLaserNode/JWLaserNode.hpp"
#include "Nodes/JWTransformationNode/JWTransformationNode.hpp"
#include "Nodes/QPENode/QPENode.hpp"
#include "Nodes/QPEQuESTNode/QPEQuESTNode.hpp"
#include "Nodes/SimulatorQuESTNode/SimulatorQuESTNode.hpp"
#include "Nodes/TDExpHadamardTestQuESTNode/TDExpHadamardTestQuESTNode.hpp"
#include "Nodes/TDStatevectorToTDCIWFNode/TDStatevectorToTDCIWFNode.hpp"
#include "Nodes/TrotterPropagationNode/TrotterPropagationNode.hpp"
#include "Nodes/TrotterPropagationQuESTNode/TrotterPropagationQuESTNode.hpp"

#include <iostream>

using json = nlohmann::json;

// our class plugin
class pluginQuantumComputing final : public micro::iplugin<>, public std::enable_shared_from_this<pluginQuantumComputing> {
public:

  plugin_node_manager* NodeManager;
  json plugin_infos;

  pluginQuantumComputing(int v, const std::string& nm):micro::iplugin<>(v, nm), std::enable_shared_from_this<pluginQuantumComputing>() {

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
  
    NodeManager->RegisteredNodes->registerClass<CreateStateFromCINode>("CreateStateFromCINode");
    NodeManager->RegisteredNodes->registerClass<JWTransformationNode>("JWTransformationNode");
    NodeManager->RegisteredNodes->registerClass<JWEnergyCAPNode>("JWEnergyCAPNode");
    NodeManager->RegisteredNodes->registerClass<JWLaserNode>("JWLaserNode");
    NodeManager->RegisteredNodes->registerClass<QPENode>("QPENode");
    NodeManager->RegisteredNodes->registerClass<QPEQuESTNode>("QPEQuESTNode");
    NodeManager->RegisteredNodes->registerClass<SimulatorQuESTNode>("SimulatorQuESTNode");
    NodeManager->RegisteredNodes->registerClass<TDExpHadamardTestQuESTNode>("TDExpHadamardTestQuESTNode");
    NodeManager->RegisteredNodes->registerClass<TDStatevectorToTDCIWFNode>("TDStatevectorToTDCIWFNode");
    NodeManager->RegisteredNodes->registerClass<TrotterPropagationNode>("TrotterPropagationNode");
    NodeManager->RegisteredNodes->registerClass<TrotterPropagationQuESTNode>("TrotterPropagationQuESTNode");


    //Register all functions needed for Programm-Plugin communication
    subscribe<1>("ConstructNode", std::bind(&plugin_node_manager::ConstructNode, NodeManager, std::placeholders::_1));
    subscribe<0>("GetInfosToAllNodes", std::bind(&plugin_node_manager::GetInfosToAllNodes, NodeManager));
    
    //Register additional functions for the Core-Programm or dependent nodes etc.
    subscribe<0>("GetPluginInfos", std::bind(&pluginQuantumComputing::GetPluginInfos, this));
  }

  ~pluginQuantumComputing() override {}

  std::shared_ptr<micro::iplugin<>> get_shared_ptr() override {
    return std::shared_ptr<micro::iplugin<>>(shared_from_this());
  }
  
  //Define all additional functions needed for Programm-Plugin communication
  std::any GetPluginInfos() {
    return std::make_any<json>(plugin_infos);
  }
};


// instance of the plugin
static std::shared_ptr<pluginQuantumComputing> instance = nullptr;


// extern function, that declared in "iplugin.hpp", for export the plugin from dll
std::shared_ptr<micro::iplugin<>> import_plugin() {
  int plugin_version = micro::make_version(1,0); //Set the Version of the Plugin
  return instance ? instance : (instance = std::make_shared<pluginQuantumComputing>(plugin_version, "QuantumComputing"));
}