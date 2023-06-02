#include "SimulatorQuESTNode.hpp"

SimulatorQuESTNode::SimulatorQuESTNode(std::string InpNodeTypeID) : Node()
{   
    NodeTypeID = InpNodeTypeID;
    NodeName = "QuEST Simulator";
    NodeDescription = "QuEST Simulator";
    NodeCategory = "QPUs";
    Plugin = "QuantumComputing";
    HasCalulateFunction = true;
}

SimulatorQuESTNode::~SimulatorQuESTNode(){}

void SimulatorQuESTNode::LoadData(json Data, std::string file)
{
    if (State == NodeState::Calculated)
    {
        _simulator = std::make_shared<QC::QC_Simulator>(Simulator);
    }
}

json SimulatorQuESTNode::SaveData(std::string file)
{
    if (State == NodeState::Calculated)
    {

    }
    json DataJson;
    return DataJson;
}

void SimulatorQuESTNode::calculate()
{
    _simulator = std::make_shared<QC::QC_Simulator>(Simulator);
    update_AllPortData();

    State = NodeState::Calculated;
}

void SimulatorQuESTNode::NodeInspector(QWidget* Inspector)
{
    /*
    QVBoxLayout* lay = new QVBoxLayout();
    lay->addWidget(new QLabel("Dies ist Testnode"));

    Select_Integral = new QComboBox;
    Select_Integral->addItem("Overlapintegrals <i|j>");
    Select_Integral->addItem("Kinetic Energy Integrals <i|Kin|j>");
    Select_Integral->addItem("Nuclear Repulsion Integrals <i|Nuc|j>");
    Select_Integral->addItem("Kinetic + Nuclear Integrals <i|Kin + Nuc|j>");
    Select_Integral->addItem("<i|x|j>");
    Select_Integral->addItem("<i|y|j>");
    Select_Integral->addItem("<i|z|j>");
    Select_Integral->setCurrentIndex(select_integral_id);
    connect(Select_Integral, &QComboBox::currentTextChanged, this, &SimulatorQuESTNode::selectintegral);

    lay->addWidget(new QLabel("Select Integral Type"));
    lay->addWidget(Select_Integral);

    Inspector->setLayout(lay);
    */
}

bool SimulatorQuESTNode::InPortAllowMultipleConnections(int Port)
{ 
    return false; 
}

unsigned int SimulatorQuESTNode::Ports(NodePortType PortType)
{
    if(PortType == NodePortType::In)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

bool SimulatorQuESTNode::IsCustomPortCaption(NodePortType PortType, int Port)
{ 
    return true; 
}

std::string SimulatorQuESTNode::CustomPortCaption(NodePortType PortType, int Port)
{ 
    if(PortType == NodePortType::In)
    {
    }
    else
    {
        if(Port == 0) {return "QPU"; }
    }
}

std::string SimulatorQuESTNode::DataTypeName(NodePortType PortType, int Port)
{
    if(PortType == NodePortType::In)
    {
    }
    else
    {
        if(Port == 0) {return "QC::QC_Simulator"; }
    }
}

std::any SimulatorQuESTNode::getOutData(int Port) 
{ 
    std::any ret;
    if(Port == 0)
    {
        ret = std::make_any<std::weak_ptr<QC::QC_Simulator>>(_simulator);
    }
    return ret;
}

void SimulatorQuESTNode::setInData(int Port, std::any data)
{

}