#include "CombineBasissetsNode.hpp"

CombineBasissetsNode::CombineBasissetsNode(std::string InpNodeTypeID) : Node()
{   
    NodeTypeID = InpNodeTypeID;
    NodeName = "Combine Bassissets";
    NodeDescription = "Combine Bassissets";
    NodeCategory = "Bassisset";
    Plugin = "Basics";
    HasCalulateFunction = true;
}

CombineBasissetsNode::~CombineBasissetsNode(){}

void CombineBasissetsNode::LoadData(json Data, std::string file)
{
    if (State == NodeState::Calculated)
    {
        save_load::load(file, this->id, "basisset", &combined_basisset);

        _basissetout = std::make_shared<QC::Gaussian_Basisset>(this->combined_basisset);
    }
}

json CombineBasissetsNode::SaveData(std::string file)
{
    if (State == NodeState::Calculated)
    {
        save_load::save(file, this->id, "basisset", combined_basisset);
    }

    json DataJson;
    return DataJson;
}

void CombineBasissetsNode::calculate()
{
    auto Basisset1 = *(_basissetin1.lock().get());
    auto Basisset2 = *(_basissetin2.lock().get());

    QC::Gaussian_Basisset new_Basisset;
    new_Basisset.set_basisset_type(Basisset1.get_basisset_type());
    for (int i = 0; i < Basisset1.get_Basisfunctionnumber_angular_compact(); i++)
    {
        new_Basisset.add_Basisfunction(Basisset1.get_Basisfunction(i));
    }
    for (int i = 0; i < Basisset2.get_Basisfunctionnumber_angular_compact(); i++)
    {
        new_Basisset.add_Basisfunction(Basisset2.get_Basisfunction(i));
    }
    this->combined_basisset = new_Basisset;

    _basissetout = std::make_shared<QC::Gaussian_Basisset>(this->combined_basisset);
    update_AllPortData();

    State = NodeState::Calculated;
}

void CombineBasissetsNode::NodeInspector(QWidget* Inspector)
{
    QVBoxLayout* lay = new QVBoxLayout();
    Inspector->setLayout(lay);
}

bool CombineBasissetsNode::InPortAllowMultipleConnections(int Port)
{ 
    return false; 
}

unsigned int CombineBasissetsNode::Ports(NodePortType PortType)
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

bool CombineBasissetsNode::IsCustomPortCaption(NodePortType PortType, int Port)
{ 
    return true; 
}

std::string CombineBasissetsNode::CustomPortCaption(NodePortType PortType, int Port)
{ 
        if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "Basisset 1"; }
        else if(Port == 1) {return "Basisset 2"; }
    }
    else
    {
        if(Port == 0) {return "Basisset"; }
    }
}

std::string CombineBasissetsNode::DataTypeName(NodePortType PortType, int Port)
{
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "QC::Gaussian_Basisset"; }
        else if(Port == 1) {return "QC::Gaussian_Basisset"; }
    }
    else
    {
        if(Port == 0) {return "QC::Gaussian_Basisset"; }
    }
}

std::any CombineBasissetsNode::getOutData(int Port) 
{ 
    std::any ret;
    if(Port == 0)
    {
        ret = std::make_any<std::weak_ptr<QC::Gaussian_Basisset>>(_basissetout);
    }
    return ret;
}

void CombineBasissetsNode::setInData(int Port, std::any data)
{
    if(data.has_value())
    {
        if(Port == 0)
        {
            _basissetin1 = std::any_cast<std::weak_ptr<QC::Gaussian_Basisset>>(data);
        }
        if(Port == 1)
        {
            _basissetin2 = std::any_cast<std::weak_ptr<QC::Gaussian_Basisset>>(data);
        }
    }
    else
    {
        if(Port == 0){_basissetin1.reset();}
        else if(Port == 1){_basissetin2.reset();}
    }
};