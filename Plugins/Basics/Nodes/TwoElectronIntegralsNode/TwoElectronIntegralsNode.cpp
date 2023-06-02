#include "TwoElectronIntegralsNode.hpp"

TwoElectronIntegralsNode::TwoElectronIntegralsNode(std::string InpNodeTypeID) : Node()
{   
    NodeTypeID = InpNodeTypeID;
    NodeName = "Two-Electron Integrals";
    NodeDescription = "Two-Electron Integrals";
    NodeCategory = "Integrals";
    Plugin = "Basics";
    HasCalulateFunction = true;
}

TwoElectronIntegralsNode::~TwoElectronIntegralsNode(){}

void TwoElectronIntegralsNode::LoadData(json Data, std::string file)
{
    this->select_integral_id = Data["selected_integral"];

    if (State == NodeState::Calculated)
    {
        save_load::load(file, this->id, "integrals", &integrals);

         _twoelectronintegrals = std::make_shared<Eigen::Tensor<double, 4>>(integrals);
    }
}

json TwoElectronIntegralsNode::SaveData(std::string file)
{
    if (State == NodeState::Calculated)
    {
        save_load::save(file, this->id, "integrals", integrals);
    }

    json DataJson;
    DataJson["selected_integral"] = this->select_integral_id;
    return DataJson;
}

void TwoElectronIntegralsNode::calculate()
{
    QC::Integrator_libint_interface_2e integrator_class;
    auto Basisset = _basisset.lock().get();
    integrator_class.set_basisset(*Basisset);


    if (select_integral_id == 0)
    {
        integrator_class.set_integraltype(0);
        integrator_class.compute();
    }

    this->integrals = integrator_class.get_Integrals();
    _twoelectronintegrals = std::make_shared<Eigen::Tensor<double, 4>>(integrals);
    update_AllPortData();

    State = NodeState::Calculated;
}

void TwoElectronIntegralsNode::NodeInspector(QWidget* Inspector)
{
    QVBoxLayout* lay = new QVBoxLayout();

    Select_Integral = new QComboBox;
    Select_Integral->addItem("Repulsionintegrals <ij|kl>");
    Select_Integral->setCurrentIndex(this->select_integral_id);
    connect(Select_Integral, &QComboBox::currentTextChanged, this, &TwoElectronIntegralsNode::selectintegral);

    lay->addWidget(new QLabel("Select Integral Type"));
    lay->addWidget(Select_Integral);

    Inspector->setLayout(lay);
}

bool TwoElectronIntegralsNode::InPortAllowMultipleConnections(int Port)
{ 
    return false; 
}

unsigned int TwoElectronIntegralsNode::Ports(NodePortType PortType)
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

bool TwoElectronIntegralsNode::IsCustomPortCaption(NodePortType PortType, int Port)
{ 
    return true; 
}

std::string TwoElectronIntegralsNode::CustomPortCaption(NodePortType PortType, int Port)
{ 
    if(PortType == NodePortType::In)
    {
        return "Basisset";
    }
    else
    {
        return "Integrals";
    }
}

std::string TwoElectronIntegralsNode::DataTypeName(NodePortType PortType, int Port)
{
    if(PortType == NodePortType::In)
    {
        return "QC::Gaussian_Basisset";
    }
    else
    {
        return "Eigen::Tensor<double, 4>";
    }
}

std::any TwoElectronIntegralsNode::getOutData(int Port) 
{ 
    std::any ret;
    if(Port == 0)
    {
        ret = std::make_any<std::weak_ptr<Eigen::Tensor<double, 4>>>(_twoelectronintegrals);
    }
    return ret;
}

void TwoElectronIntegralsNode::setInData(int Port, std::any data)
{
    if(data.has_value())
    {
        _basisset = std::any_cast<std::weak_ptr<QC::Gaussian_Basisset>>(data);
    }
    else
    {
        if(Port == 0){_basisset.reset();}
    }
}

void TwoElectronIntegralsNode::selectintegral()
{
    select_integral_id = Select_Integral->currentIndex();
    PropertiesChanged();
};