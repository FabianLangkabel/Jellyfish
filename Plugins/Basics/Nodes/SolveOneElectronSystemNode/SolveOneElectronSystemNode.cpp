#include "SolveOneElectronSystemNode.hpp"

SolveOneElectronSystemNode::SolveOneElectronSystemNode(std::string InpNodeTypeID) : Node()
{   
    NodeTypeID = InpNodeTypeID;
    NodeName = "Single-Electron System";
    NodeDescription = "Single-Electron System";
    NodeCategory = "Electronic Structure";
    Plugin = "Basics";
    HasCalulateFunction = true;
}

SolveOneElectronSystemNode::~SolveOneElectronSystemNode(){}

void SolveOneElectronSystemNode::LoadData(json Data, std::string file)
{
    if (State == NodeState::Calculated)
    {
        save_load::load(file, this->id, "c_matrix", &CMatrix);
        save_load::load(file, this->id, "orbital_energys", &OrbitalEnergys);

        _cmatrix = std::make_shared<Eigen::MatrixXd>(this->CMatrix);
        _orbitalenergys = std::make_shared<Eigen::VectorXd>(this->OrbitalEnergys);
    }
}

json SolveOneElectronSystemNode::SaveData(std::string file)
{
    if (State == NodeState::Calculated)
    {
        save_load::save(file, this->id, "c_matrix", CMatrix);
        save_load::save(file, this->id, "orbital_energys", OrbitalEnergys);
    }

    json DataJson;
    return DataJson;
}

void SolveOneElectronSystemNode::calculate()
{
    auto Overlapintegrals = *(_overlapintegrals.lock().get());
    auto OneElectronOperator = *(_oneelectronoperatorintegrals.lock().get());

    QC::One_Electron_System Solver;

    if (!_energy0.expired())
    {
        auto E0 = *(_energy0.lock().get());
        Solver.set_E0(E0);
    }

    Solver.set_overlapmatrix(Overlapintegrals);
    Solver.set_one_electron_integrals(OneElectronOperator);

    
    Solver.compute();

    CMatrix = Solver.get_C_matrix();
    OrbitalEnergys = Solver.get_orbitalenergys();
    

    _cmatrix = std::make_shared<Eigen::MatrixXd>(this->CMatrix);
    _orbitalenergys = std::make_shared<Eigen::VectorXd>(this->OrbitalEnergys);
    update_AllPortData();

    State = NodeState::Calculated;
}

void SolveOneElectronSystemNode::NodeInspector(QWidget* Inspector)
{
    QVBoxLayout* lay = new QVBoxLayout();

    QPushButton* show_results_button = new QPushButton("Show Results");
    connect(show_results_button, &QPushButton::clicked, this, &SolveOneElectronSystemNode::show_results);
    lay->addWidget(show_results_button);

    Inspector->setLayout(lay);
}

bool SolveOneElectronSystemNode::InPortAllowMultipleConnections(int Port)
{ 
    return false; 
}

unsigned int SolveOneElectronSystemNode::Ports(NodePortType PortType)
{
    if(PortType == NodePortType::In)
    {
        return 3;
    }
    else
    {
        return 2;
    }
}

bool SolveOneElectronSystemNode::IsCustomPortCaption(NodePortType PortType, int Port)
{ 
    return true; 
}

std::string SolveOneElectronSystemNode::CustomPortCaption(NodePortType PortType, int Port)
{ 
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "Overlapintegrals"; }
        else if(Port == 1) {return "1e Integrals"; }
        else if(Port == 2) {return "Energyshift"; }
    }
    else
    {
        if(Port == 0) {return "C-Matrix"; }
        else if(Port == 1) {return "Orbital Energys"; }
    }
}

std::string SolveOneElectronSystemNode::DataTypeName(NodePortType PortType, int Port)
{
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "Eigen::MatrixXd"; }
        else if(Port == 1) {return "Eigen::MatrixXd"; }
        else if(Port == 2) {return "double"; }
    }
    else
    {
        if(Port == 0) {return "Eigen::MatrixXd"; }
        else if(Port == 1) {return "Eigen::VectorXd"; }
    }
}

std::any SolveOneElectronSystemNode::getOutData(int Port) 
{ 
    std::any ret;
    if(Port == 0)
    {
        ret = std::make_any<std::weak_ptr<Eigen::MatrixXd>>(_cmatrix);
    }
    else
    {
        ret = std::make_any<std::weak_ptr<Eigen::VectorXd>>(_orbitalenergys);
    }
    return ret;
}

void SolveOneElectronSystemNode::setInData(int Port, std::any data)
{
    if(data.has_value())
    {
        if(Port == 0)
        {
            _overlapintegrals = std::any_cast<std::weak_ptr<Eigen::MatrixXd>>(data);
        }
        if(Port == 1)
        {
            _oneelectronoperatorintegrals = std::any_cast<std::weak_ptr<Eigen::MatrixXd>>(data);
        }
        if(Port == 2)
        {
            _energy0 = std::any_cast<std::weak_ptr<double>>(data);
        }
    }
    else
    {
        if(Port == 0){_overlapintegrals.reset();}
        else if(Port == 1){_oneelectronoperatorintegrals.reset();}
        else if(Port == 2){_energy0.reset();}
    }
}

void SolveOneElectronSystemNode::show_results()
{
    results_dialog = new QWidget;
    QFormLayout* form = new QFormLayout(results_dialog);

    form->addRow(new QLabel("Results for the " + QString::number(OrbitalEnergys.rows()) + " Orbital-Energys"));

    QTableWidget* result_table = new QTableWidget;
    result_table->setRowCount(OrbitalEnergys.rows());
    result_table->setColumnCount(2);
    result_table->setHorizontalHeaderItem(0, new QTableWidgetItem("Absolute Energys"));
    result_table->setHorizontalHeaderItem(1, new QTableWidgetItem("Relative Energy"));
    for (int i = 0; i < OrbitalEnergys.rows(); i++)
    {
        result_table->setVerticalHeaderItem(i, new QTableWidgetItem("Orbital " + QString::number(i)));
        result_table->setItem(i, 0, new QTableWidgetItem(QString::number(OrbitalEnergys[i])));
        result_table->setItem(i, 1, new QTableWidgetItem(QString::number(OrbitalEnergys[i] - OrbitalEnergys[0])));
    }
    form->addRow(result_table);
    results_dialog->show();

};