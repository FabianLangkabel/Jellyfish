#include "InputElectronNumberNode.hpp"

InputElectronNumberNode::InputElectronNumberNode(std::string InpNodeTypeID) : Node()
{   
    NodeTypeID = InpNodeTypeID;
    NodeName = "Set Electron Number";
    NodeDescription = "Set Electron Number";
    NodeCategory = "Input";
    Plugin = "Basics";
    HasCalulateFunction = true;
}

InputElectronNumberNode::~InputElectronNumberNode(){}

void InputElectronNumberNode::LoadData(json Data, std::string file)
{
    this->electrons = Data["electrons"];

    if (State == NodeState::Calculated)
    {
        //save_load::load(file, this->id, "electrons", &electrons);
        _electrons = std::make_shared<int>(this->electrons);
    }
}

json InputElectronNumberNode::SaveData(std::string file)
{
    if (State == NodeState::Calculated)
    {
        //save_load::save(file, this->id, "electrons", electrons);
    }

    json DataJson;
    DataJson["electrons"] = this->electrons;
    return DataJson;
}

void InputElectronNumberNode::calculate()
{
    _electrons = std::make_shared<int>(this->electrons);
    update_AllPortData();
    
    State = NodeState::Calculated;
}

void InputElectronNumberNode::NodeInspector(QWidget* Inspector)
{
    QVBoxLayout* lay = new QVBoxLayout();

    selectedelectrons = new QLineEdit;
    selectedelectrons->setValidator(new QIntValidator(0, 100));
    selectedelectrons->setText(QString::number(electrons));
    connect(selectedelectrons, &QLineEdit::textChanged, this, &InputElectronNumberNode::selectelectrons);

    lay->addWidget(new QLabel("Set Electrons"));
    lay->addWidget(selectedelectrons);

    Inspector->setLayout(lay);
}

bool InputElectronNumberNode::InPortAllowMultipleConnections(int Port)
{ 
    return false; 
}

unsigned int InputElectronNumberNode::Ports(NodePortType PortType)
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

bool InputElectronNumberNode::IsCustomPortCaption(NodePortType PortType, int Port)
{ 
    return true; 
}

std::string InputElectronNumberNode::CustomPortCaption(NodePortType PortType, int Port)
{ 
    return "Electrons"; 
}

std::string InputElectronNumberNode::DataTypeName(NodePortType PortType, int Port)
{
    return "int";
}

std::any InputElectronNumberNode::getOutData(int Port) 
{ 
    std::any ret;
    if(Port == 0)
    {
        ret = std::make_any<std::weak_ptr<int>>(_electrons);
    }
    return ret;
}

void InputElectronNumberNode::setInData(int Port, std::any data)
{
}

void InputElectronNumberNode::selectelectrons()
{
    electrons = this->selectedelectrons->text().toInt();
    PropertiesChanged();
};