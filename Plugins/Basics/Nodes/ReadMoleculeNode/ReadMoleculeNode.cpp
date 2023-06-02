#include "ReadMoleculeNode.hpp"

ReadMoleculeNode::ReadMoleculeNode(std::string InpNodeTypeID) : Node()
{   
    NodeTypeID = InpNodeTypeID;
    NodeName = "Read Molecule";
    NodeDescription = "Read Molecule";
    NodeCategory = "Input";
    Plugin = "Basics";
    HasCalulateFunction = true;
}

ReadMoleculeNode::~ReadMoleculeNode(){}

void ReadMoleculeNode::LoadData(json Data, std::string file)
{
    this->moleculefilename = Data["moleculefilename"];
    this->basissetname = Data["basissetname"];
    this->charge = Data["charge"];

    if (State == NodeState::Calculated)
    {
        save_load::load(file, this->id, "electrons", &electrons);
        save_load::load(file, this->id, "pointcharges", &pointcharges);
        save_load::load(file, this->id, "basisset", &basisset);

        _electrons = std::make_shared<int>(this->electrons);
        _pointcharges = std::make_shared<std::vector<QC::Pointcharge>>(this->pointcharges);
        _basisset = std::make_shared<QC::Gaussian_Basisset>(this->basisset);
    }
}

json ReadMoleculeNode::SaveData(std::string file)
{
    if (State == NodeState::Calculated)
    {
        save_load::save(file, this->id, "electrons", electrons);
        save_load::save(file, this->id, "pointcharges", pointcharges);
        save_load::save(file, this->id, "basisset", basisset);
    }

    json DataJson;
    DataJson["moleculefilename"] = this->moleculefilename;
    DataJson["basissetname"] = this->basissetname;
    DataJson["charge"] = this->charge;
    return DataJson;
}

void ReadMoleculeNode::calculate()
{
    QC::Read_Molecule Read_Molecule_Class;
    Read_Molecule_Class.set_basissetfile(this->basissetname);
    Read_Molecule_Class.set_moleculefile(this->moleculefilename);
    Read_Molecule_Class.set_charge(this->charge);

    Read_Molecule_Class.compute();

    this->electrons = Read_Molecule_Class.get_electrons();
    this->pointcharges = Read_Molecule_Class.get_pointcharges();
    this->basisset = Read_Molecule_Class.get_basisset();

    _electrons = std::make_shared<int>(this->electrons);
    _pointcharges = std::make_shared<std::vector<QC::Pointcharge>>(this->pointcharges);
    _basisset = std::make_shared<QC::Gaussian_Basisset>(this->basisset);
    update_AllPortData();

    State = NodeState::Calculated;
}

bool ReadMoleculeNode::InPortAllowMultipleConnections(int Port)
{ 
    return false; 
}

unsigned int ReadMoleculeNode::Ports(NodePortType PortType)
{
    if(PortType == NodePortType::In)
    {
        return 0;
    }
    else
    {
        return 3;
    }
}

bool ReadMoleculeNode::IsCustomPortCaption(NodePortType PortType, int Port)
{ 
    return true; 
}

std::string ReadMoleculeNode::CustomPortCaption(NodePortType PortType, int Port)
{ 
    if(PortType == NodePortType::In)
    {
    }
    else
    {
        if(Port == 0) {return "Electrons"; }
        else if(Port == 1) {return "Pointcharges"; }
        else if(Port == 2) {return "Basisset"; }
    }
}

std::string ReadMoleculeNode::DataTypeName(NodePortType PortType, int Port)
{
    if(PortType == NodePortType::In)
    {
    }
    else
    {
        if(Port == 0) {return "int"; }
        else if(Port == 1) {return "std::vector<QC::Pointcharge>"; }
        else if(Port == 2) {return "QC::Gaussian_Basisset"; }
    }
}

std::any ReadMoleculeNode::getOutData(int Port) 
{ 
    std::any ret;
    if(Port == 0){ ret = std::make_any<std::weak_ptr<int>>(_electrons); }
    else if(Port == 1){ ret = std::make_any<std::weak_ptr<std::vector<QC::Pointcharge>>>(_pointcharges); }
    else if(Port == 2){ ret = std::make_any<std::weak_ptr<QC::Gaussian_Basisset>>(_basisset); }
    return ret;
}

void ReadMoleculeNode::setInData(int Port, std::any data)
{
}

void ReadMoleculeNode::NodeInspector(QWidget* Inspector)
{
    QVBoxLayout* lay = new QVBoxLayout();


    QPushButton* Select_Molecule = new QPushButton("Select");
    connect(Select_Molecule, &QPushButton::clicked, this, &ReadMoleculeNode::selectmolecule_dialog);

    selectedmolecule_label = new QLabel;
    std::string filename = this->moleculefilename;
    size_t i = filename.rfind("/", filename.length());
    filename = filename.substr(i + 1, filename.length() - i);
    selectedmolecule_label->setText("Selected: " + QString::fromStdString(filename));

    auto line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    Select_Basisset = new QComboBox;
    QC::Read_Molecule Read_Molecule_Class;
    std::vector<std::string> Basissetfiles = Read_Molecule_Class.get_basissetfiles();
    for (int i = 0; i < Basissetfiles.size(); i++)
    {
        Select_Basisset->addItem(QString::fromStdString(Basissetfiles[i]));
    }
    connect(Select_Basisset, &QComboBox::currentTextChanged, this, &ReadMoleculeNode::selectbasissetfile);

    selectedbasis_label = new QLabel;
    std::string basisname = this->basissetname;
    selectedbasis_label->setText("Selected: " + QString::fromStdString(basisname));

    auto line2 = new QFrame;
    line2->setFrameShape(QFrame::HLine);
    line2->setFrameShadow(QFrame::Sunken);

    selectedcharge = new QLineEdit;
    selectedcharge->setValidator(new QIntValidator(-100, 100, this));
    selectedcharge->setText(QString::number(this->charge));
    connect(selectedcharge, &QLineEdit::textChanged, this, &ReadMoleculeNode::selectcharge);

    lay->addWidget(new QLabel("Select Molecule Input-File (.xyz format)"));
    lay->addWidget(selectedmolecule_label);
    lay->addWidget(Select_Molecule);
    lay->addWidget(line);
    lay->addWidget(new QLabel("Select Basisset"));
    lay->addWidget(selectedbasis_label);
    lay->addWidget(Select_Basisset);
    lay->addWidget(line2);
    lay->addWidget(new QLabel("Set Charge"));
    lay->addWidget(selectedcharge);

    Inspector->setLayout(lay);
}

void ReadMoleculeNode::selectmolecule_dialog()
{
    this->moleculefilename = QFileDialog::getOpenFileName(new QDialog, tr("Open Molecule File"), "", tr("Molecule Files (*.xyz)")).toStdString();
    std::string filename = this->moleculefilename;
    size_t i = filename.rfind("/", filename.length());
    filename = filename.substr(i + 1, filename.length() - i);
    selectedmolecule_label->setText("Selected: " + QString::fromStdString(filename));
    PropertiesChanged();
}

void ReadMoleculeNode::selectbasissetfile()
{
    this->basissetname = this->Select_Basisset->currentText().toStdString();
    std::string basisname = this->basissetname;
    selectedbasis_label->setText("Selected: " + QString::fromStdString(basisname));
    PropertiesChanged();
}

void ReadMoleculeNode::selectcharge()
{
    this->charge = this->selectedcharge->text().toInt();
    PropertiesChanged();
}