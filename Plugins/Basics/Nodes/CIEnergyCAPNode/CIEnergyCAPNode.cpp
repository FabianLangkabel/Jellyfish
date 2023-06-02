#include "CIEnergyCAPNode.hpp"

CIEnergyCAPNode::CIEnergyCAPNode(std::string InpNodeTypeID) : Node()
{   
    NodeTypeID = InpNodeTypeID;
    NodeName = "Energy CAP";
    NodeDescription = "Energy CAP";
    NodeCategory = "Potentials";
    Plugin = "Basics";
    HasCalulateFunction = true;
}

CIEnergyCAPNode::~CIEnergyCAPNode(){}

void CIEnergyCAPNode::LoadData(json Data, std::string file)
{
    this->escape_lenght = Data["escape_lenght"];

    if (State == NodeState::Calculated)
    {
        save_load::load(file, this->id, "capmatrix", &capmatrix);

        _capmatrix = std::make_shared<Eigen::MatrixXcd>(this->capmatrix);
    }
}

json CIEnergyCAPNode::SaveData(std::string file)
{
    if (State == NodeState::Calculated)
    {
        save_load::save(file, this->id, "capmatrix", capmatrix);
    }

    json DataJson;
    DataJson["escape_lenght"] = this->escape_lenght;
    return DataJson;
}

void CIEnergyCAPNode::calculate()
{
    auto orbital_energys = *(_orbital_energys.lock().get());
    QC::CIResults ci_results = *(_ci_results.lock().get());

    Eigen::MatrixXd cap_matrix_spin_orbs = Eigen::MatrixXd::Zero(orbital_energys.rows() * 2, orbital_energys.rows() * 2);

    for (int i = 0; i < orbital_energys.rows(); i++)
    {
        if (orbital_energys(i) > 0)
        {
            cap_matrix_spin_orbs(i * 2, i * 2) = (1 / this->escape_lenght) * sqrt(orbital_energys(i));
            cap_matrix_spin_orbs(i * 2 + 1, i * 2 + 1) = (1 / this->escape_lenght) * sqrt(orbital_energys(i));
        }
    }

    QC::Configuration_Interaction_One_Electron_Operator CI_Operator;
    CI_Operator.set_ci_matrix(ci_results.ci_matrix);
    CI_Operator.set_one_electron_integrals_spinorbitals(cap_matrix_spin_orbs);

    std::vector<QC::Configuration> configurations;
    for (int i = 0; i < ci_results.configuration_strings.size(); i++)
    {
        QC::Configuration conf;
        for (int j = 0; j < ci_results.configuration_strings[i].size() / 2; j++) {
            if (ci_results.configuration_strings[i].substr(2 * j, 1) == "1") { conf.alphastring.push_back(1); }
            else { conf.alphastring.push_back(0); }
            if (ci_results.configuration_strings[i].substr(2 * j + 1, 1) == "1") { conf.betastring.push_back(1); }
            else { conf.betastring.push_back(0); }
        }
        configurations.push_back(conf);
    }
    CI_Operator.set_configuration(configurations);
    CI_Operator.compute();

    int electrons = 0;
    for (int i = 0; i < configurations[0].alphastring.size(); i++)
    {
        if (configurations[0].alphastring[i]) { electrons++; }
        if (configurations[0].betastring[i]) { electrons++; }
    }
    std::complex<double> prefactor(0, -1);

    this->capmatrix = prefactor * CI_Operator.get_one_electron_operator_matrix();
    _capmatrix = std::make_shared<Eigen::MatrixXcd>(this->capmatrix);
    update_AllPortData();

    State = NodeState::Calculated;
}

void CIEnergyCAPNode::NodeInspector(QWidget* Inspector)
{
    QVBoxLayout* lay = new QVBoxLayout();
    
    escape_lenght_lineedit = new QLineEdit;
    escape_lenght_lineedit->setValidator(new QDoubleValidator(0, 100000000, 5, this));
    escape_lenght_lineedit->setText(QString::number(escape_lenght));
    connect(escape_lenght_lineedit, &QLineEdit::textChanged, this, &CIEnergyCAPNode::select_escape_lenght);
    lay->addWidget(new QLabel("Select Escape Lenght"));
    lay->addWidget(escape_lenght_lineedit);

    Inspector->setLayout(lay);
}

bool CIEnergyCAPNode::InPortAllowMultipleConnections(int Port)
{ 
    return false; 
}

unsigned int CIEnergyCAPNode::Ports(NodePortType PortType)
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

bool CIEnergyCAPNode::IsCustomPortCaption(NodePortType PortType, int Port)
{ 
    return true; 
}

std::string CIEnergyCAPNode::CustomPortCaption(NodePortType PortType, int Port)
{ 
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "OrbitalEnergys"; }
        else if(Port == 1) {return "CIResults"; }
    }
    else
    {
        return "CAPMatrix";
    }
}

std::string CIEnergyCAPNode::DataTypeName(NodePortType PortType, int Port)
{
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "Eigen::VectorXd"; }
        else if(Port == 1) {return "QC::CIResults"; }
    }
    else
    {
        return "Eigen::MatrixXcd";
    }
}

std::any CIEnergyCAPNode::getOutData(int Port) 
{ 
    std::any ret;
    if(Port == 0)
    {
        ret = std::make_any<std::weak_ptr<Eigen::MatrixXcd>>(_capmatrix);
    }
    return ret;
}

void CIEnergyCAPNode::setInData(int Port, std::any data)
{
    if(data.has_value())
    {
        if(Port == 0)
        {
            _orbital_energys = std::any_cast<std::weak_ptr<Eigen::VectorXd>>(data);
        }
        if(Port == 1)
        {
            _ci_results = std::any_cast<std::weak_ptr<QC::CIResults>>(data);
        }
    }
    else
    {
        if(Port == 0){_orbital_energys.reset();}
        else if(Port == 1){_ci_results.reset();}
    }
}

void CIEnergyCAPNode::select_escape_lenght()
{
    this->escape_lenght = this->escape_lenght_lineedit->text().toDouble();
    PropertiesChanged();
};