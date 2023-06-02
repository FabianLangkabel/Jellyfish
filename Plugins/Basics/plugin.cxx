#include "../../JellyfishCore/include/PluginSystem/iplugins.hpp"
#include "../../JellyfishCore/include/NodeSystem/plugin_node_manager.hpp"
#include "../../JellyfishCore/include/json.hpp"

#include "Nodes/ReadMoleculeNode/ReadMoleculeNode.hpp"
#include "Nodes/OneElectronIntegralsNode/OneElectronIntegralsNode.hpp"
#include "Nodes/OneElectronPotentialIntegralsNode/OneElectronPotentialIntegralsNode.hpp"
#include "Nodes/TwoElectronIntegralsNode/TwoElectronIntegralsNode.hpp"
#include "Nodes/CIEnergyCAPNode/CIEnergyCAPNode.hpp"
#include "Nodes/ConfigurationInteractionNode/ConfigurationInteractionNode.hpp"
#include "Nodes/RestrictedHartreeFockNode/RestrictedHartreeFockNode.hpp"
#include "Nodes/CombineOneElectronIntegralsNode/CombineOneElectronIntegralsNode.hpp"
#include "Nodes/CombineBasissetsNode/CombineBasissetsNode.hpp"
#include "Nodes/CreateEvenTemperedBasissetNode/CreateEvenTemperedBasissetNode.hpp"
#include "Nodes/CreateGaussPotentialNode/CreateGaussPotentialNode.hpp"
#include "Nodes/EvaluateNTONode/EvaluateNTONode.hpp"
#include "Nodes/GaussPotentialIntegralsNode/GaussPotentialIntegralsNode.hpp"
#include "Nodes/InputElectronNumberNode/InputElectronNumberNode.hpp"
#include "Nodes/IntegralsTransformationToHFNode/IntegralsTransformationToHFNode.hpp"
#include "Nodes/IntegralsTransformationToSpinNode/IntegralsTransformationToSpinNode.hpp"
#include "Nodes/PointchargeEnergyNode/PointchargeEnergyNode.hpp"
#include "Nodes/PropagatorNode/PropagatorNode.hpp"
#include "Nodes/PulsedLaserNode/PulsedLaserNode.hpp"
#include "Nodes/SolveOneElectronSystemNode/SolveOneElectronSystemNode.hpp"
#include "Nodes/SpartialCAPNode/SpartialCAPNode.hpp"
#include "Nodes/TransitionDipoleMomentsCINode/TransitionDipoleMomentsCINode.hpp"
#include "Nodes/TransitionDipoleMomentsOneElectronNode/TransitionDipoleMomentsOneElectronNode.hpp"

#include <iostream>

using json = nlohmann::json;

// our class plugin
class pluginbasics final : public micro::iplugin<>, public std::enable_shared_from_this<pluginbasics> {
public:

  plugin_node_manager* NodeManager;
  json plugin_infos;

  pluginbasics(int v, const std::string& nm):micro::iplugin<>(v, nm), std::enable_shared_from_this<pluginbasics>() {

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
    NodeManager->RegisteredNodes->registerClass<CIEnergyCAPNode>("CIEnergyCAPNode");
    NodeManager->RegisteredNodes->registerClass<SpartialCAPNode>("SpartialCAPNode");
    NodeManager->RegisteredNodes->registerClass<CreateGaussPotentialNode>("CreateGaussPotentialNode");

    NodeManager->RegisteredNodes->registerClass<CombineBasissetsNode>("CombineBasissetsNode");
    NodeManager->RegisteredNodes->registerClass<CreateEvenTemperedBasissetNode>("CreateEvenTemperedBasissetNode");

    NodeManager->RegisteredNodes->registerClass<OneElectronIntegralsNode>("OneElectronIntegralsNode");
    NodeManager->RegisteredNodes->registerClass<OneElectronPotentialIntegralsNode>("OneElectronPotentialIntegralsNode");
    NodeManager->RegisteredNodes->registerClass<GaussPotentialIntegralsNode>("GaussPotentialIntegralsNode");
    NodeManager->RegisteredNodes->registerClass<CombineOneElectronIntegralsNode>("CombineOneElectronIntegralsNode");
    NodeManager->RegisteredNodes->registerClass<TwoElectronIntegralsNode>("TwoElectronIntegralsNode");
    NodeManager->RegisteredNodes->registerClass<IntegralsTransformationToHFNode>("IntegralsTransformationToHFNode");
    NodeManager->RegisteredNodes->registerClass<IntegralsTransformationToSpinNode>("IntegralsTransformationToSpinNode");

    NodeManager->RegisteredNodes->registerClass<EvaluateNTONode>("EvaluateNTONode");

    NodeManager->RegisteredNodes->registerClass<RestrictedHartreeFockNode>("RestrictedHartreeFockNode");
    NodeManager->RegisteredNodes->registerClass<PointchargeEnergyNode>("PointchargeEnergyNode");
    NodeManager->RegisteredNodes->registerClass<ConfigurationInteractionNode>("ConfigurationInteractionNode");
    NodeManager->RegisteredNodes->registerClass<TransitionDipoleMomentsCINode>("TransitionDipoleMomentsCINode");
    NodeManager->RegisteredNodes->registerClass<SolveOneElectronSystemNode>("SolveOneElectronSystemNode");
    NodeManager->RegisteredNodes->registerClass<TransitionDipoleMomentsOneElectronNode>("TransitionDipoleMomentsOneElectronNode");

    NodeManager->RegisteredNodes->registerClass<ReadMoleculeNode>("ReadMoleculeNode");
    NodeManager->RegisteredNodes->registerClass<InputElectronNumberNode>("InputElectronNumberNode");
    
    NodeManager->RegisteredNodes->registerClass<PropagatorNode>("PropagatorNode");
    NodeManager->RegisteredNodes->registerClass<PulsedLaserNode>("PulsedLaserNode");
    
    
    

    //Register all functions needed for Programm-Plugin communication
    subscribe<1>("ConstructNode", std::bind(&plugin_node_manager::ConstructNode, NodeManager, std::placeholders::_1));
    subscribe<0>("GetInfosToAllNodes", std::bind(&plugin_node_manager::GetInfosToAllNodes, NodeManager));
    
    //Register additional functions for the Core-Programm or dependent nodes etc.
    subscribe<0>("GetPluginInfos", std::bind(&pluginbasics::GetPluginInfos, this));
  }

  ~pluginbasics() override {}

  std::shared_ptr<micro::iplugin<>> get_shared_ptr() override {
    return std::shared_ptr<micro::iplugin<>>(shared_from_this());
  }
  
  //Define all additional functions needed for Programm-Plugin communication
  std::any GetPluginInfos() {
    return std::make_any<json>(plugin_infos);
  }
};


// instance of the plugin
static std::shared_ptr<pluginbasics> instance = nullptr;


// extern function, that declared in "iplugin.hpp", for export the plugin from dll
std::shared_ptr<micro::iplugin<>> import_plugin() {
  int plugin_version = micro::make_version(1,0); //Set the Version of the Plugin
  return instance ? instance : (instance = std::make_shared<pluginbasics>(plugin_version, "Basics"));
}