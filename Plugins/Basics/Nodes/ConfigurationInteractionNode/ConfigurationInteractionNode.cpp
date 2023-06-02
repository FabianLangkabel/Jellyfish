#include "ConfigurationInteractionNode.hpp"

ConfigurationInteractionNode::ConfigurationInteractionNode(std::string InpNodeTypeID) : Node()
{   
    NodeTypeID = InpNodeTypeID;
    NodeName = "Configuration Interaction";
    NodeDescription = "Configuration Interaction";
    NodeCategory = "Electronic Structure";
    Plugin = "Basics";
    HasCalulateFunction = true;
}

ConfigurationInteractionNode::~ConfigurationInteractionNode(){}

void ConfigurationInteractionNode::LoadData(json Data, std::string file)
{
    this->excitation_level = Data["excitation_level"];

    if (State == NodeState::Calculated)
    {
        save_load::load(file, this->id, "configurations", &(ci_results.configuration_strings));
        save_load::load(file, this->id, "ci_matrix", &(ci_results.ci_matrix));
        save_load::load(file, this->id, "ci_values", &(ci_results.ci_values));

        _ci_results = std::make_shared<QC::CIResults>(this->ci_results);
    }
}

json ConfigurationInteractionNode::SaveData(std::string file)
{
    if (State == NodeState::Calculated)
    {
        save_load::save(file, this->id, "configurations", ci_results.configuration_strings);
        save_load::save(file, this->id, "ci_matrix", ci_results.ci_matrix);
        save_load::save(file, this->id, "ci_values", ci_results.ci_values);
    }

    json DataJson;
    DataJson["excitation_level"] = this->excitation_level;
    return DataJson;
}

void ConfigurationInteractionNode::calculate()
{
    QC::Configuration_Interaction ci_class;

    auto OneElectronIntegrals = _oneelectronintegrals.lock().get();
    auto TwoElectronIntegrals = _twoelectronintegrals.lock().get();
    auto Electrons = _electrons.lock().get();
    
    ci_class.set_excitation_level(excitation_level);
    ci_class.set_one_electron_integrals_spinorbitals(*OneElectronIntegrals);
    ci_class.set_two_electron_integrals_spinorbitals(*TwoElectronIntegrals);
    ci_class.set_electrons(*Electrons);

    if (!_energyshift.expired())
    {
        auto E0 = _energyshift.lock().get();
        ci_class.set_E0(*E0);
    }

    ci_class.compute();

    ci_results.configuration_strings = ci_class.get_configuration_strings();
    ci_results.ci_matrix = ci_class.get_ci_matrix();
    ci_results.ci_values = ci_class.get_eigenenergys();

    _ci_results = std::make_shared<QC::CIResults>(this->ci_results);
    update_AllPortData();

    State = NodeState::Calculated;
}

void ConfigurationInteractionNode::NodeInspector(QWidget* Inspector)
{
    QVBoxLayout* lay = new QVBoxLayout();
    
    selectedexcitation = new QLineEdit;
    selectedexcitation->setValidator(new QIntValidator(0, 100, this));
    selectedexcitation->setText(QString::number(this->excitation_level));
    connect(selectedexcitation, &QLineEdit::textChanged, this, &ConfigurationInteractionNode::selectexcitation);

    lay->addWidget(new QLabel("Select excitation level"));
    lay->addWidget(selectedexcitation);

    QPushButton* show_results_button = new QPushButton("Show Results");
    connect(show_results_button, &QPushButton::clicked, this, &ConfigurationInteractionNode::show_results);
    lay->addWidget(show_results_button);

    Inspector->setLayout(lay);
}

bool ConfigurationInteractionNode::InPortAllowMultipleConnections(int Port)
{ 
    return false; 
}

unsigned int ConfigurationInteractionNode::Ports(NodePortType PortType)
{
    if(PortType == NodePortType::In)
    {
        return 4;
    }
    else
    {
        return 1;
    }
}

bool ConfigurationInteractionNode::IsCustomPortCaption(NodePortType PortType, int Port)
{ 
    return true; 
}

std::string ConfigurationInteractionNode::CustomPortCaption(NodePortType PortType, int Port)
{ 
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "1e Spin Integrals"; }
        else if(Port == 1) {return "2e Spin Integrals"; }
        else if(Port == 2) {return "Electrons"; }
        else if(Port == 3) {return "Energyshift"; }
    }
    else
    {
        if(Port == 0) {return "CI Results"; }
    }
}

std::string ConfigurationInteractionNode::DataTypeName(NodePortType PortType, int Port)
{
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "Eigen::MatrixXd"; }
        else if(Port == 1) {return "Eigen::Tensor<double, 4>"; }
        else if(Port == 2) {return "int"; }
        else if(Port == 3) {return "double"; }
    }
    else
    {
        if(Port == 0) {return "QC::CIResults"; }
    }
}

std::any ConfigurationInteractionNode::getOutData(int Port) 
{ 
    std::any ret;
    if(Port == 0) { ret = std::make_any<std::weak_ptr<QC::CIResults>>(_ci_results); }
    return ret;
}

void ConfigurationInteractionNode::setInData(int Port, std::any data)
{
    if(data.has_value())
    {
        if(Port == 0){ _oneelectronintegrals = std::any_cast<std::weak_ptr<Eigen::MatrixXd>>(data); }
        else if(Port == 1){ _twoelectronintegrals = std::any_cast<std::weak_ptr<Eigen::Tensor<double, 4>>>(data); }
        else if(Port == 2){ _electrons = std::any_cast<std::weak_ptr<int>>(data); }
        else if(Port == 3){ _energyshift = std::any_cast<std::weak_ptr<double>>(data); }
    }
    else
    {
        if(Port == 0){_oneelectronintegrals.reset();}
        else if(Port == 1){_twoelectronintegrals.reset();}
        else if(Port == 2){_electrons.reset();}
        else if(Port == 3){_energyshift.reset();}
    }
}

void ConfigurationInteractionNode::selectexcitation()
{
    this->excitation_level = this->selectedexcitation->text().toInt();
    PropertiesChanged();
}

void ConfigurationInteractionNode::show_results()
{
    results_dialog = new QWidget;
    QFormLayout* form = new QFormLayout(results_dialog);

    form->addRow(new QLabel("Results for the " + QString::number(ci_results.ci_values.rows()) + " CI-States"));

    QTableWidget* result_table = new QTableWidget;
    result_table->setRowCount(ci_results.ci_values.rows());
    result_table->setColumnCount(2);
    result_table->setHorizontalHeaderItem(0, new QTableWidgetItem("Absolute Energys"));
    result_table->setHorizontalHeaderItem(1, new QTableWidgetItem("Relative Energy"));
    for (int i = 0; i < ci_results.ci_values.rows(); i++)
    {
        result_table->setVerticalHeaderItem(i, new QTableWidgetItem("State " + QString::number(i)));
        result_table->setItem(i, 0, new QTableWidgetItem(QString::number(ci_results.ci_values[i])));
        result_table->setItem(i, 1, new QTableWidgetItem(QString::number(ci_results.ci_values[i] - ci_results.ci_values[0])));
    }
    form->addRow(result_table);
    results_dialog->show();

}