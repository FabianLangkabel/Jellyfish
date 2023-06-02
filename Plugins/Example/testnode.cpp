#include "testnode.hpp"

Testnode::Testnode(std::string InpNodeTypeID) : Node()
{   
    NodeTypeID = InpNodeTypeID;
    NodeName = "Testnode";
    NodeDescription = "Dies ist ein Testnode";
    Plugin = "test";
    HasCalulateFunction = true;
}

Testnode::~Testnode(){}

void Testnode::calculate()
{
    std::cout << "Dies ist Testnode" << std::endl;
    State = NodeState::Calculated;
}

void Testnode::NodeInspector(QWidget* Inspector)
{
    QVBoxLayout* lay = new QVBoxLayout();
    lay->addWidget(new QLabel("Dies ist Testnode"));
    Inspector->setLayout(lay);
}

bool Testnode::InPortAllowMultipleConnections(int Port)
{ 
    return false; 
}

unsigned int Testnode::Ports(NodePortType PortType)
{
    if(PortType == NodePortType::In)
    {
        return 2;
    }
    else
    {
        return 2;
    }
}

bool Testnode::IsCustomPortCaption(NodePortType PortType, int Port)
{ 
    return false; 
}

std::string Testnode::CustomPortCaption(NodePortType PortType, int Port)
{ 
    return ""; 
}

std::string Testnode::DataTypeName(NodePortType PortType, int Port)
{
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "int"; }
        else if(Port == 1) {return "Eigen"; }
    }
    else
    {
        if(Port == 0) {return "int"; }
        else if(Port == 1) {return "Eigen"; }
    }
}

std::any Testnode::getOutData(int Port) 
{ 
    std::any ret;
    if(Port == 0)
    {
        ret = std::make_any<int>(5);
    }
    else
    {
        ret = std::make_any<std::string>("Hallo");
    }
    return ret;
}

void Testnode::setInData(int Port, std::any data)
{
    if(data.has_value())
    {
        if(Port == 0)
        {
            int test = std::any_cast<int>(data);
            std::cout << test << std::endl;
        }
        if(Port == 1)
        {
            std::string test = std::any_cast<std::string>(data);
            std::cout << test << std::endl;
        }
    }
    else
    {
        std::cout << "Data hat kein Value" << std::endl;
    }
};