#include "CreateEvenTemperedBasissetNode.hpp"

CreateEvenTemperedBasissetNode::CreateEvenTemperedBasissetNode(std::string InpNodeTypeID) : Node()
{   
    NodeTypeID = InpNodeTypeID;
    NodeName = "Even Tempered Basisset";
    NodeDescription = "Even Tempered Basisset";
    NodeCategory = "Bassisset";
    Plugin = "Basics";
    HasCalulateFunction = true;
}

CreateEvenTemperedBasissetNode::~CreateEvenTemperedBasissetNode(){}

void CreateEvenTemperedBasissetNode::LoadData(json Data, std::string file)
{
    json basisfunctionsjson = Data["basisfunctions"];

    for (int i = 0; i < (int)basisfunctionsjson["number"]; i++)
    {
        QC::Gaussian_Basisfunction func;
        json funcjson =  basisfunctionsjson[std::to_string(i)];
        func.set_center(funcjson[0], funcjson[1], funcjson[2]);
        func.set_total_angular_moment(funcjson[3]);
        func.add_parameterdouble(1, funcjson[4]);
        this->basisset.add_Basisfunction(func);
    }
    this->basisset.set_basisset_type(basisfunctionsjson["basissettype"]);

    if (State == NodeState::Calculated)
    {
        //save_load::load(file, this->id, "basisset", &basisset); //Alternative: Save Basisset as extra file

        _basisset = std::make_shared<QC::Gaussian_Basisset>(this->basisset);
    }
}

json CreateEvenTemperedBasissetNode::SaveData(std::string file)
{
    if (State == NodeState::Calculated)
    {
        //save_load::save(file, this->id, "basisset", basisset); //Alternative: Save Basisset as extra file
    }

    json DataJson;
    json basisfunctionsjson;
    for (int i = 0; i < this->basisset.get_Basisfunctionnumber_angular_compact(); i++)
    {
        std::vector<double> basisfunction;
        QC::Gaussian_Basisfunction basisfunctionobject = this->basisset.get_Basisfunction(i);

        basisfunction.push_back(basisfunctionobject.get_center_x());
        basisfunction.push_back(basisfunctionobject.get_center_y());
        basisfunction.push_back(basisfunctionobject.get_center_z());
        basisfunction.push_back(basisfunctionobject.get_total_angular_moment());
        basisfunction.push_back(basisfunctionobject.get_exponent(0));

        basisfunctionsjson[std::to_string(i)] = basisfunction;
    }
    basisfunctionsjson["number"] = this->basisset.get_Basisfunctionnumber_angular_compact();
    basisfunctionsjson["basissettype"] = this->basisset.get_basisset_type();
    DataJson["basisfunctions"] = basisfunctionsjson;
    return DataJson;
}

void CreateEvenTemperedBasissetNode::calculate()
{
    _basisset = std::make_shared<QC::Gaussian_Basisset>(this->basisset);
    update_AllPortData();
    
    State = NodeState::Calculated;
}

void CreateEvenTemperedBasissetNode::NodeInspector(QWidget* Inspector)
{
    QVBoxLayout* lay = new QVBoxLayout();

    basiset_type_box = new QComboBox;
    basiset_type_box->addItem("Spherical");
    basiset_type_box->addItem("Cartesian");
    basiset_type_box->setCurrentIndex(this->basisset.get_basisset_type());
    connect(basiset_type_box, &QComboBox::currentTextChanged, this, &CreateEvenTemperedBasissetNode::select_basiset_type);


    eventemperedview = new QTreeView;
    QStandardItemModel* standardModel = new QStandardItemModel(this);
    eventemperedviewrootnode = standardModel->invisibleRootItem();
    eventemperedview->setModel(standardModel);
    //gausspotentialview->expandAll();

    for (int i = 0; i < this->basisset.get_Basisfunctionnumber_angular_compact(); i++)
    {
        QStandardItem* basisfunction = new QStandardItem("Basisfunction " + QString::number(i));
        QStandardItem* basisfunction_x = new QStandardItem("x = " + QString::number(this->basisset.get_Basisfunction(i).get_center_x()));
        QStandardItem* basisfunction_y = new QStandardItem("y = " + QString::number(this->basisset.get_Basisfunction(i).get_center_y()));
        QStandardItem* basisfunction_z = new QStandardItem("z = " + QString::number(this->basisset.get_Basisfunction(i).get_center_z()));
        QStandardItem* basisfunction_angular = new QStandardItem("Angular Moment = " + QString::number(this->basisset.get_Basisfunction(i).get_total_angular_moment()));
        QStandardItem* basisfunction_exponent = new QStandardItem("Exponent = " + QString::number(this->basisset.get_Basisfunction(i).get_exponent(0)));
        eventemperedviewrootnode->appendRow(basisfunction);
        basisfunction->appendRow(basisfunction_x);
        basisfunction->appendRow(basisfunction_y);
        basisfunction->appendRow(basisfunction_z);
        basisfunction->appendRow(basisfunction_angular);
        basisfunction->appendRow(basisfunction_exponent);
    }


    QPushButton* add_eventempered_button = new QPushButton("Add Even Tempered Basisfunctions");
    connect(add_eventempered_button, &QPushButton::clicked, this, &CreateEvenTemperedBasissetNode::add_eventempered);


    lay->addWidget(new QLabel("Select Basissettype"));
    lay->addWidget(basiset_type_box);

    auto line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    lay->addWidget(line);

    lay->addWidget(new QLabel("Add/Delete Even Tempered Basisfunctions"));
    lay->addWidget(eventemperedview);
    lay->addWidget(add_eventempered_button);

    Inspector->setLayout(lay);
}

bool CreateEvenTemperedBasissetNode::InPortAllowMultipleConnections(int Port)
{ 
    return false; 
}

unsigned int CreateEvenTemperedBasissetNode::Ports(NodePortType PortType)
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

bool CreateEvenTemperedBasissetNode::IsCustomPortCaption(NodePortType PortType, int Port)
{ 
    return true; 
}

std::string CreateEvenTemperedBasissetNode::CustomPortCaption(NodePortType PortType, int Port)
{ 
    if(PortType == NodePortType::In)
    {
    }
    else
    {
        if(Port == 0) {return "Basisset"; }
    }
}

std::string CreateEvenTemperedBasissetNode::DataTypeName(NodePortType PortType, int Port)
{
    if(PortType == NodePortType::In)
    {
    }
    else
    {
        if(Port == 0) {return "QC::Gaussian_Basisset"; }
    }
}

std::any CreateEvenTemperedBasissetNode::getOutData(int Port) 
{ 
    std::any ret;
    if(Port == 0)
    {
        ret = std::make_any<std::weak_ptr<QC::Gaussian_Basisset>>(_basisset);
    }
    return ret;
}

void CreateEvenTemperedBasissetNode::setInData(int Port, std::any data)
{
}

void CreateEvenTemperedBasissetNode::add_eventempered()
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

    QString label_angular = QString("Angular Moment");
    QLineEdit* lineedit_angular = new QLineEdit(&dialog);
    lineedit_angular->setValidator(new QIntValidator(0, 7, this));
    form.addRow(label_angular, lineedit_angular);

    QString label_exp0 = QString("Exponent 0");
    QLineEdit* lineedit_exp0 = new QLineEdit(&dialog);
    lineedit_exp0->setValidator(new QDoubleValidator(0, 100, 10, this));
    form.addRow(label_exp0, lineedit_exp0);

    QString label_basis = QString("Basis");
    QLineEdit* lineedit_basis = new QLineEdit(&dialog);
    lineedit_basis->setValidator(new QDoubleValidator(0, 100, 10, this));
    form.addRow(label_basis, lineedit_basis);

    QString label_number = QString("Number of Functions");
    QLineEdit* lineedit_number = new QLineEdit(&dialog);
    lineedit_number->setValidator(new QIntValidator(1, 100, this));
    form.addRow(label_number, lineedit_number);


    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    if (dialog.exec() == QDialog::Accepted) {
        for (int i = 0; i < lineedit_number->text().toInt(); i++)
        {
            QC::Gaussian_Basisfunction new_Basisfunction;
            new_Basisfunction.set_center(lineedit_x_pos->text().toDouble(), lineedit_y_pos->text().toDouble(), lineedit_z_pos->text().toDouble());
            new_Basisfunction.set_total_angular_moment(lineedit_angular->text().toInt());
            double exponent = lineedit_exp0->text().toDouble() * pow(lineedit_basis->text().toDouble(), i);
            new_Basisfunction.add_parameterdouble(1, exponent);
            basisset.add_Basisfunction(new_Basisfunction);
            QStandardItem* basisfunc = new QStandardItem("Basisfunction " + QString::number(this->basisset.get_Basisfunctionnumber_angular_compact() - 1));
            eventemperedviewrootnode->appendRow(basisfunc);
            basisfunc->appendRow(new QStandardItem("x = " + QString::number(lineedit_x_pos->text().toDouble())));
            basisfunc->appendRow(new QStandardItem("y = " + QString::number(lineedit_y_pos->text().toDouble())));
            basisfunc->appendRow(new QStandardItem("z = " + QString::number(lineedit_z_pos->text().toDouble())));
            basisfunc->appendRow(new QStandardItem("Angular Moment = " + QString::number(lineedit_angular->text().toInt())));
            basisfunc->appendRow(new QStandardItem("Exponent = " + QString::number(exponent)));
        }
    }
    PropertiesChanged();
}

void CreateEvenTemperedBasissetNode::select_basiset_type()
{
    this->basisset.set_basisset_type(this->basiset_type_box->currentIndex());
    PropertiesChanged();
}