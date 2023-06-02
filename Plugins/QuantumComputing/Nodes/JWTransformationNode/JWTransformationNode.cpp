#include "JWTransformationNode.hpp"

JWTransformationNode::JWTransformationNode(std::string InpNodeTypeID) : Node()
{   
    NodeTypeID = InpNodeTypeID;
    NodeName = "Jordan-Wigner-Transformation";
    NodeDescription = "Jordan-Wigner-Transformation";
    NodeCategory = "Transformation";
    Plugin = "QuantumComputing";
    HasCalulateFunction = true;
}

JWTransformationNode::~JWTransformationNode(){}

void JWTransformationNode::LoadData(json Data, std::string file)
{
    this->integral_threshold = Data["integral_threshold"];

    if (State == NodeState::Calculated)
    {
        save_load::load(file, this->id, "pauli_operator", &Operator);

        _paulioperator = std::make_shared<QC::Pauli_Operator>(this->Operator);
    }
}

json JWTransformationNode::SaveData(std::string file)
{
    if (State == NodeState::Calculated)
    {
        save_load::save(file, this->id, "pauli_operator", Operator);
    }

    json DataJson;
    DataJson["integral_threshold"] = this->integral_threshold;
    return DataJson;
}

void JWTransformationNode::calculate()
{
    bool translate_one_electron_integrals = false;
    bool translate_two_electron_integrals = false;

    Eigen::MatrixXd OneElectronIntegral;
    Eigen::Tensor<double, 4> TwoElectronIntegral;
    double e0 = 0;

    if (!_oneelectronintegralsin.expired()){
        translate_one_electron_integrals = true;
        OneElectronIntegral = *(_oneelectronintegralsin.lock().get());
    }
    if (!_twoelectronintegralsin.expired()) {
        translate_two_electron_integrals = true;
        TwoElectronIntegral = *(_twoelectronintegralsin.lock().get());
    }
    if (!_e0.expired()) {
        e0 = *(_e0.lock().get());
    }

    QC::MO_Integrals_to_JW_Pauli_Operator Transformation;
    Transformation.set_one_electron_integrals(OneElectronIntegral);
    Transformation.set_two_electron_integrals(TwoElectronIntegral);
    Transformation.set_e0(e0);
    Transformation.compute(translate_one_electron_integrals, translate_two_electron_integrals);

    this->Operator = Transformation.get_Pauli_Operator();
    //this->Operator.print_Pauli_Operator();
    _paulioperator = std::make_shared<QC::Pauli_Operator>(this->Operator);
    update_AllPortData();

    State = NodeState::Calculated;
}

void JWTransformationNode::NodeInspector(QWidget* Inspector)
{
    QVBoxLayout* lay = new QVBoxLayout();
    integral_threshold_lineedit = new QLineEdit;
    integral_threshold_lineedit->setValidator(new QDoubleValidator(0.0, 0.1, 10, this));
    integral_threshold_lineedit->setText(QString::number(integral_threshold));
    connect(integral_threshold_lineedit, &QLineEdit::textChanged, this, &JWTransformationNode::select_threshold);

    lay->addWidget(new QLabel("Set Integral Threshold"));
    lay->addWidget(integral_threshold_lineedit);

    Inspector->setLayout(lay);
}

bool JWTransformationNode::InPortAllowMultipleConnections(int Port)
{ 
    return false; 
}

unsigned int JWTransformationNode::Ports(NodePortType PortType)
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

bool JWTransformationNode::IsCustomPortCaption(NodePortType PortType, int Port)
{ 
    return true; 
}

std::string JWTransformationNode::CustomPortCaption(NodePortType PortType, int Port)
{ 
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "1e Integrals"; }
        else if(Port == 1) {return "2e Integrals"; }
        else if(Port == 2) {return "Shift"; }
    }
    else
    {
        if(Port == 0) {return "Pauli Operator"; }
    }
}

std::string JWTransformationNode::DataTypeName(NodePortType PortType, int Port)
{
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "Eigen::MatrixXd"; }
        else if(Port == 1) {return "Eigen::Tensor<double, 4>"; }
        else if(Port == 2) {return "double"; }
    }
    else
    {
        if(Port == 0) {return "QC::Pauli_Operator"; }
    }
}

std::any JWTransformationNode::getOutData(int Port) 
{ 
    std::any ret;
    if(Port == 0)
    {
        ret = std::make_any<std::weak_ptr<QC::Pauli_Operator>>(_paulioperator);
    }
    return ret;
}

void JWTransformationNode::setInData(int Port, std::any data)
{
    if(data.has_value())
    {
        if(Port == 0)
        {
            _oneelectronintegralsin = std::any_cast<std::weak_ptr<Eigen::MatrixXd>>(data);
        }
        if(Port == 1)
        {
            _twoelectronintegralsin = std::any_cast<std::weak_ptr<Eigen::Tensor<double, 4>>>(data);
        }
        if(Port == 2)
        {
            _e0 = std::any_cast<std::weak_ptr<double>>(data);
        }
    }
    else
    {
        if(Port == 0){_oneelectronintegralsin.reset();}
        else if(Port == 1){_twoelectronintegralsin.reset();}
        else if(Port == 2){_e0.reset();}
    }
}

void JWTransformationNode::select_threshold()
{
    integral_threshold = this->integral_threshold_lineedit->text().toDouble();
    PropertiesChanged();
}