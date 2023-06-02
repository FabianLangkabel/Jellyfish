#include "ReadOrcaNode.hpp"

ReadOrcaNode::ReadOrcaNode(std::string InpNodeTypeID) : Node()
{   
    NodeTypeID = InpNodeTypeID;
    NodeName = "Read Orca Output";
    NodeDescription = "Read Orca Output";
    NodeCategory = "Read Output";
    Plugin = "OrcaInterface";
    HasCalulateFunction = true;
}

ReadOrcaNode::~ReadOrcaNode(){}

void ReadOrcaNode::LoadData(json Data, std::string file)
{
    this->OrcaFilePath = Data["orcafilepath"];
    this->has_coordinates_section = Data["has_coordinates_section"];
    this->has_basisset_section = Data["has_basisset_section"];
    this->has_SCF_energy_section = Data["has_SCF_energy_section"];
    this->has_orbital_energies_section = Data["has_orbital_energies_section"];
    this->has_molecular_orbitals_section = Data["has_molecular_orbitals_section"];
    this->has_tddft_excited_states_section = Data["has_tddft_excited_states_section"];

    if (State == NodeState::Calculated)
    {
        if (has_coordinates_section)
        {
            save_load::load(file, this->id, "electrons", &electrons);
            _electrons = std::make_shared<int>(electrons);
            save_load::load(file, this->id, "pointcharges", &pointcharges);
            _pointcharges = std::make_shared<std::vector<QC::Pointcharge>>(pointcharges);
        }
        if (has_basisset_section)
        {
            save_load::load(file, this->id, "basisset", &basisset);
            _basisset = std::make_shared<QC::Gaussian_Basisset>(basisset);
        }
        if (has_molecular_orbitals_section)
        {
            save_load::load(file, this->id, "scf_mo_matrix", &SCF_MO_Matrix);
            _hfcmatrix = std::make_shared<Eigen::MatrixXd>(SCF_MO_Matrix);
        }
        if (has_SCF_energy_section)
        {
            save_load::load(file, this->id, "scf_energy", &scf_energy);
            _hfenergy = std::make_shared<double>(scf_energy);
        }
        if (has_orbital_energies_section)
        {
            save_load::load(file, this->id, "orbital_energys", &orbital_energys);
            _orbitalenergys = std::make_shared<Eigen::VectorXd>(orbital_energys);
        }
        if (has_tddft_excited_states_section)
        {
            save_load::load(file, this->id, "configurations",  &(ci_results.configuration_strings));
            save_load::load(file, this->id, "ci_matrix", &(ci_results.ci_matrix));
            save_load::load(file, this->id, "ci_energys", &(ci_results.ci_values));
            _ci_results = std::make_shared<QC::CIResults>(this->ci_results);
        }
    }
}

json ReadOrcaNode::SaveData(std::string file)
{
    if (State == NodeState::Calculated)
    {
        if(has_coordinates_section)
        {
            save_load::save(file, this->id, "electrons", electrons);
            save_load::save(file, this->id, "pointcharges", pointcharges);
        }
        if (has_basisset_section)
        {
            save_load::save(file, this->id, "basisset", basisset);
        }
        if (has_molecular_orbitals_section)
        {
            save_load::save(file, this->id, "scf_mo_matrix", SCF_MO_Matrix);
        }
        if(has_SCF_energy_section)
        {
            save_load::save(file, this->id, "scf_energy", scf_energy);
        }
        if(has_orbital_energies_section)
        {
            save_load::save(file, this->id, "orbital_energys", orbital_energys);
        }
        if(has_tddft_excited_states_section)
        {
            save_load::save(file, this->id, "configurations", ci_results.configuration_strings);
            save_load::save(file, this->id, "ci_matrix", ci_results.ci_matrix);
            save_load::save(file, this->id, "ci_energys", ci_results.ci_values);
        }
    }

    json DataJson;
    DataJson["orcafilepath"] = this->OrcaFilePath;
    DataJson["has_coordinates_section"] = this->has_coordinates_section;
    DataJson["has_basisset_section"] = this->has_basisset_section;
    DataJson["has_SCF_energy_section"] = this->has_SCF_energy_section;
    DataJson["has_orbital_energies_section"] = this->has_orbital_energies_section;
    DataJson["has_molecular_orbitals_section"] = this->has_molecular_orbitals_section;
    DataJson["has_tddft_excited_states_section"] = this->has_tddft_excited_states_section;

    return DataJson;
}

void ReadOrcaNode::calculate()
{
    QC::ReadOrcaFile FileReader;

    electrons = 0;
    pointcharges.clear();

    FileReader.set_OrcaFilePath(OrcaFilePath);
    FileReader.ReadFile();

    has_coordinates_section = FileReader.file_has_coordinates_section();
    has_basisset_section = FileReader.file_has_basisset_section();
    has_SCF_energy_section = FileReader.file_has_SCF_energy_section();
    has_orbital_energies_section = FileReader.file_has_orbital_energies_section();
    has_molecular_orbitals_section = FileReader.file_has_molecular_orbitals_section();
    has_tddft_excited_states_section = FileReader.file_has_tddft_excited_states_section();

    if (has_coordinates_section){ 
        electrons = FileReader.get_electrons();
        _electrons = std::make_shared<int>(electrons);

        pointcharges = FileReader.get_pointcharges();
        _pointcharges = std::make_shared<std::vector<QC::Pointcharge>>(pointcharges);
    }

    if (has_basisset_section) {
        basisset = FileReader.get_basisset();
        _basisset = std::make_shared<QC::Gaussian_Basisset>(basisset);
    }

    if (has_molecular_orbitals_section) {
        SCF_MO_Matrix = FileReader.get_SCF_MO_Matrix();
        _hfcmatrix = std::make_shared<Eigen::MatrixXd>(SCF_MO_Matrix);
    }

    if (has_SCF_energy_section) {
        scf_energy = FileReader.get_scf_energy();
        _hfenergy = std::make_shared<double>(scf_energy);
    }

    if (has_orbital_energies_section) {
        orbital_energys = FileReader.get_orbital_energys();
        _orbitalenergys = std::make_shared<Eigen::VectorXd>(orbital_energys);
    }

    if (has_tddft_excited_states_section) {
        ci_results.configuration_strings = FileReader.get_CI_strings();
        ci_results.ci_matrix = FileReader.get_CI_Matrix();
        ci_results.ci_values = FileReader.get_CI_Energys();

        _ci_results = std::make_shared<QC::CIResults>(this->ci_results);
    }

    update_AllPortData();
    State = NodeState::Calculated;
}

void ReadOrcaNode::NodeInspector(QWidget* Inspector)
{
    QVBoxLayout* lay = new QVBoxLayout();

    QPushButton* Select_OrcaFile = new QPushButton("Select");
    connect(Select_OrcaFile, &QPushButton::clicked, this, &ReadOrcaNode::selectedorcafile_dialog);

    selectedorcafile_label = new QLabel;
    std::string filename = OrcaFilePath;
    size_t i = filename.rfind("/", filename.length());
    filename = filename.substr(i + 1, filename.length() - i);
    selectedorcafile_label->setText("Selected: " + QString::fromStdString(filename));


    lay->addWidget(new QLabel("Select Orca-File"));
    lay->addWidget(selectedorcafile_label);
    lay->addWidget(Select_OrcaFile);

    if (has_tddft_excited_states_section)
    {
        QPushButton* show_ci_energys = new QPushButton("Show CI-Energys");
        connect(show_ci_energys, &QPushButton::clicked, this, &ReadOrcaNode::show_ci_energys);
        lay->addWidget(show_ci_energys);
    }

    Inspector->setLayout(lay);
}

bool ReadOrcaNode::InPortAllowMultipleConnections(int Port)
{ 
    return false; 
}

unsigned int ReadOrcaNode::Ports(NodePortType PortType)
{
    if(PortType == NodePortType::In)
    {
        return 0;
    }
    else
    {
        return 7;
    }
}

bool ReadOrcaNode::IsCustomPortCaption(NodePortType PortType, int Port)
{ 
    return true; 
}

std::string ReadOrcaNode::CustomPortCaption(NodePortType PortType, int Port)
{ 
    if(PortType == NodePortType::In)
    {
    }
    else
    {
        if(Port == 0) {return "Electrons"; }
        else if(Port == 1) {return "Pointcharges"; }
        else if(Port == 2) {return "Basisset"; }
        else if(Port == 3) {return "C-Matrix"; }
        else if(Port == 4) {return "HF Energy"; }
        else if(Port == 5) {return "Orbital Energys"; }
        else if(Port == 6) {return "CIResults"; }
    }
}

std::string ReadOrcaNode::DataTypeName(NodePortType PortType, int Port)
{
    if(PortType == NodePortType::In)
    {
    }
    else
    {
        if(Port == 0) {return "int"; }
        else if(Port == 1) {return "std::vector<QC::Pointcharge>"; }
        else if(Port == 2) {return "QC::Gaussian_Basisset"; }
        else if(Port == 3) {return "Eigen::MatrixXd"; }
        else if(Port == 4) {return "double"; }
        else if(Port == 5) {return "Eigen::VectorXd"; }
        else if(Port == 6) {return "QC::CIResults"; }
    }
}

std::any ReadOrcaNode::getOutData(int Port) 
{ 
    std::any ret;
    if(Port == 0) { ret = std::make_any<std::weak_ptr<int>>(); }
    else if(Port == 1) { ret = std::make_any<std::weak_ptr<std::vector<QC::Pointcharge>>>(_pointcharges); }
    else if(Port == 2) { ret = std::make_any<std::weak_ptr<QC::Gaussian_Basisset>>(_basisset); }
    else if(Port == 3) { ret = std::make_any<std::weak_ptr<Eigen::MatrixXd>>(_hfcmatrix); }
    else if(Port == 4) { ret = std::make_any<std::weak_ptr<double>>(_hfenergy); }
    else if(Port == 5) { ret = std::make_any<std::weak_ptr<Eigen::VectorXd>>(_orbitalenergys); }
    else if(Port == 6) { ret = std::make_any<std::weak_ptr<QC::CIResults>>(_ci_results); }
    return ret;
}

void ReadOrcaNode::setInData(int Port, std::any data)
{
    if(data.has_value())
    {
    }
    else
    {
    }
}

void ReadOrcaNode::selectedorcafile_dialog()
{
    OrcaFilePath = QFileDialog::getOpenFileName(new QDialog, tr("Open Molecule File"), "", tr("Orca File (*.log)")).toStdString();
    std::string filename = OrcaFilePath;
    size_t i = filename.rfind("/", filename.length());
    filename = filename.substr(i + 1, filename.length() - i);
    selectedorcafile_label->setText("Selected: " + QString::fromStdString(filename));
    PropertiesChanged();
}


void ReadOrcaNode::show_ci_energys()
{
    if (has_orbital_energies_section)
    {
        show_ci_energys_dialog = new QWidget;
        QFormLayout* form = new QFormLayout(show_ci_energys_dialog);

        form->addRow(new QLabel("Results for the " +  QString::number(ci_results.ci_values.rows()) + " CI-States"));

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
        show_ci_energys_dialog->show();
    }
}