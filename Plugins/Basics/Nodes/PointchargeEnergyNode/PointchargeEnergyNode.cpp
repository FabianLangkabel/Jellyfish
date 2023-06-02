#include "PointchargeEnergyNode.hpp"

PointchargeEnergyNode::PointchargeEnergyNode(std::string InpNodeTypeID) : Node()
{   
    NodeTypeID = InpNodeTypeID;
    NodeName = "Pointcharge Energy";
    NodeDescription = "Pointcharge Energy";
    NodeCategory = "Electronic Structure";
    Plugin = "Basics";
    HasCalulateFunction = true;
}

PointchargeEnergyNode::~PointchargeEnergyNode(){}

void PointchargeEnergyNode::LoadData(json Data, std::string file)
{
    if (State == NodeState::Calculated)
    {
        save_load::load(file, this->id, "energy", &energy);

        _energy = std::make_shared<double>(this->energy);
    }
}

json PointchargeEnergyNode::SaveData(std::string file)
{
    if (State == NodeState::Calculated)
    {
        save_load::save(file, this->id, "energy", energy);
    }

    json DataJson;
    return DataJson;
}

void PointchargeEnergyNode::calculate()
{
    QC::Pointcharge_Energy Pointcharge_Energy_Class;

    auto Pointcharges = *(_pointcharges.lock().get());
    Pointcharge_Energy_Class.set_pointcharges(Pointcharges);
    Pointcharge_Energy_Class.compute();

    this->energy = Pointcharge_Energy_Class.get_energy();
    _energy = std::make_shared<double>(this->energy);
    update_AllPortData();

    State = NodeState::Calculated;
}

void PointchargeEnergyNode::NodeInspector(QWidget* Inspector)
{
    QVBoxLayout* lay = new QVBoxLayout();
    Inspector->setLayout(lay);
}

bool PointchargeEnergyNode::InPortAllowMultipleConnections(int Port)
{ 
    return false; 
}

unsigned int PointchargeEnergyNode::Ports(NodePortType PortType)
{
    if(PortType == NodePortType::In)
    {
        return 1;
    }
    else
    {
        return 1;
    }
}

bool PointchargeEnergyNode::IsCustomPortCaption(NodePortType PortType, int Port)
{ 
    return true; 
}

std::string PointchargeEnergyNode::CustomPortCaption(NodePortType PortType, int Port)
{ 
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "Pointcharges"; }
    }
    else
    {
        if(Port == 0) {return "Energy"; }
    }
}

std::string PointchargeEnergyNode::DataTypeName(NodePortType PortType, int Port)
{
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "std::vector<QC::Pointcharge>"; }
    }
    else
    {
        if(Port == 0) {return "double"; }
    }
}

std::any PointchargeEnergyNode::getOutData(int Port) 
{ 
    std::any ret;
    ret = std::make_any<std::weak_ptr<double>>(_energy);
    return ret;
}

void PointchargeEnergyNode::setInData(int Port, std::any data)
{
    if(data.has_value())
    {
        if(Port == 0)
        {
            _pointcharges = std::any_cast<std::weak_ptr<std::vector<QC::Pointcharge>>>(data);
        }
    }
    else
    {
        if(Port == 0){_pointcharges.reset();}
    }
};