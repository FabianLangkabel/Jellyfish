#include "CombineOneElectronIntegralsNode.hpp"

CombineOneElectronIntegralsNode::CombineOneElectronIntegralsNode(std::string InpNodeTypeID) : Node()
{   
    NodeTypeID = InpNodeTypeID;
    NodeName = "Combine One-Electron Integrals";
    NodeDescription = "Combine One-Electron Integrals";
    NodeCategory = "Integrals";
    Plugin = "Basics";
    HasCalulateFunction = true;
}

CombineOneElectronIntegralsNode::~CombineOneElectronIntegralsNode(){}

void CombineOneElectronIntegralsNode::LoadData(json Data, std::string file)
{
    if (State == NodeState::Calculated)
    {
        save_load::load(file, this->id, "integrals", &combined_integrals);

        _oneelectronintegralsout = std::make_shared<Eigen::MatrixXd>(this->combined_integrals);
    }
}

json CombineOneElectronIntegralsNode::SaveData(std::string file)
{
    if (State == NodeState::Calculated)
    {
        save_load::save(file, this->id, "integrals", combined_integrals);
    }

    json DataJson;
    return DataJson;
}

void CombineOneElectronIntegralsNode::calculate()
{
    auto Integral1 = _oneelectronintegralsin1.lock().get();
    auto Integral2 = _oneelectronintegralsin2.lock().get();

    this->combined_integrals = *Integral1 + *Integral2;
    _oneelectronintegralsout = std::make_shared<Eigen::MatrixXd>(this->combined_integrals);
    update_AllPortData();

    State = NodeState::Calculated;
}

void CombineOneElectronIntegralsNode::NodeInspector(QWidget* Inspector)
{
    QVBoxLayout* lay = new QVBoxLayout();

    Inspector->setLayout(lay);
}

bool CombineOneElectronIntegralsNode::InPortAllowMultipleConnections(int Port)
{ 
    return false; 
}

unsigned int CombineOneElectronIntegralsNode::Ports(NodePortType PortType)
{
    if(PortType == NodePortType::In)
    {
        return 2;
    }
    else
    {
        return 1;
    }
}

bool CombineOneElectronIntegralsNode::IsCustomPortCaption(NodePortType PortType, int Port)
{ 
    return true; 
}

std::string CombineOneElectronIntegralsNode::CustomPortCaption(NodePortType PortType, int Port)
{ 
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "Integrals 1"; }
        else if(Port == 1) {return "Integrals 2"; }
    }
    else
    {
        if(Port == 0) {return "Integrals"; }
    }
}

std::string CombineOneElectronIntegralsNode::DataTypeName(NodePortType PortType, int Port)
{
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "Eigen::MatrixXd"; }
        else if(Port == 1) {return "Eigen::MatrixXd"; }
    }
    else
    {
        if(Port == 0) {return "Eigen::MatrixXd"; }
    }
}

std::any CombineOneElectronIntegralsNode::getOutData(int Port) 
{ 
    std::any ret;
    if(Port == 0)
    {
        ret = std::make_any<std::weak_ptr<Eigen::MatrixXd>>(_oneelectronintegralsout);
    }
    return ret;
}

void CombineOneElectronIntegralsNode::setInData(int Port, std::any data)
{
    if(data.has_value())
    {
        if(Port == 0)
        {
            _oneelectronintegralsin1 = std::any_cast<std::weak_ptr<Eigen::MatrixXd>>(data);
        }
        if(Port == 1)
        {
            _oneelectronintegralsin2 = std::any_cast<std::weak_ptr<Eigen::MatrixXd>>(data);
        }
    }
    else
    {
        if(Port == 0){_oneelectronintegralsin1.reset();}
        else if(Port == 1){_oneelectronintegralsin2.reset();}
    }
};