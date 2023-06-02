#include "PropagatorNode.hpp"

PropagatorNode::PropagatorNode(std::string InpNodeTypeID) : Node()
{   
    NodeTypeID = InpNodeTypeID;
    NodeName = "Propagate Wavefunction";
    NodeDescription = "Propagate Wavefunction";
    NodeCategory = "Electron Dynamics";
    Plugin = "Basics";
    HasCalulateFunction = true;
}

PropagatorNode::~PropagatorNode(){}

void PropagatorNode::LoadData(json Data, std::string file)
{
    this->states_string = Data["initial_states"];
    this->steps = Data["steps"];
    this->stepsize = Data["stepsize"];
    this->use_laserfile = Data["use_laserfile"];
    this->full_laserfile = Data["full_laserfile"];
    size_t i = full_laserfile.rfind("/", full_laserfile.length());
    laserfile = full_laserfile.substr(i + 1, full_laserfile.length() - i);

    if(Data.find("truncate_states") != Data.end())
    {
        this->truncate_states = Data["truncate_states"];
        this->truncation_state = Data["truncation_state"];
    }

    if (State == NodeState::Calculated)
    {
        save_load::load(file, this->id, "tdci_wf", &tdci_wf);

        _tdci_coeffs = std::make_shared<QC::TDCI_WF>(this->tdci_wf);
    }
}

json PropagatorNode::SaveData(std::string file)
{
    if (State == NodeState::Calculated)
    {
        save_load::save(file, this->id, "tdci_wf", tdci_wf);
    }

    json DataJson;
    DataJson["initial_states"] = this->states_string;
    DataJson["steps"] = this->steps;
    DataJson["stepsize"] = this->stepsize;
    DataJson["use_laserfile"] = this->use_laserfile;
    DataJson["full_laserfile"] = this->full_laserfile;
    DataJson["truncate_states"] = this->truncate_states;
    DataJson["truncation_state"] = this->truncation_state;
    return DataJson;
}

void PropagatorNode::calculate()
{
    auto cienergys = (*(_ci_results.lock().get())).ci_values;

    QC::Propagator Propagator;

    std::istringstream iss(this->states_string);
    std::string s;
    while (std::getline(iss, s, ' ')) {
        Propagator.add_initial_wf(std::stoi(s));
    }

    Propagator.set_TruncateStates(truncate_states);
    Propagator.set_StateNumberThresh(truncation_state);
    Propagator.set_eigen_energys(cienergys);

    if (this->use_laserfile && !_cicapmatrix.expired()) {
        auto TransitionDipoleMoments = *(_TransitionDipoleMoments.lock().get());
        Propagator.set_electronic_dipole_matrices(TransitionDipoleMoments);
        QC::Laser laser;
        laser.set_type(QC::Laser::LaserType::File);
        laser.set_laserfile(this->full_laserfile);
        Propagator.set_laser(&laser);
        auto CICAPMatrix = *(_cicapmatrix.lock().get());
        Propagator.set_one_electron_operator(CICAPMatrix);
        Propagator.compute_propagation_with_laserfile_and_cap();
    }
    else if (this->use_laserfile && _cicapmatrix.expired()) {
        auto TransitionDipoleMoments = *(_TransitionDipoleMoments.lock().get());
        Propagator.set_electronic_dipole_matrices(TransitionDipoleMoments);
        QC::Laser laser; 
        laser.set_type(QC::Laser::LaserType::File);
        laser.set_laserfile(this->full_laserfile);
        Propagator.set_laser(&laser);
        Propagator.compute_propagation_with_laserfile();
    }
    else if (!_laser.expired() && !_cicapmatrix.expired()) {
        auto TransitionDipoleMoments = *(_TransitionDipoleMoments.lock().get());
        Propagator.set_electronic_dipole_matrices(TransitionDipoleMoments);
        Propagator.set_steps(this->steps);
        Propagator.set_stepsize(this->stepsize);
        auto laser = _laser.lock().get();
        Propagator.set_laser(laser);
        auto CICAPMatrix = *(_cicapmatrix.lock().get());
        Propagator.set_one_electron_operator(CICAPMatrix);
        Propagator.compute_propagation_with_laser_and_cap();

    }
    else if (!_laser.expired() && _cicapmatrix.expired()) {
        auto TransitionDipoleMoments = *(_TransitionDipoleMoments.lock().get());
        Propagator.set_electronic_dipole_matrices(TransitionDipoleMoments);
        Propagator.set_steps(this->steps);
        Propagator.set_stepsize(this->stepsize);
        auto laser = _laser.lock().get();
        Propagator.set_laser(laser);
        Propagator.compute_propagation_with_laser();
    }
    else if (_laser.expired() && !_cicapmatrix.expired()) {
        Propagator.set_steps(this->steps);
        Propagator.set_stepsize(this->stepsize);
        auto CICAPMatrix = *(_cicapmatrix.lock().get());
        Propagator.set_one_electron_operator(CICAPMatrix);
        Propagator.compute_propagation_with_cap();
    }
    else if (_laser.expired() && _cicapmatrix.expired()) {
        Propagator.set_steps(this->steps);
        Propagator.set_stepsize(this->stepsize);
        Propagator.compute_propagation();
    }

    this->tdci_wf.clear();
    this->tdci_wf = Propagator.get_tdci_wf();

    _tdci_coeffs = std::make_shared<QC::TDCI_WF>(this->tdci_wf);
    update_AllPortData();

    State = NodeState::Calculated;
}

void PropagatorNode::NodeInspector(QWidget* Inspector)
{
    QVBoxLayout* lay = new QVBoxLayout();

   states_lineedit = new QLineEdit;
    states_lineedit->setText(QString::fromStdString(states_string));
    connect(states_lineedit, &QLineEdit::textChanged, this, &PropagatorNode::select_states);

    steps_lineedit = new QLineEdit;
    steps_lineedit->setValidator(new QIntValidator(0, 100000000, this));
    steps_lineedit->setText(QString::number(steps));
    connect(steps_lineedit, &QLineEdit::textChanged, this, &PropagatorNode::select_steps);

    stepsize_lineedit = new QLineEdit;
    stepsize_lineedit->setValidator(new QDoubleValidator(0, 100000000, 5, this));
    stepsize_lineedit->setText(QString::number(stepsize));
    connect(stepsize_lineedit, &QLineEdit::textChanged, this, &PropagatorNode::select_stepsize);

    truncate_states_box = new QCheckBox;
    truncate_states_box->setText("Truncate States");
    truncate_states_box->setChecked(truncate_states);
    connect(truncate_states_box, &QCheckBox::stateChanged, this, &PropagatorNode::select_truncate_states);

    truncation_state_lineedit = new QLineEdit;
    truncation_state_lineedit->setValidator(new QIntValidator(0, 100000000, this));
    truncation_state_lineedit->setText(QString::number(truncation_state));
    connect(truncation_state_lineedit, &QLineEdit::textChanged, this, &PropagatorNode::select_truncation_state);

    use_laserfile_box = new QCheckBox;
    use_laserfile_box->setText("Use Laserfile");
    use_laserfile_box->setChecked(use_laserfile);
    connect(use_laserfile_box, &QCheckBox::stateChanged, this, &PropagatorNode::select_use_laserfile);
    laserfile_label = new QLabel;
    laserfile_label->setText("Selected: " + QString::fromStdString(laserfile));
    QPushButton* select_laserfile_button = new QPushButton("Select");
    connect(select_laserfile_button, &QPushButton::clicked, this, &PropagatorNode::select_laserfile);

    lay->addWidget(new QLabel("Select initial states"));
    QLabel * initial_states_info = new QLabel("When propagating multiple states, separate them with spaces(e.g.: 0 8 27).");
    initial_states_info->setWordWrap(true);
    lay->addWidget(initial_states_info);
    lay->addWidget(states_lineedit);

    auto line2 = new QFrame;
    line2->setFrameShape(QFrame::HLine);
    line2->setFrameShadow(QFrame::Sunken);
    lay->addWidget(line2);

    lay->addWidget(new QLabel("Select number of time steps"));
    lay->addWidget(steps_lineedit);

    auto line3 = new QFrame;
    line3->setFrameShape(QFrame::HLine);
    line3->setFrameShadow(QFrame::Sunken);
    lay->addWidget(line3);

    lay->addWidget(new QLabel("Select size of time steps"));
    lay->addWidget(stepsize_lineedit);

    auto line4 = new QFrame;
    line4->setFrameShape(QFrame::HLine);
    line4->setFrameShadow(QFrame::Sunken);
    lay->addWidget(line4);

    QLabel* Truncation_info = new QLabel("The Propagation can also be performed in a truncated basis of CI-States.");
    Truncation_info->setWordWrap(true);
    lay->addWidget(Truncation_info);
    lay->addWidget(truncate_states_box);
    lay->addWidget(new QLabel("Select last CI-state"));
    lay->addWidget(truncation_state_lineedit);

    auto line5 = new QFrame;
    line5->setFrameShape(QFrame::HLine);
    line5->setFrameShadow(QFrame::Sunken);
    lay->addWidget(line5);

    lay->addWidget(new QLabel("Select Laser-File"));
    QLabel* laserfile_info = new QLabel("A laser file is an ASCII file with 4 columns of time and the laser intensities in the 3 spatial directions at the corresponding time. If a laser file is specified, the time steps and laser information from the file are used and all further inputs are ignored.");
    laserfile_info->setWordWrap(true);
    lay->addWidget(laserfile_info);
    lay->addWidget(use_laserfile_box);
    lay->addWidget(laserfile_label);
    lay->addWidget(select_laserfile_button);

    QPushButton* Propagation_Analysis = new QPushButton("Analyse Propagation");
    connect(Propagation_Analysis, &QPushButton::clicked, this, &PropagatorNode::start_analyse_propagation);
    lay->addWidget(Propagation_Analysis);

    Inspector->setLayout(lay);
}

bool PropagatorNode::InPortAllowMultipleConnections(int Port)
{ 
    return false; 
}

unsigned int PropagatorNode::Ports(NodePortType PortType)
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

bool PropagatorNode::IsCustomPortCaption(NodePortType PortType, int Port)
{ 
    return true; 
}

std::string PropagatorNode::CustomPortCaption(NodePortType PortType, int Port)
{ 
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "Dipolematrix"; }
        else if(Port == 1) {return "CIResults"; }
        else if(Port == 2) {return "Laser"; }
        else if(Port == 3) {return "CAPMatrix"; }
    }
    else
    {
        if(Port == 0) {return "TDCIVectors"; }
    }
}

std::string PropagatorNode::DataTypeName(NodePortType PortType, int Port)
{
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "std::vector<Eigen::MatrixXd>"; }
        else if(Port == 1) {return "QC::CIResults"; }
        else if(Port == 2) {return "QC::Laser"; }
        else if(Port == 3) {return "Eigen::MatrixXcd"; }
    }
    else
    {
        if(Port == 0) {return "QC::TDCI_WF"; }
    }
}

std::any PropagatorNode::getOutData(int Port) 
{ 
    std::any ret;
    if(Port == 0)
    {
        ret = std::make_any<std::weak_ptr<QC::TDCI_WF>>(_tdci_coeffs);
    }
    return ret;
}

void PropagatorNode::setInData(int Port, std::any data)
{
    if(data.has_value())
    {
        if(Port == 0)
        {
            _TransitionDipoleMoments = std::any_cast<std::weak_ptr<std::vector<Eigen::MatrixXd>>>(data);
        }
        if(Port == 1)
        {
            _ci_results = std::any_cast<std::weak_ptr<QC::CIResults>>(data);
        }
        if(Port == 2)
        {
            _laser = std::any_cast<std::weak_ptr<QC::Laser>>(data);
        }
        if(Port == 3)
        {
            _cicapmatrix = std::any_cast<std::weak_ptr<Eigen::MatrixXcd>>(data);
        }
    }
    else
    {
        if(Port == 0){_TransitionDipoleMoments.reset();}
        else if(Port == 1){_ci_results.reset();}
        else if(Port == 2){_laser.reset();}
        else if(Port == 3){_cicapmatrix.reset();}
    }
}

void PropagatorNode::select_states()
{
    this->states_string = this->states_lineedit->text().toStdString();
    PropertiesChanged();
}

void PropagatorNode::select_steps()
{
    this->steps = this->steps_lineedit->text().toInt();
    PropertiesChanged();
}

void PropagatorNode::select_stepsize()
{
    this->stepsize = this->stepsize_lineedit->text().toDouble();
    PropertiesChanged();
}

void PropagatorNode::select_truncate_states()
{
    this->truncate_states = this->truncate_states_box->isChecked();
    PropertiesChanged();
}

void PropagatorNode::select_truncation_state()
{
    this->truncation_state = this->truncation_state_lineedit->text().toInt();
    PropertiesChanged();
}

void PropagatorNode::select_use_laserfile()
{
    this->use_laserfile = this->use_laserfile_box->isChecked();
    PropertiesChanged();
}

void PropagatorNode::select_laserfile()
{
    this->full_laserfile = QFileDialog::getOpenFileName(new QDialog, tr("Open Laser File"), "", tr("Laser Files (*.laser)")).toStdString();
    size_t i = full_laserfile.rfind("/", full_laserfile.length());
    laserfile = full_laserfile.substr(i + 1, full_laserfile.length() - i);
    laserfile_label->setText("Selected: " + QString::fromStdString(laserfile));
    PropertiesChanged();
}

void PropagatorNode::start_analyse_propagation()
{
    TDWF_Analysis_Window* tdwf_analysis_window = new TDWF_Analysis_Window(&this->tdci_wf, (*(_ci_results.lock().get())).ci_values, this->_TransitionDipoleMoments, this->_laser);
    tdwf_analysis_window->show();
};