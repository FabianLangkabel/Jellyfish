#include "GaussPotentialIntegralsNode.hpp"

GaussPotentialIntegralsNode::GaussPotentialIntegralsNode(std::string InpNodeTypeID) : Node()
{   
    NodeTypeID = InpNodeTypeID;
    NodeName = "Gauss Potential Integrals";
    NodeDescription = "Gauss Potential Integrals";
    NodeCategory = "Integrals";
    Plugin = "Basics";
    HasCalulateFunction = true;
}

GaussPotentialIntegralsNode::~GaussPotentialIntegralsNode(){}

void GaussPotentialIntegralsNode::LoadData(json Data, std::string file)
{
    if (State == NodeState::Calculated)
    {
        save_load::load(file, this->id, "integrals", &integrals);
        _oneelectronintegrals = std::make_shared<Eigen::MatrixXd>(this->integrals);
    }
}

json GaussPotentialIntegralsNode::SaveData(std::string file)
{
    if (State == NodeState::Calculated)
    {
        save_load::save(file, this->id, "integrals", integrals);
    }

    json DataJson;
    return DataJson;
}

void GaussPotentialIntegralsNode::calculate()
{
    QC::Integrator_Gaussian_Potential_Gaussian Integrator;

    auto Basisset = _basisset.lock().get();
    auto Gausspotentials = *(_gausspotentials.lock().get());

    Integrator.set_basisset(*Basisset);
    for (int i = 0; i < Gausspotentials.size(); i++)
    {
        Integrator.add_potential_gaussian(Gausspotentials[i]);
    }
    Integrator.compute();

    this->integrals = Integrator.get_Integrals();
    std::cout << this->integrals << std::endl;
    _oneelectronintegrals = std::make_shared<Eigen::MatrixXd>(this->integrals);
    update_AllPortData();

    State = NodeState::Calculated;
}

void GaussPotentialIntegralsNode::NodeInspector(QWidget* Inspector)
{
    QVBoxLayout* lay = new QVBoxLayout();
    Inspector->setLayout(lay);
}

bool GaussPotentialIntegralsNode::InPortAllowMultipleConnections(int Port)
{ 
    return false; 
}

unsigned int GaussPotentialIntegralsNode::Ports(NodePortType PortType)
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

bool GaussPotentialIntegralsNode::IsCustomPortCaption(NodePortType PortType, int Port)
{ 
    return true; 
}

std::string GaussPotentialIntegralsNode::CustomPortCaption(NodePortType PortType, int Port)
{ 
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "GaussPotentials"; }
        else if(Port == 1) {return "Basisset"; }
    }
    else
    {
        if(Port == 0) {return "Integrals"; }
    }
}

std::string GaussPotentialIntegralsNode::DataTypeName(NodePortType PortType, int Port)
{
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "std::vector<QC::Potential_Gaussian>"; }
        else if(Port == 1) {return "QC::Gaussian_Basisset"; }
    }
    else
    {
        if(Port == 0) {return "Eigen::MatrixXd"; }
    }
}

std::any GaussPotentialIntegralsNode::getOutData(int Port) 
{ 
    std::any ret;
    if(Port == 0)
    {
        ret = std::make_any<std::weak_ptr<Eigen::MatrixXd>>(_oneelectronintegrals);
    }
    return ret;
}

void GaussPotentialIntegralsNode::setInData(int Port, std::any data)
{
    if(data.has_value())
    {
        if(Port == 0)
        {
            _gausspotentials = std::any_cast<std::weak_ptr<std::vector<QC::Potential_Gaussian>>>(data);
        }
        if(Port == 1)
        {
            _basisset = std::any_cast<std::weak_ptr<QC::Gaussian_Basisset>>(data);
        }
    }
    else
    {
        if(Port == 0){_gausspotentials.reset();}
        else if(Port == 1){_basisset.reset();}
    }
};