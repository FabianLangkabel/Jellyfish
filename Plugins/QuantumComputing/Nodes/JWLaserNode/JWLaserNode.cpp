#include "JWLaserNode.hpp"

JWLaserNode::JWLaserNode(std::string InpNodeTypeID) : Node()
{   
    NodeTypeID = InpNodeTypeID;
    NodeName = "JW-Transformation for Laser";
    NodeDescription = "JW-Transformation for Laser";
    NodeCategory = "Transformation";
    Plugin = "QuantumComputing";
    HasCalulateFunction = true;
}

JWLaserNode::~JWLaserNode(){}

void JWLaserNode::LoadData(json Data, std::string file)
{
    this->integral_threshold = Data["integral_threshold"];

    if (State == NodeState::Calculated)
    {
        State == NodeState::notCalculated; //Is not loaded properly yet, so just make it uncalculated
    }
}

json JWLaserNode::SaveData(std::string file)
{
    if (State == NodeState::Calculated)
    {
    }

    json DataJson;
    DataJson["integral_threshold"] = this->integral_threshold;
    return DataJson;
}

void JWLaserNode::calculate()
{
    auto CMatrix = *(_hfcmatrix.lock().get());
    this->laseroperator.set_transformation_matrix(CMatrix);
    this->laseroperator.set_Basisset(*(_basisset.lock().get()));
    this->laseroperator.set_Laser(_laser.lock().get());

    _laserpaulioperator = std::make_shared<QC::Laser_Pauli_Operator>(laseroperator);
    //_laserpaulioperator = std::make_shared<QC::Laser_Pauli_Operator>(&laseroperator); //?
    update_AllPortData();

    State = NodeState::Calculated;
}

void JWLaserNode::NodeInspector(QWidget* Inspector)
{
    
    QVBoxLayout* lay = new QVBoxLayout();
    integral_threshold_lineedit = new QLineEdit;
    integral_threshold_lineedit->setValidator(new QDoubleValidator(0.0, 0.1, 10, this));
    integral_threshold_lineedit->setText(QString::number(integral_threshold));
    connect(integral_threshold_lineedit, &QLineEdit::textChanged, this, &JWLaserNode::select_threshold);

    lay->addWidget(new QLabel("Set Integral Threshold"));
    lay->addWidget(integral_threshold_lineedit);
    Inspector->setLayout(lay);
}

bool JWLaserNode::InPortAllowMultipleConnections(int Port)
{ 
    return false; 
}

unsigned int JWLaserNode::Ports(NodePortType PortType)
{
    if(PortType == NodePortType::In)
    {
        return 3;
    }
    else
    {
        return 1;
    }
}

bool JWLaserNode::IsCustomPortCaption(NodePortType PortType, int Port)
{ 
    return true; 
}

std::string JWLaserNode::CustomPortCaption(NodePortType PortType, int Port)
{ 
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "Basisset"; }
        else if(Port == 1) {return "Laser"; }
        else if(Port == 2) {return "C-Matrix"; }
    }
    else
    {
        if(Port == 0) {return "Laser Pauli Operator"; }
    }
}

std::string JWLaserNode::DataTypeName(NodePortType PortType, int Port)
{
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "QC::Gaussian_Basisset"; }
        else if(Port == 1) {return "QC::Laser"; }
        else if(Port == 2) {return "Eigen::MatrixXd"; }
    }
    else
    {
        if(Port == 0) {return "QC::Laser_Pauli_Operator"; }
    }
}

std::any JWLaserNode::getOutData(int Port) 
{ 
    std::any ret;
    if(Port == 0)
    {
        ret = std::make_any<std::weak_ptr<QC::Laser_Pauli_Operator>>(_laserpaulioperator);
    }
    return ret;
}

void JWLaserNode::setInData(int Port, std::any data)
{
    if(data.has_value())
    {
        if(Port == 0)
        {
            _basisset = std::any_cast<std::weak_ptr<QC::Gaussian_Basisset>>(data);
        }
        if(Port == 1)
        {
            _laser = std::any_cast<std::weak_ptr<QC::Laser>>(data);
        }
        if(Port == 2)
        {
            _hfcmatrix = std::any_cast<std::weak_ptr<Eigen::MatrixXd>>(data);
        }
    }
    else
    {
        if(Port == 0){_basisset.reset();}
        else if(Port == 1){_laser.reset();}
        else if(Port == 2){_hfcmatrix.reset();}
    }
}

void JWLaserNode::select_threshold()
{
    integral_threshold = this->integral_threshold_lineedit->text().toDouble();
    PropertiesChanged();
}