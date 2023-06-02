#include "RestrictedHartreeFockNode.hpp"

RestrictedHartreeFockNode::RestrictedHartreeFockNode(std::string InpNodeTypeID) : Node()
{   
    NodeTypeID = InpNodeTypeID;
    NodeName = "Restricted Hartree-Fock";
    NodeDescription = "Restricted Hartree-Fock";
    NodeCategory = "Electronic Structure";
    Plugin = "Basics";
    HasCalulateFunction = true;
}

RestrictedHartreeFockNode::~RestrictedHartreeFockNode(){}

void RestrictedHartreeFockNode::LoadData(json Data, std::string file)
{
    this->max_iteration_steps = Data["max_iteration_steps"];
    this->energy_threshold = Data["energy_threshold"];

    if (State == NodeState::Calculated)
    {
        save_load::load(file, this->id, "c_matrix", &c_matrix);
        save_load::load(file, this->id, "density_matrix", &density_matrix);
        save_load::load(file, this->id, "energy", &energy);
        save_load::load(file, this->id, "orbital_energys", &orbital_energys);

        _hfcmatrix = std::make_shared<Eigen::MatrixXd>(this->c_matrix);
        _hfdensitymatrix = std::make_shared<Eigen::MatrixXd>(this->density_matrix);
        _hfenergy = std::make_shared<double>(this->energy);
        _orbitalenergys = std::make_shared<Eigen::VectorXd>(this->orbital_energys);
    }
}

json RestrictedHartreeFockNode::SaveData(std::string file)
{
    if (State == NodeState::Calculated)
    {
        save_load::save(file, this->id, "c_matrix", c_matrix);
        save_load::save(file, this->id, "density_matrix", density_matrix);
        save_load::save(file, this->id, "energy", energy);
        save_load::save(file, this->id, "orbital_energys", orbital_energys);
    }

    json DataJson;
    DataJson["max_iteration_steps"] = this->max_iteration_steps;
    DataJson["energy_threshold"] = this->energy_threshold;
    return DataJson;
}

void RestrictedHartreeFockNode::calculate()
{
    QC::Hartree_Fock_Closed_Shell Hartree_Fock_Class;

    auto Overlapintegrals = _overlapintegrals.lock().get();
    auto OneElectronOperator = _oneelectronoperatorintegrals.lock().get();
    auto TwoElectronOperator = _twoelectronoperatorintegrals.lock().get();
    auto Electrons = _electrons.lock().get();

    Hartree_Fock_Class.set_overlapmatrix(*Overlapintegrals);
    Hartree_Fock_Class.set_one_electron_integrals(*OneElectronOperator);
    Hartree_Fock_Class.set_two_electron_integrals(*TwoElectronOperator);
    Hartree_Fock_Class.set_electrons(*Electrons);
    Hartree_Fock_Class.set_max_iteration_steps(this->max_iteration_steps);
    Hartree_Fock_Class.set_energy_threshold(this->energy_threshold);

    if (!_energy0.expired())
    {
        auto E0 = _energy0.lock().get();
        Hartree_Fock_Class.set_E0(*E0);
    }
    else { Hartree_Fock_Class.set_E0(0); }

    Hartree_Fock_Class.compute();

    this->c_matrix = Hartree_Fock_Class.get_C_matrix();
    this->density_matrix = Hartree_Fock_Class.get_densitymatrix();
    this->energy = Hartree_Fock_Class.get_groundstate_Energy();
    this->orbital_energys = Hartree_Fock_Class.get_orbitalenergys();

    _hfcmatrix = std::make_shared<Eigen::MatrixXd>(this->c_matrix);
    _hfdensitymatrix = std::make_shared<Eigen::MatrixXd>(this->density_matrix);
    _hfenergy = std::make_shared<double>(this->energy);
    _orbitalenergys = std::make_shared<Eigen::VectorXd>(this->orbital_energys);
    update_AllPortData();

    State = NodeState::Calculated;
}

void RestrictedHartreeFockNode::NodeInspector(QWidget* Inspector)
{
    QVBoxLayout* lay = new QVBoxLayout();

    selectedmaxiter = new QLineEdit;
    selectedmaxiter->setValidator(new QIntValidator(0, 100000000, this));
    selectedmaxiter->setText(QString::number(max_iteration_steps));
    connect(selectedmaxiter, &QLineEdit::textChanged, this, &RestrictedHartreeFockNode::selectmaxiter);

    auto line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    selectedthreshold = new QLineEdit;
    selectedthreshold->setValidator(new QDoubleValidator(0.0, 0.1, 10, this));
    selectedthreshold->setText(QString::number(energy_threshold));
    connect(selectedthreshold, &QLineEdit::textChanged, this, &RestrictedHartreeFockNode::selectthreshold);

    lay->addWidget(new QLabel("Select maximum iteration steps"));
    lay->addWidget(selectedmaxiter);
    lay->addWidget(line);
    lay->addWidget(new QLabel("Select Energy-threshold"));
    lay->addWidget(selectedthreshold);

    QPushButton* show_results_button = new QPushButton("Show Results");
    connect(show_results_button, &QPushButton::clicked, this, &RestrictedHartreeFockNode::show_results);
    lay->addWidget(show_results_button);

    Inspector->setLayout(lay);
}

bool RestrictedHartreeFockNode::InPortAllowMultipleConnections(int Port)
{ 
    return false; 
}

unsigned int RestrictedHartreeFockNode::Ports(NodePortType PortType)
{
    if(PortType == NodePortType::In)
    {
        return 5;
    }
    else
    {
        return 4;
    }
}

bool RestrictedHartreeFockNode::IsCustomPortCaption(NodePortType PortType, int Port)
{ 
    return true; 
}

std::string RestrictedHartreeFockNode::CustomPortCaption(NodePortType PortType, int Port)
{ 
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "Overlapintegrals"; }
        else if(Port == 1) {return "1e Integrals"; }
        else if(Port == 2) {return "2e Integrals"; }
        else if(Port == 3) {return "Electrons"; }
        else if(Port == 4) {return "Energyshift"; }
    }
    else
    {
        if(Port == 0) {return "C-Matrix"; }
        else if(Port == 1) {return "Densitymatrix"; }
        else if(Port == 2) {return "HF Energy"; }
        else if(Port == 3) {return "Orbital Energys"; }
    }
}

std::string RestrictedHartreeFockNode::DataTypeName(NodePortType PortType, int Port)
{
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "Eigen::MatrixXd"; }
        else if(Port == 1) {return "Eigen::MatrixXd"; }
        else if(Port == 2) {return "Eigen::Tensor<double, 4>"; }
        else if(Port == 3) {return "int"; }
        else if(Port == 4) {return "double"; }
    }
    else
    {
        if(Port == 0) {return "Eigen::MatrixXd"; }
        else if(Port == 1) {return "Eigen::MatrixXd"; }
        else if(Port == 2) {return "double"; }
        else if(Port == 3) {return "Eigen::VectorXd"; }
    }
}

std::any RestrictedHartreeFockNode::getOutData(int Port) 
{ 
    std::any ret;
    if(Port == 0) { ret = std::make_any<std::weak_ptr<Eigen::MatrixXd>>(_hfcmatrix); }
    else if(Port == 1) { ret = std::make_any<std::weak_ptr<Eigen::MatrixXd>>(_hfdensitymatrix); }
    else if(Port == 2) { ret = std::make_any<std::weak_ptr<double>>(_hfenergy); }
    else if(Port == 3) { ret = std::make_any<std::weak_ptr<Eigen::VectorXd>>(_orbitalenergys); }
    return ret;
}

void RestrictedHartreeFockNode::setInData(int Port, std::any data)
{
    if(data.has_value())
    {
        if(Port == 0){ _overlapintegrals = std::any_cast<std::weak_ptr<Eigen::MatrixXd>>(data); }
        else if(Port == 1){ _oneelectronoperatorintegrals = std::any_cast<std::weak_ptr<Eigen::MatrixXd>>(data); }
        else if(Port == 2){ _twoelectronoperatorintegrals = std::any_cast<std::weak_ptr<Eigen::Tensor<double, 4>>>(data); }
        else if(Port == 3){ _electrons = std::any_cast<std::weak_ptr<int>>(data); }
        else if(Port == 4){ _energy0 = std::any_cast<std::weak_ptr<double>>(data); }
    }
    else
    {
        if(Port == 0){_overlapintegrals.reset();}
        else if(Port == 1){_oneelectronoperatorintegrals.reset();}
        else if(Port == 2){_twoelectronoperatorintegrals.reset();}
        else if(Port == 3){_electrons.reset();}
        else if(Port == 4){_energy0.reset();}
    }
}

void RestrictedHartreeFockNode::selectmaxiter()
{
    this->max_iteration_steps = this->selectedmaxiter->text().toInt();
    PropertiesChanged();
}

void RestrictedHartreeFockNode::selectthreshold()
{
    this->energy_threshold = this->selectedthreshold->text().toDouble();
    PropertiesChanged();
}

void RestrictedHartreeFockNode::show_results()
{

    results_dialog = new QWidget;
    QFormLayout* form = new QFormLayout(results_dialog);

    form->addRow(new QLabel("Results for the " + QString::number(orbital_energys.rows()) + " Orbitals"));

    QTableWidget* result_table = new QTableWidget;
    result_table->setRowCount(orbital_energys.rows());
    result_table->setColumnCount(1);
    result_table->setHorizontalHeaderItem(0, new QTableWidgetItem("Absolute Energys"));
    for (int i = 0; i < orbital_energys.rows(); i++)
    {
        result_table->setVerticalHeaderItem(i, new QTableWidgetItem("State " + QString::number(i)));
        result_table->setItem(i, 0, new QTableWidgetItem(QString::number(orbital_energys[i])));
    }
    form->addRow(result_table);
    results_dialog->show();

}