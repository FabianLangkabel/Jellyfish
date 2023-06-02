#include "OneElectronPotentialIntegralsNode.hpp"

OneElectronPotentialIntegralsNode::OneElectronPotentialIntegralsNode(std::string InpNodeTypeID) : Node()
{   
    NodeTypeID = InpNodeTypeID;
    NodeName = "One Electron-Integrals with Potential";
    NodeDescription = "One Electron-Integrals with Potential";
    NodeCategory = "Integrals";
    Plugin = "Basics";
    HasCalulateFunction = true;
}

OneElectronPotentialIntegralsNode::~OneElectronPotentialIntegralsNode(){}

void OneElectronPotentialIntegralsNode::LoadData(json Data, std::string file)
{
    this->potential_formular = Data["potential_formular"];
    this->integrator_range = Data["integrator_range"];
    this->integrator_points = Data["integrator_points"];

    if (State == NodeState::Calculated)
    {
        save_load::load(file, this->id, "integrals", &Integrals);

        _oneelectronintegrals = std::make_shared<Eigen::MatrixXd>(Integrals);
    }
}

json OneElectronPotentialIntegralsNode::SaveData(std::string file)
{
    if (State == NodeState::Calculated)
    {
        save_load::save(file, this->id, "integrals", Integrals);
    }

    json DataJson;
    DataJson["potential_formular"] = this->potential_formular;
    DataJson["integrator_range"] = this->integrator_range;
    DataJson["integrator_points"] = this->integrator_points;
    return DataJson;
}

void OneElectronPotentialIntegralsNode::calculate()
{
    auto Basisset = _basisset.lock().get();
    
    QC::Parser_Function func;
    func.set_function_string(this->potential_formular);
    
    QC::Integrator_Operator_Function_Numerical integrator;
    integrator.set_operator_function(&func);
    integrator.set_range(this->integrator_range);
    integrator.set_steps(this->integrator_points);
    integrator.set_basisset(*Basisset);
    integrator.compute();

    Integrals = integrator.get_Integrals();
    _oneelectronintegrals = std::make_shared<Eigen::MatrixXd>(Integrals);
    update_AllPortData();

    State = NodeState::Calculated;
}

void OneElectronPotentialIntegralsNode::NodeInspector(QWidget* Inspector)
{
    QVBoxLayout* lay = new QVBoxLayout();

    potential_formular_lineedit = new QLineEdit;
    potential_formular_lineedit->setText(QString::fromStdString(potential_formular));
    connect(potential_formular_lineedit, &QLineEdit::textChanged, this, &OneElectronPotentialIntegralsNode::select_potential_formular);

    lay->addWidget(new QLabel("Potential Formular (x,y,z)"));
    lay->addWidget(potential_formular_lineedit);

    auto line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    lay->addWidget(line);

    integrator_range_lineedit = new QLineEdit;
    integrator_range_lineedit->setValidator(new QDoubleValidator(0, 100000000, 5, this));
    integrator_range_lineedit->setText(QString::number(integrator_range));
    connect(integrator_range_lineedit, &QLineEdit::textChanged, this, &OneElectronPotentialIntegralsNode::select_integrator_range);
    lay->addWidget(new QLabel("Select CAP-Integrator-Range"));
    lay->addWidget(integrator_range_lineedit);

    auto line2 = new QFrame;
    line2->setFrameShape(QFrame::HLine);
    line2->setFrameShadow(QFrame::Sunken);
    lay->addWidget(line2);

    integrator_points_lineedit = new QLineEdit;
    integrator_points_lineedit->setValidator(new QIntValidator(0, 100000000, this));
    integrator_points_lineedit->setText(QString::number(integrator_points));
    connect(integrator_points_lineedit, &QLineEdit::textChanged, this, &OneElectronPotentialIntegralsNode::select_integrator_points);
    lay->addWidget(new QLabel("Select CAP-Integrator-Points"));
    lay->addWidget(integrator_points_lineedit);

    Inspector->setLayout(lay);
}

bool OneElectronPotentialIntegralsNode::InPortAllowMultipleConnections(int Port)
{ 
    return false; 
}

unsigned int OneElectronPotentialIntegralsNode::Ports(NodePortType PortType)
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

bool OneElectronPotentialIntegralsNode::IsCustomPortCaption(NodePortType PortType, int Port)
{ 
    return true; 
}

std::string OneElectronPotentialIntegralsNode::CustomPortCaption(NodePortType PortType, int Port)
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

std::string OneElectronPotentialIntegralsNode::DataTypeName(NodePortType PortType, int Port)
{
    if(PortType == NodePortType::In)
    {
        return "QC::Gaussian_Basisset";
    }
    else
    {
        return "Eigen::MatrixXd";
    }
}

std::any OneElectronPotentialIntegralsNode::getOutData(int Port) 
{ 
    std::any ret;
    if(Port == 0)
    {
        ret = std::make_any<std::weak_ptr<Eigen::MatrixXd>>(_oneelectronintegrals);
    }
    return ret;
}

void OneElectronPotentialIntegralsNode::setInData(int Port, std::any data)
{
    if(data.has_value())
    {
        if(Port == 0)
        {
            _basisset = std::any_cast<std::weak_ptr<QC::Gaussian_Basisset>>(data);
        }
    }
    else
    {
        if(Port == 0){_basisset.reset();}
    }
}

void OneElectronPotentialIntegralsNode::select_potential_formular()
{
    this->potential_formular = this->potential_formular_lineedit->text().toStdString();
    PropertiesChanged();
}

void OneElectronPotentialIntegralsNode::select_integrator_range()
{
    this->integrator_range = this->integrator_range_lineedit->text().toDouble();
    PropertiesChanged();
}

void OneElectronPotentialIntegralsNode::select_integrator_points()
{
    this->integrator_points = this->integrator_points_lineedit->text().toInt();
    PropertiesChanged();
};