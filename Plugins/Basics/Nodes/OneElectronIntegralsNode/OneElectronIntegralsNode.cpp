#include "OneElectronIntegralsNode.hpp"

OneElectronIntegralsNode::OneElectronIntegralsNode(std::string InpNodeTypeID) : Node()
{   
    NodeTypeID = InpNodeTypeID;
    NodeName = "One-Electron Integrals";
    NodeDescription = "One-Electron Integrals";
    NodeCategory = "Integrals";
    Plugin = "Basics";
    HasCalulateFunction = true;
}

OneElectronIntegralsNode::~OneElectronIntegralsNode(){}

void OneElectronIntegralsNode::LoadData(json Data, std::string file)
{
    this->select_integral_id = Data["selected_integral"];

    if (State == NodeState::Calculated)
    {
        save_load::load(file, this->id, "integrals", &integrals);

        _oneelectronintegrals = std::make_shared<Eigen::MatrixXd>(this->integrals);
    }
}

json OneElectronIntegralsNode::SaveData(std::string file)
{
    if (State == NodeState::Calculated)
    {
        save_load::save(file, this->id, "integrals", integrals);
    }

    json DataJson;
    DataJson["selected_integral"] = this->select_integral_id;
    return DataJson;
}

void OneElectronIntegralsNode::calculate()
{
    QC::Integrator_libint_interface_1e integrator_class;

    auto Basisset = _basisset.lock().get();
    integrator_class.set_basisset(*Basisset);
    
    if (!_pointcharges.expired())
    {
        auto Pointcharges = _pointcharges.lock().get();
        integrator_class.set_pointcharges(*Pointcharges);
    }

    if (select_integral_id == 0)
    {
        integrator_class.set_integraltype(0);
        integrator_class.compute();
        integrals = integrator_class.get_Integrals();
    }
    else if (select_integral_id == 1)
    {
        integrator_class.set_integraltype(1);
        integrator_class.compute();
        integrals = integrator_class.get_Integrals();
    }
    else if (select_integral_id == 2)
    {
        integrator_class.set_integraltype(5);
        integrator_class.compute();
        integrals = integrator_class.get_Integrals();
    }
    else if (select_integral_id == 3)
    {
        integrator_class.set_integraltype(1);
        integrator_class.compute();
        Eigen::MatrixXd one_e_integrals1 = integrator_class.get_Integrals();

        integrator_class.set_integraltype(5);
        integrator_class.compute();
        Eigen::MatrixXd one_e_integrals2 = integrator_class.get_Integrals();

        integrals = one_e_integrals1 + one_e_integrals2;
    }
    else if (select_integral_id == 4)
    {
        integrator_class.set_integraltype(2);
        integrator_class.compute();
        integrals = integrator_class.get_Integrals();
    }
    else if (select_integral_id == 5)
    {
        integrator_class.set_integraltype(3);
        integrator_class.compute();
        integrals = integrator_class.get_Integrals();
    }
    else if (select_integral_id == 6)
    {
        integrator_class.set_integraltype(4);
        integrator_class.compute();
        integrals = integrator_class.get_Integrals();
    }


    _oneelectronintegrals = std::make_shared<Eigen::MatrixXd>(integrals);
    update_AllPortData();

    State = NodeState::Calculated;
}

void OneElectronIntegralsNode::NodeInspector(QWidget* Inspector)
{
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
    connect(Select_Integral, &QComboBox::currentTextChanged, this, &OneElectronIntegralsNode::selectintegral);

    lay->addWidget(new QLabel("Select Integral Type"));
    lay->addWidget(Select_Integral);

    Inspector->setLayout(lay);
}

bool OneElectronIntegralsNode::InPortAllowMultipleConnections(int Port)
{ 
    return false; 
}

unsigned int OneElectronIntegralsNode::Ports(NodePortType PortType)
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

bool OneElectronIntegralsNode::IsCustomPortCaption(NodePortType PortType, int Port)
{ 
    return true; 
}

std::string OneElectronIntegralsNode::CustomPortCaption(NodePortType PortType, int Port)
{ 
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "Pointcharges"; }
        else if(Port == 1) {return "Basisset"; }
    }
    else
    {
        if(Port == 0) {return "Integrals"; }
    }
}

std::string OneElectronIntegralsNode::DataTypeName(NodePortType PortType, int Port)
{
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "std::vector<QC::Pointcharge>"; }
        else if(Port == 1) {return "QC::Gaussian_Basisset"; }
    }
    else
    {
        if(Port == 0) {return "Eigen::MatrixXd"; }
    }
}

std::any OneElectronIntegralsNode::getOutData(int Port) 
{ 
    std::any ret;
    if(Port == 0)
    {
        ret = std::make_any<std::weak_ptr<Eigen::MatrixXd>>(_oneelectronintegrals);
    }
    return ret;
}

void OneElectronIntegralsNode::setInData(int Port, std::any data)
{
    if(data.has_value())
    {
        if(Port == 0)
        {
            _pointcharges = std::any_cast<std::weak_ptr<std::vector<QC::Pointcharge>>>(data);
        }
        if(Port == 1)
        {
            _basisset = std::any_cast<std::weak_ptr<QC::Gaussian_Basisset>>(data);
        }
    }
    else
    {
        if(Port == 0){_pointcharges.reset();}
        else if(Port == 1){_basisset.reset();}
    }
}


void OneElectronIntegralsNode::selectintegral()
{
    select_integral_id = Select_Integral->currentIndex();
};