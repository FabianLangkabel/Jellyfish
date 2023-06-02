#include "CreateGaussPotentialNode.hpp"

CreateGaussPotentialNode::CreateGaussPotentialNode(std::string InpNodeTypeID) : Node()
{   
    NodeTypeID = InpNodeTypeID;
    NodeName = "Gauss Potential";
    NodeDescription = "Gauss Potential";
    NodeCategory = "Potentials";
    Plugin = "Basics";
    HasCalulateFunction = true;
}

CreateGaussPotentialNode::~CreateGaussPotentialNode(){}

void CreateGaussPotentialNode::LoadData(json Data, std::string file)
{
    json gausspotentialsjson = Data["gausspotentials"];
    for (int i = 0; i < (int)gausspotentialsjson["number"]; i++)
    {
        QC::Potential_Gaussian gauss;
        json gaussjson = gausspotentialsjson[std::to_string(i)];
        gauss.set_center(gaussjson[0], gaussjson[1], gaussjson[2]);
        gauss.set_coefficient(gaussjson[3]);
        gauss.set_exponent(gaussjson[4]);
        this->gausspotentials.push_back(gauss);
    }

    if (State == NodeState::Calculated)
    {
        //save_load::load(file, this->id, "gausspotentials", &gausspotentials);
        _gausspotentials = std::make_shared<std::vector<QC::Potential_Gaussian>>(this->gausspotentials);
    }
}

json CreateGaussPotentialNode::SaveData(std::string file)
{
    if (State == NodeState::Calculated)
    {
        //save_load::save(file, this->id, "gausspotentials", gausspotentials);
    }

    json DataJson;
    json gausspotentialsjson;
    int numbergausspotentials = this->gausspotentials.size();
    gausspotentialsjson["number"] = numbergausspotentials;
    for (int i = 0; i < numbergausspotentials; i++)
    {
        std::vector<double> gausspotential;
        QC::Potential_Gaussian gausspotentialobject = this->gausspotentials[i];

        gausspotential.push_back(gausspotentialobject.get_center_x());
        gausspotential.push_back(gausspotentialobject.get_center_y());
        gausspotential.push_back(gausspotentialobject.get_center_z());
        gausspotential.push_back(gausspotentialobject.get_coefficient());
        gausspotential.push_back(gausspotentialobject.get_exponent());

        gausspotentialsjson[std::to_string(i)] = gausspotential;
    }
    DataJson["gausspotentials"] = gausspotentialsjson;
    return DataJson;
}

void CreateGaussPotentialNode::calculate()
{
    _gausspotentials = std::make_shared<std::vector<QC::Potential_Gaussian>>(this->gausspotentials);
    update_AllPortData();
    
    State = NodeState::Calculated;
}

void CreateGaussPotentialNode::NodeInspector(QWidget* Inspector)
{
    QVBoxLayout* lay = new QVBoxLayout();

    gausspotentialview = new QTreeView;
    QStandardItemModel* standardModel = new QStandardItemModel();
    gausspotentialviewrootnode = standardModel->invisibleRootItem();
    gausspotentialview->setModel(standardModel);
    //gausspotentialview->expandAll();

    for (int i = 0; i < this->gausspotentials.size(); i++)
    {
        QStandardItem* potential = new QStandardItem("Gauss Potential " + QString::number(i));
        QStandardItem* potential_x = new QStandardItem("x = " + QString::number(this->gausspotentials[i].get_center_x()));
        QStandardItem* potential_y = new QStandardItem("y = " + QString::number(this->gausspotentials[i].get_center_y()));
        QStandardItem* potential_z = new QStandardItem("z = " + QString::number(this->gausspotentials[i].get_center_z()));
        QStandardItem* potential_depth = new QStandardItem("depth = " + QString::number(this->gausspotentials[i].get_coefficient()));
        QStandardItem* potential_width = new QStandardItem("width = " + QString::number(this->gausspotentials[i].get_exponent()));
        gausspotentialviewrootnode->appendRow(potential);
        potential->appendRow(potential_x);
        potential->appendRow(potential_y);
        potential->appendRow(potential_z);
        potential->appendRow(potential_depth);
        potential->appendRow(potential_width);
    }


    QPushButton* add_gauss_potential_button = new QPushButton("Add Gauss Potential");
    connect(add_gauss_potential_button, &QPushButton::clicked, this, &CreateGaussPotentialNode::add_gauss_potential);

    lay->addWidget(new QLabel("Add/Delete Gauss Potentials"));
    lay->addWidget(gausspotentialview);
    lay->addWidget(add_gauss_potential_button);

    Inspector->setLayout(lay);
}

bool CreateGaussPotentialNode::InPortAllowMultipleConnections(int Port)
{ 
    return false; 
}

unsigned int CreateGaussPotentialNode::Ports(NodePortType PortType)
{
    if(PortType == NodePortType::In)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

bool CreateGaussPotentialNode::IsCustomPortCaption(NodePortType PortType, int Port)
{ 
    return true; 
}

std::string CreateGaussPotentialNode::CustomPortCaption(NodePortType PortType, int Port)
{ 
    return "GaussPotentials"; 
}

std::string CreateGaussPotentialNode::DataTypeName(NodePortType PortType, int Port)
{
    return "std::vector<QC::Potential_Gaussian>";
}

std::any CreateGaussPotentialNode::getOutData(int Port) 
{ 
    std::any ret;
    ret = std::make_any<std::weak_ptr<std::vector<QC::Potential_Gaussian>>>(_gausspotentials);
    return ret;
}

void CreateGaussPotentialNode::setInData(int Port, std::any data)
{
    if(data.has_value())
    {
    }
}

void CreateGaussPotentialNode::add_gauss_potential()
{
    QDialog dialog;
    QFormLayout form(&dialog);
    form.addRow(new QLabel("Add Gauss Potential"));

    QString label_x_pos = QString("x-Position");
    QLineEdit* lineedit_x_pos = new QLineEdit(&dialog);
    lineedit_x_pos->setValidator(new QDoubleValidator(-100, 100, 10, this));
    form.addRow(label_x_pos, lineedit_x_pos);

    QString label_y_pos = QString("y-Position");
    QLineEdit* lineedit_y_pos = new QLineEdit(&dialog);
    lineedit_y_pos->setValidator(new QDoubleValidator(-100, 100, 10, this));
    form.addRow(label_y_pos, lineedit_y_pos);

    QString label_z_pos = QString("z-Position");
    QLineEdit* lineedit_z_pos = new QLineEdit(&dialog);
    lineedit_z_pos->setValidator(new QDoubleValidator(-100, 100, 10, this));
    form.addRow(label_z_pos, lineedit_z_pos);

    QString label_depth = QString("Depth");
    QLineEdit* lineedit_depth = new QLineEdit(&dialog);
    lineedit_depth->setValidator(new QDoubleValidator(-100, 100, 10, this));
    form.addRow(label_depth, lineedit_depth);

    QString label_width = QString("Width");
    QLineEdit* lineedit_width = new QLineEdit(&dialog);
    lineedit_width->setValidator(new QDoubleValidator(-100, 100, 10, this));
    form.addRow(label_width, lineedit_width);
    

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    if (dialog.exec() == QDialog::Accepted) {
        QC::Potential_Gaussian new_gaussian;
        new_gaussian.set_center(lineedit_x_pos->text().toDouble(), lineedit_y_pos->text().toDouble(), lineedit_z_pos->text().toDouble());
        new_gaussian.set_coefficient(lineedit_depth->text().toDouble());
        new_gaussian.set_exponent(lineedit_width->text().toDouble());
        this->gausspotentials.push_back(new_gaussian);

        QStandardItem* potential = new QStandardItem("Gauss Potential " + QString::number(this->gausspotentials.size() - 1));
        QStandardItem* potential_x = new QStandardItem("x = " + QString::number(new_gaussian.get_center_x()));
        QStandardItem* potential_y = new QStandardItem("y = " + QString::number(new_gaussian.get_center_y()));
        QStandardItem* potential_z = new QStandardItem("z = " + QString::number(new_gaussian.get_center_z()));
        QStandardItem* potential_depth = new QStandardItem("depth = " + QString::number(new_gaussian.get_coefficient()));
        QStandardItem* potential_width = new QStandardItem("width = " + QString::number(new_gaussian.get_exponent()));
        gausspotentialviewrootnode->appendRow(potential);
        potential->appendRow(potential_x);
        potential->appendRow(potential_y);
        potential->appendRow(potential_z);
        potential->appendRow(potential_depth);
        potential->appendRow(potential_width);
    }
    PropertiesChanged();
};