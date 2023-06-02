#include "SpartialCAPNode.hpp"

SpartialCAPNode::SpartialCAPNode(std::string InpNodeTypeID) : Node()
{   
    NodeTypeID = InpNodeTypeID;
    NodeName = "Spartial CAP";
    NodeDescription = "Spartial CAP";
    NodeCategory = "Potentials";
    Plugin = "Basics";
    HasCalulateFunction = true;
}

SpartialCAPNode::~SpartialCAPNode(){}

void SpartialCAPNode::LoadData(json Data, std::string file)
{
	this->cap_type_index = Data["cap_type_index"];
	this->cap0_radius = Data["cap0_radius"];
	this->cap0_strength = Data["cap0_strength"];
	this->cap0_order = Data["cap0_order"];
	this->cap1_radius = Data["cap1_radius"];
	this->cap1_potential_max = Data["cap1_potential_max"];
	this->cap1_curvat = Data["cap1_curvat"];
	this->cap_integrator_range = Data["cap_integrator_range"];
	this->cap_integrator_points = Data["cap_integrator_points"];

    if (State == NodeState::Calculated)
    {
        save_load::load(file, this->id, "cap_matrix", &capmatrix);

        _capmatrix = std::make_shared<Eigen::MatrixXcd>(this->capmatrix);
    }
}

json SpartialCAPNode::SaveData(std::string file)
{
    if (State == NodeState::Calculated)
    {
        save_load::save(file, this->id, "cap_matrix", capmatrix);
    }

    json DataJson;
	DataJson["cap_type_index"] = this->cap_type_index;
	DataJson["cap0_radius"] = this->cap0_radius;
	DataJson["cap0_strength"] = this->cap0_strength;
	DataJson["cap0_order"] = this->cap0_order;
	DataJson["cap1_radius"] = this->cap1_radius;
	DataJson["cap1_potential_max"] = this->cap1_potential_max;
	DataJson["cap1_curvat"] = this->cap1_curvat;
	DataJson["cap_integrator_range"] = this->cap_integrator_range;
	DataJson["cap_integrator_points"] = this->cap_integrator_points;
    return DataJson;
}

void SpartialCAPNode::calculate()
{
    auto basisset = *(_basisset.lock().get());
    auto hfcmatrix = *(_hfcmatrix.lock().get());
    QC::CIResults ci_results = *(_ci_results.lock().get());

    QC::Operator_Function* Cap_ref;
    QC::CAP1 Cap_type0;
    QC::CAP2 Cap_type1;

    if (this->cap_type_index == 0)
    {
        Cap_type0.set_radius_abs(this->cap0_radius);
        Cap_type0.set_order(this->cap0_order);
        Cap_type0.set_strength(this->cap0_strength);
        Cap_ref = &Cap_type0;
    }
    else if (this->cap_type_index == 1)
    {
        Cap_type1.set_radius_abs(this->cap1_radius);
        Cap_type1.set_curvate(this->cap1_curvat);
        Cap_type1.set_potential_max(this->cap1_potential_max);
        Cap_ref = &Cap_type1;
    }

    QC::Integrator_Operator_Function_Numerical Integrator;
    Integrator.set_basisset(basisset);
    Integrator.set_range(this->cap_integrator_range);
    Integrator.set_steps(this->cap_integrator_points);
    Integrator.set_operator_function(Cap_ref);
    Integrator.compute();

    QC::Transformation_Integrals_Basisfunction_To_HFOrbitals transform1;
    QC::Transformation_Integrals_HFOrbitals_To_Spinorbitals transform2;
    transform1.set_one_electron_integrals_basisfunction(Integrator.get_Integrals());
    transform1.set_hf_cmatrix(hfcmatrix);
    transform1.compute_one_electron_integrals();

    transform2.set_one_electron_integrals_hforbitals(transform1.get_one_electron_integrals_hforbitals());
    transform2.compute_one_electron_integrals();

    QC::Configuration_Interaction_One_Electron_Operator CI_Operator;
    CI_Operator.set_ci_matrix(ci_results.ci_matrix);
    CI_Operator.set_one_electron_integrals_spinorbitals(transform2.get_one_electron_integrals_spinorbitals());

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
    std::complex<double> prefactor(0, -(1/(double)electrons));
    this->capmatrix = prefactor * CI_Operator.get_one_electron_operator_matrix();

    _capmatrix = std::make_shared<Eigen::MatrixXcd>(this->capmatrix);
    update_AllPortData();

    State = NodeState::Calculated;
}

void SpartialCAPNode::NodeInspector(QWidget* Inspector)
{
    QVBoxLayout* lay = new QVBoxLayout();

    cap_type = new QComboBox;
    cap_type->addItem("CAP1");
    cap_type->addItem("CAP2");
    cap_type->setCurrentIndex(this->cap_type_index);
    connect(cap_type, &QComboBox::currentTextChanged, this, &SpartialCAPNode::select_cap_type);
    //connect(cap_type, &QComboBox::currentTextChanged, [=]() { this->select_cap_type(parameterlistlayout); });


    lay->addWidget(new QLabel("Select CAP-Type"));
    lay->addWidget(cap_type);
    
    CAP0_Layout = new QVBoxLayout();
    CAP1_Layout = new QVBoxLayout();

    CAP0_settings = new QWidget();
    CAP1_settings = new QWidget();
    { 
        cap0_radius_lineedit = new QLineEdit;
        cap0_radius_lineedit->setValidator(new QDoubleValidator(0, 100000000, 5, this));
        cap0_radius_lineedit->setText(QString::number(cap0_radius));
        connect(cap0_radius_lineedit, &QLineEdit::textChanged, this, &SpartialCAPNode::select_cap0_radius);
        QLabel* text1 = new QLabel("Select CAP-Radius");
        CAP0_Layout->addWidget(text1);
        CAP0_Layout->addWidget(cap0_radius_lineedit);

        auto line2 = new QFrame;
        line2->setFrameShape(QFrame::HLine);
        line2->setFrameShadow(QFrame::Sunken);
        lay->addWidget(line2);
        CAP0_Layout->addWidget(line2);

        cap_strength_lineedit = new QLineEdit;
        cap_strength_lineedit->setValidator(new QDoubleValidator(0, 100000000, 5, this));
        cap_strength_lineedit->setText(QString::number(cap0_strength));
        connect(cap_strength_lineedit, &QLineEdit::textChanged, this, &SpartialCAPNode::select_cap_strength);
        QLabel* text2 = new QLabel("Select CAP-Strenght");
        CAP0_Layout->addWidget(text2);
        CAP0_Layout->addWidget(cap_strength_lineedit);

        auto line3 = new QFrame;
        line3->setFrameShape(QFrame::HLine);
        line3->setFrameShadow(QFrame::Sunken);
        CAP0_Layout->addWidget(line3);

        cap_order_lineedit = new QLineEdit;
        cap_order_lineedit->setValidator(new QIntValidator(0, 100000000, this));
        cap_order_lineedit->setText(QString::number(cap0_order));
        connect(cap_order_lineedit, &QLineEdit::textChanged, this, &SpartialCAPNode::select_cap_order);
        QLabel* text3 = new QLabel("Select CAP-Order");
        CAP0_Layout->addWidget(text3);
        CAP0_Layout->addWidget(cap_order_lineedit);
    }
    {
        cap1_radius_lineedit = new QLineEdit;
        cap1_radius_lineedit->setValidator(new QDoubleValidator(0, 100000000, 5, this));
        cap1_radius_lineedit->setText(QString::number(cap1_radius));
        connect(cap1_radius_lineedit, &QLineEdit::textChanged, this, &SpartialCAPNode::select_cap1_radius);
        QLabel* text1 = new QLabel("Select CAP-Radius");
        CAP1_Layout->addWidget(text1);
        CAP1_Layout->addWidget(cap1_radius_lineedit);

        auto line2 = new QFrame;
        line2->setFrameShape(QFrame::HLine);
        line2->setFrameShadow(QFrame::Sunken);
        CAP1_Layout->addWidget(line2);

        cap_potential_max_lineedit = new QLineEdit;
        cap_potential_max_lineedit->setValidator(new QDoubleValidator(0, 100000000, 5, this));
        cap_potential_max_lineedit->setText(QString::number(cap1_potential_max));
        connect(cap_potential_max_lineedit, &QLineEdit::textChanged, this, &SpartialCAPNode::select_cap_potential_max);
        QLabel* text2 = new QLabel("Select CAP-Potential_Max");
        CAP1_Layout->addWidget(text2);
        CAP1_Layout->addWidget(cap_potential_max_lineedit);

        auto line3 = new QFrame;
        line3->setFrameShape(QFrame::HLine);
        line3->setFrameShadow(QFrame::Sunken);
        CAP1_Layout->addWidget(line3);

        cap_curvat_lineedit = new QLineEdit;
        cap_curvat_lineedit->setValidator(new QDoubleValidator(0, 100000000, 5, this));
        cap_curvat_lineedit->setText(QString::number(cap1_curvat));
        connect(cap_curvat_lineedit, &QLineEdit::textChanged, this, &SpartialCAPNode::select_cap_curvat);
        QLabel* text3 = new QLabel("Select CAP-Curvat");
        CAP1_Layout->addWidget(text3);
        CAP1_Layout->addWidget(cap_curvat_lineedit);
    }

    CAP0_settings->setLayout(CAP0_Layout);
    CAP1_settings->setLayout(CAP1_Layout);
    lay->addWidget(CAP0_settings);
    lay->addWidget(CAP1_settings);

    if (cap_type_index == 0)
    {
        CAP0_settings->setVisible(true);
        CAP1_settings->setVisible(false);
    }
    else if (cap_type_index == 1)
    {
        CAP0_settings->setVisible(false);
        CAP1_settings->setVisible(true);
    }


    auto line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    lay->addWidget(line);

    cap_integrator_range_lineedit = new QLineEdit;
    cap_integrator_range_lineedit->setValidator(new QDoubleValidator(0, 100000000, 5, this));
    cap_integrator_range_lineedit->setText(QString::number(cap_integrator_range));
    connect(cap_integrator_range_lineedit, &QLineEdit::textChanged, this, &SpartialCAPNode::select_cap_integrator_range);
    lay->addWidget(new QLabel("Select CAP-Integrator-Range"));
    lay->addWidget(cap_integrator_range_lineedit);

    auto line2 = new QFrame;
    line2->setFrameShape(QFrame::HLine);
    line2->setFrameShadow(QFrame::Sunken);
    lay->addWidget(line2);

    cap_integrator_points_lineedit = new QLineEdit;
    cap_integrator_points_lineedit->setValidator(new QIntValidator(0, 100000000, this));
    cap_integrator_points_lineedit->setText(QString::number(cap_integrator_points));
    connect(cap_integrator_points_lineedit, &QLineEdit::textChanged, this, &SpartialCAPNode::select_cap_integrator_points);
    lay->addWidget(new QLabel("Select CAP-Integrator-Points"));
    lay->addWidget(cap_integrator_points_lineedit);

    Inspector->setLayout(lay);
}

bool SpartialCAPNode::InPortAllowMultipleConnections(int Port)
{ 
    return false; 
}

unsigned int SpartialCAPNode::Ports(NodePortType PortType)
{
    if(PortType == NodePortType::In)
    {
        return 3;
    }
    else
    {
        return 1;
    }
}

bool SpartialCAPNode::IsCustomPortCaption(NodePortType PortType, int Port)
{ 
    return true; 
}

std::string SpartialCAPNode::CustomPortCaption(NodePortType PortType, int Port)
{ 
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "Basisset"; }
        else if(Port == 1) {return "C-Matrix"; }
        else if(Port == 2) {return "CIResults"; }
    }
    else
    {
        if(Port == 0) {return "CAPMatrix"; }
    }
}

std::string SpartialCAPNode::DataTypeName(NodePortType PortType, int Port)
{
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "QC::Gaussian_Basisset"; }
        else if(Port == 1) {return "Eigen::MatrixXd"; }
        else if(Port == 2) {return "QC::CIResults"; }
    }
    else
    {
        if(Port == 0) {return "Eigen::MatrixXcd"; }
    }
}

std::any SpartialCAPNode::getOutData(int Port) 
{ 
    std::any ret;
    ret = std::make_any<std::weak_ptr<Eigen::MatrixXcd>>(_capmatrix);
    return ret;
}

void SpartialCAPNode::setInData(int Port, std::any data)
{
    if(data.has_value())
    {
        if(Port == 0)
        {
            _basisset = std::any_cast<std::weak_ptr<QC::Gaussian_Basisset>>(data);
        }
        if(Port == 1)
        {
            _hfcmatrix = std::any_cast<std::weak_ptr<Eigen::MatrixXd>>(data);
        }
        if(Port == 2)
        {
            _ci_results = std::any_cast<std::weak_ptr<QC::CIResults>>(data);
        }
    }
    else
    {
        if(Port == 0){_basisset.reset();}
        else if(Port == 1){_hfcmatrix.reset();}
        else if(Port == 2){_ci_results.reset();}
    }
}








void SpartialCAPNode::select_cap_type()
{
    if(this->cap_type->currentIndex() == cap_type_index) { return; }
    cap_type_index = this->cap_type->currentIndex();

    if (cap_type_index == 0)
    {
        CAP0_settings->setVisible(true);
        CAP1_settings->setVisible(false);
    }
    else if (cap_type_index == 1)
    {
        CAP0_settings->setVisible(false);
        CAP1_settings->setVisible(true);
    }
}

void SpartialCAPNode::select_cap0_radius()
{
    this->cap0_radius = this->cap0_radius_lineedit->text().toDouble();
}

void SpartialCAPNode::select_cap1_radius()
{
    this->cap1_radius = this->cap1_radius_lineedit->text().toDouble();
}

void SpartialCAPNode::select_cap_strength()
{
    this->cap0_strength = this->cap_strength_lineedit->text().toDouble();
}

void SpartialCAPNode::select_cap_order()
{
    this->cap0_order = this->cap_order_lineedit->text().toInt();
}

void SpartialCAPNode::select_cap_potential_max()
{
    this->cap1_potential_max = this->cap_potential_max_lineedit->text().toDouble();
}

void SpartialCAPNode::select_cap_curvat()
{
    this->cap1_curvat = this->cap_curvat_lineedit->text().toDouble();
}

void SpartialCAPNode::select_cap_integrator_range()
{
    this->cap_integrator_range = this->cap_integrator_range_lineedit->text().toDouble();
}

void SpartialCAPNode::select_cap_integrator_points()
{
    this->cap_integrator_points = this->cap_integrator_points_lineedit->text().toInt();
};