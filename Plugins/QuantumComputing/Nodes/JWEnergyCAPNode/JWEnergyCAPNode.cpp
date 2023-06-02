#include "JWEnergyCAPNode.hpp"

JWEnergyCAPNode::JWEnergyCAPNode(std::string InpNodeTypeID) : Node()
{   
    NodeTypeID = InpNodeTypeID;
    NodeName = "JW-Transformation for Energy CAP";
    NodeDescription = "JW-Transformation for Energy CAP";
    NodeCategory = "Transformation";
    Plugin = "QuantumComputing";
    HasCalulateFunction = true;
}

JWEnergyCAPNode::~JWEnergyCAPNode(){}

void JWEnergyCAPNode::LoadData(json Data, std::string file)
{
    this->escape_lenght = Data["escape_lenght"];

    if (State == NodeState::Calculated)
    {
        save_load::load(file, this->id, "pauli_operator", &Operator);

        _paulioperator = std::make_shared<QC::Pauli_Operator>(this->Operator);
    }
}

json JWEnergyCAPNode::SaveData(std::string file)
{   
    if (State == NodeState::Calculated)
    {
        save_load::save(file, this->id, "pauli_operator", Operator);
    }

    json DataJson;
    DataJson["escape_lenght"] = this->escape_lenght;
    return DataJson;
}

void JWEnergyCAPNode::calculate()
{
    auto orbital_energys = *(_orbital_energys.lock().get());
    Eigen::MatrixXd cap_matrix = Eigen::MatrixXd::Zero(orbital_energys.rows(), orbital_energys.rows());

    for (int i = 0; i < orbital_energys.rows(); i++)
    {
        if (orbital_energys(i) > 0)
        {
            cap_matrix(i, i) = (1 / this->escape_lenght) * sqrt(orbital_energys(i));
        }
    }
    QC::MO_Integrals_to_JW_Pauli_Operator Transformation;
    Transformation.set_one_electron_integrals(cap_matrix);
    Transformation.set_e0(0);
    Transformation.compute(true, false);

    this->Operator = Transformation.get_Pauli_Operator();
    _paulioperator = std::make_shared<QC::Pauli_Operator>(this->Operator);
    update_AllPortData();

    State = NodeState::Calculated;
}

void JWEnergyCAPNode::NodeInspector(QWidget* Inspector)
{
    QVBoxLayout* lay = new QVBoxLayout();
    escape_lenght_lineedit = new QLineEdit;
    escape_lenght_lineedit->setValidator(new QDoubleValidator(0, 100000000, 5, this));
    escape_lenght_lineedit->setText(QString::number(escape_lenght));
    connect(escape_lenght_lineedit, &QLineEdit::textChanged, this, &JWEnergyCAPNode::select_escape_lenght);
    lay->addWidget(new QLabel("Select Escape Lenght"));
    lay->addWidget(escape_lenght_lineedit);    
    
    Inspector->setLayout(lay);
}

bool JWEnergyCAPNode::InPortAllowMultipleConnections(int Port)
{ 
    return false; 
}

unsigned int JWEnergyCAPNode::Ports(NodePortType PortType)
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

bool JWEnergyCAPNode::IsCustomPortCaption(NodePortType PortType, int Port)
{ 
    return true; 
}

std::string JWEnergyCAPNode::CustomPortCaption(NodePortType PortType, int Port)
{ 
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "Orbital Energys"; }
    }
    else
    {
        if(Port == 0) {return "Pauli Operator"; }
    }
}

std::string JWEnergyCAPNode::DataTypeName(NodePortType PortType, int Port)
{
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "Eigen::VectorXd"; }
    }
    else
    {
        if(Port == 0) {return "QC::Pauli_Operator"; }
    }
}

std::any JWEnergyCAPNode::getOutData(int Port) 
{ 
    std::any ret;
    if(Port == 0)
    {
        ret = std::make_any<std::weak_ptr<QC::Pauli_Operator>>(_paulioperator);
    }
    return ret;
}

void JWEnergyCAPNode::setInData(int Port, std::any data)
{
    if(data.has_value())
    {
        if(Port == 0)
        {
            _orbital_energys = std::any_cast<std::weak_ptr<Eigen::VectorXd>>(data);
        }
    }
    else
    {
        if(Port == 0){_orbital_energys.reset();}
    }
}

void JWEnergyCAPNode::select_escape_lenght()
{
    this->escape_lenght = this->escape_lenght_lineedit->text().toDouble();
    PropertiesChanged();
}