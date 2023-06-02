#include "TrotterPropagationNode.hpp"

TrotterPropagationNode::TrotterPropagationNode(std::string InpNodeTypeID) : Node()
{   
    NodeTypeID = InpNodeTypeID;
    NodeName = "Propagation with Trotter";
    NodeDescription = "Propagation with Trotter";
    NodeCategory = "Algorithms";
    Plugin = "QuantumComputing";
    HasCalulateFunction = true;
}

TrotterPropagationNode::~TrotterPropagationNode(){}

void TrotterPropagationNode::LoadData(json Data, std::string file)
{
    this->trotter_order = Data["trotter_order"];
    this->trotter_timestep = Data["trotter_timestep"];
    this->trotter_numbertimesteps = Data["trotter_numbertimesteps"];
    this->use_lowest_flipped_init_wavefunction = Data["use_lowest_flipped_init_wavefunction"];
    this->number_qubits_state1 = Data["number_qubits_state1"];
    this->dump_statevector = Data["dump_statevector"];
    this->get_qite_expecation_values_without_measurement = Data["get_qite_expecation_values_without_measurement"];
    this->measurments_per_qite_expection_value = Data["measurments_per_qite_expection_value"];

    if (State == NodeState::Calculated)
    {
        save_load::load(file, this->id, "dumped_statevectors", &dumped_statevectors);
        save_load::load(file, this->id, "td_norm", &td_norm);

        _tdstatevectors = std::make_shared<QC::QC_TD_Statevectors>(this->dumped_statevectors);
        _tdnorm = std::make_shared<QC::QC_TD_Norm>(this->td_norm);
    }
}

json TrotterPropagationNode::SaveData(std::string file)
{
    if (State == NodeState::Calculated)
    {
        save_load::save(file, this->id, "dumped_statevectors", dumped_statevectors);
        save_load::save(file, this->id, "td_norm", td_norm);
    }

    json DataJson;
    DataJson["trotter_order"] = this->trotter_order;
    DataJson["trotter_timestep"] = this->trotter_timestep;
    DataJson["trotter_numbertimesteps"] = this->trotter_numbertimesteps;
    DataJson["use_lowest_flipped_init_wavefunction"] = this->use_lowest_flipped_init_wavefunction;
    DataJson["number_qubits_state1"] = this->number_qubits_state1;
    DataJson["dump_statevector"] = this->dump_statevector;
    DataJson["get_qite_expecation_values_without_measurement"] = this->get_qite_expecation_values_without_measurement;
    DataJson["measurments_per_qite_expection_value"] = this->measurments_per_qite_expection_value;
    return DataJson;
}

void TrotterPropagationNode::calculate()
{
    auto Simulator = _Simulator.lock().get();

    QC::quantum_propagation_laser_cap Propagator;
    Propagator.set_trotter_parameter(this->trotter_timestep, this->trotter_order);
    Propagator.set_number_timesteps(this->trotter_numbertimesteps);
    Propagator.set_QC_Simulator(Simulator);
    Propagator.set_TimeIndependent_Pauli_Operator(*(_TIDPauliHamiltonian.lock().get()));
    Propagator.set_get_qite_expecation_values_without_measurement(this->get_qite_expecation_values_without_measurement);
    Propagator.set_measurments_per_qite_expection_value(this->measurments_per_qite_expection_value);

    if (_LaserPauliOperator.expired())
    {
        Propagator.set_use_Laser_Operator(false);
    }
    else
    {
        Propagator.set_use_Laser_Operator(true);
        Propagator.set_Laser_Pauli_Operator(_LaserPauliOperator.lock().get());
    }

    if (_CAPPauliOperator.expired())
    {
        Propagator.set_use_CAP_Operator(false);
    }
    else
    {
        Propagator.set_use_CAP_Operator(true);
        Propagator.set_CAP_Operator(*(_CAPPauliOperator.lock().get()));
    }


    if (this->use_lowest_flipped_init_wavefunction) {
        Propagator.create_init_state_as_lowest_flipped(this->number_qubits_state1);
        Simulator->type_initial_State = 0;
    }
    else {
        auto InitState = *(_InitState.lock().get());
        //Propagator.set_state_preparation_gate_sequence(InitState);

        QC::QGateSequence dummy;
        Propagator.set_state_preparation_gate_sequence(dummy);

        Simulator->initial_Statevector = InitState;
        Simulator->type_initial_State = 1;
    }
    if (this->dump_statevector)
    {
        QC::CIResults ci_results = *(_ci_results.lock().get());

        Propagator.set_dump_statevector(true);
        auto AmpsToDump = ci_results.configuration_strings;

        std::vector<int> amps_to_dump_dec;

        for (int i = 0; i < AmpsToDump.size(); i++)
        {
            std::string temp = AmpsToDump[i];
            std::reverse(temp.begin(), temp.end());
            long long int n = std::stoll(temp);
            int decimalNumber = 0, j = 0, remainder;
            while (n != 0)
            {
                remainder = n % 10;
                n /= 10;
                decimalNumber += remainder * pow(2, j);
                ++j;
            }
            amps_to_dump_dec.push_back(decimalNumber);
        }
        Propagator.set_amplitudes_to_dump(amps_to_dump_dec);
    }
    else
    {
        Propagator.set_dump_statevector(false);
    }
    Propagator.compute();


    this->dumped_statevectors = Propagator.get_dumped_statevectors();
    this->td_norm = Propagator.get_TD_Norm();
    _tdstatevectors = std::make_shared<QC::QC_TD_Statevectors>(this->dumped_statevectors);
    _tdnorm = std::make_shared<QC::QC_TD_Norm>(this->td_norm);
    update_AllPortData();

    State = NodeState::Calculated;
}

void TrotterPropagationNode::NodeInspector(QWidget* Inspector)
{
    QVBoxLayout* lay = new QVBoxLayout();

    trotter_order_lineedit = new QLineEdit;
    trotter_order_lineedit->setValidator(new QIntValidator(1, 10, this));
    trotter_order_lineedit->setText(QString::number(this->trotter_order));
    connect(trotter_order_lineedit, &QLineEdit::textChanged, this, &TrotterPropagationNode::select_trotter_order);

    trotter_timestep_lineedit = new QLineEdit;
    trotter_timestep_lineedit->setValidator(new QDoubleValidator(0.0, 10, 3, this));
    trotter_timestep_lineedit->setText(QString::number(this->trotter_timestep));
    connect(trotter_timestep_lineedit, &QLineEdit::textChanged, this, &TrotterPropagationNode::select_trotter_timestep);

    trotter_numbertimesteps_lineedit = new QLineEdit;
    trotter_numbertimesteps_lineedit->setValidator(new QIntValidator(1, 100000000, this));
    trotter_numbertimesteps_lineedit->setText(QString::number(this->trotter_numbertimesteps));
    connect(trotter_numbertimesteps_lineedit, &QLineEdit::textChanged, this, &TrotterPropagationNode::select_trotter_numbertimesteps);

    use_lowest_flipped_init_wavefunction_box = new QCheckBox;
    use_lowest_flipped_init_wavefunction_box->setText("Use first n Qubits in 1 as Init-WF");
    use_lowest_flipped_init_wavefunction_box->setChecked(use_lowest_flipped_init_wavefunction);
    connect(use_lowest_flipped_init_wavefunction_box, &QCheckBox::stateChanged, this, &TrotterPropagationNode::select_use_lowest_flipped_init_wavefunction);

    number_qubits_state1_lineedit = new QLineEdit;
    number_qubits_state1_lineedit->setValidator(new QIntValidator(1, 100000000, this));
    number_qubits_state1_lineedit->setText(QString::number(this->number_qubits_state1));
    connect(number_qubits_state1_lineedit, &QLineEdit::textChanged, this, &TrotterPropagationNode::select_number_qubits_state1);

    dump_statevector_box = new QCheckBox;
    dump_statevector_box->setText("Dump partial Statevector");
    dump_statevector_box->setChecked(dump_statevector);
    connect(dump_statevector_box, &QCheckBox::stateChanged, this, &TrotterPropagationNode::select_dump_statevector);

    get_qite_expecation_values_without_measurement_checkbox = new QCheckBox;
    get_qite_expecation_values_without_measurement_checkbox->setText("Get QITE expecation values without measurement");
    get_qite_expecation_values_without_measurement_checkbox->setChecked(get_qite_expecation_values_without_measurement);
    connect(get_qite_expecation_values_without_measurement_checkbox, &QCheckBox::stateChanged, this, &TrotterPropagationNode::select_get_qite_expecation_values_without_measurement);

    measurments_per_qite_expection_value_lineedit = new QLineEdit;
    measurments_per_qite_expection_value_lineedit->setValidator(new QIntValidator(1, 100000000, this));
    measurments_per_qite_expection_value_lineedit->setText(QString::number(this->measurments_per_qite_expection_value));
    connect(measurments_per_qite_expection_value_lineedit, &QLineEdit::textChanged, this, &TrotterPropagationNode::select_measurments_per_qite_expection_value);

    lay->addWidget(new QLabel("Select Trotter-Order"));
    lay->addWidget(trotter_order_lineedit);
    {
        auto line = new QFrame;
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        lay->addWidget(line);
    }
    lay->addWidget(new QLabel("Select Trotter-Timestep"));
    lay->addWidget(trotter_timestep_lineedit);
    {
        auto line = new QFrame;
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        lay->addWidget(line);
    }
    lay->addWidget(new QLabel("Select Number of Trotter-Timesteps"));
    lay->addWidget(trotter_numbertimesteps_lineedit);
    {
        auto line = new QFrame;
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        lay->addWidget(line);
    }

    lay->addWidget(new QLabel("Select Initial State"));
    QLabel* info = new QLabel("A state with the n first qubits in state 1 and the rest in state 0 can also be used as the initial state. If this is the case, the incoming node connection is ignored.");
    info->setWordWrap(true);
    lay->addWidget(info);
    lay->addWidget(use_lowest_flipped_init_wavefunction_box);
    lay->addWidget(new QLabel("Select number of Qubits in State 1"));
    lay->addWidget(number_qubits_state1_lineedit);
    {
        auto line = new QFrame;
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        lay->addWidget(line);
    }

    lay->addWidget(new QLabel("Dumping"));
    QLabel* info_dump = new QLabel("If activated, the selected amplitudes are stored in the TD Statevector in each time step. Otherwise, the input connection is ignored with respect to the amplitudes to be dumped. Dumping does not work on real quantum computers.");
    info_dump->setWordWrap(true);
    lay->addWidget(info_dump);
    lay->addWidget(dump_statevector_box);

    {
        auto line = new QFrame;
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        lay->addWidget(line);
    }

    lay->addWidget(new QLabel("QITE Parameter"));
    QLabel* info_expection_values = new QLabel("The expected values required for the QITE algorithm can be determined by test measurements or direct readout. Direct readout does not work on real quantum computers.");
    info_expection_values->setWordWrap(true);
    lay->addWidget(info_expection_values);
    lay->addWidget(get_qite_expecation_values_without_measurement_checkbox);
    lay->addWidget(new QLabel("Number of test measurements to determine the expectation values"));
    lay->addWidget(measurments_per_qite_expection_value_lineedit);

    Inspector->setLayout(lay);
}

bool TrotterPropagationNode::InPortAllowMultipleConnections(int Port)
{ 
    return false; 
}

unsigned int TrotterPropagationNode::Ports(NodePortType PortType)
{
    if(PortType == NodePortType::In)
    {
        return 6;
    }
    else
    {
        return 2;
    }
}

bool TrotterPropagationNode::IsCustomPortCaption(NodePortType PortType, int Port)
{ 
    return true; 
}

std::string TrotterPropagationNode::CustomPortCaption(NodePortType PortType, int Port)
{ 
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "Pauli Operator"; }
        else if(Port == 1) {return "Laser Pauli Operator"; }
        else if(Port == 2) {return "CAP Pauli Operator"; }
        else if(Port == 3) {return "QPU"; }
        else if(Port == 4) {return "Statevector"; }
        else if(Port == 5) {return "CI Results"; }
    }
    else
    {
        if(Port == 0) {return "TD Statevector"; }
        else if(Port == 1) {return "TD Norm"; }
    }
}

std::string TrotterPropagationNode::DataTypeName(NodePortType PortType, int Port)
{
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "QC::Pauli_Operator"; }
        else if(Port == 1) {return "QC::Laser_Pauli_Operator"; }
        else if(Port == 2) {return "QC::Pauli_Operator"; }
        else if(Port == 3) {return "QC::QC_Simulator"; }
        else if(Port == 4) {return "QC::QC_Statevector"; }
        else if(Port == 5) {return "QC::CIResults"; }
    }
    else
    {
        if(Port == 0) {return "QC::QC_TD_Statevectors"; }
        else if(Port == 1) {return "QC::QC_TD_Norm"; }
    }
}

std::any TrotterPropagationNode::getOutData(int Port) 
{ 
    std::any ret;
    if(Port == 0)
    {
        ret = std::make_any<std::weak_ptr<QC::QC_TD_Statevectors>>(_tdstatevectors);
    }
    else if(Port == 1)
    {
        ret = std::make_any<std::weak_ptr<QC::QC_TD_Norm>>(_tdnorm);
    }
    return ret;
}

void TrotterPropagationNode::setInData(int Port, std::any data)
{
    if(data.has_value())
    {
        if(Port == 0)
        {
            _TIDPauliHamiltonian = std::any_cast<std::weak_ptr<QC::Pauli_Operator>>(data);
        }
        else if(Port == 1)
        {
            _LaserPauliOperator = std::any_cast<std::weak_ptr<QC::Laser_Pauli_Operator>>(data);
        }
        else if(Port == 2)
        {
            _CAPPauliOperator = std::any_cast<std::weak_ptr<QC::Pauli_Operator>>(data);
        }
        else if(Port == 3)
        {
            _Simulator = std::any_cast<std::weak_ptr<QC::QC_Simulator>>(data);
        }
        else if(Port == 4)
        {
            _InitState = std::any_cast<std::weak_ptr<QC::QC_Statevector>>(data);
        }
        else if(Port == 5)
        {
            _ci_results = std::any_cast<std::weak_ptr<QC::CIResults>>(data);
        }
    }
    else
    {
        if(Port == 0){_TIDPauliHamiltonian.reset();}
        else if(Port == 1){_LaserPauliOperator.reset();}
        else if(Port == 2){_CAPPauliOperator.reset();}
        else if(Port == 3){_Simulator.reset();}
        else if(Port == 4){_InitState.reset();}
        else if(Port == 5){_ci_results.reset();}
    }
}

void TrotterPropagationNode::select_trotter_order()
{
    this->trotter_order = this->trotter_order_lineedit->text().toInt();
    PropertiesChanged();
}

void TrotterPropagationNode::select_trotter_timestep()
{
    this->trotter_timestep = this->trotter_timestep_lineedit->text().toDouble();
    PropertiesChanged();
}

void TrotterPropagationNode::select_trotter_numbertimesteps()
{
    this->trotter_numbertimesteps = this->trotter_numbertimesteps_lineedit->text().toInt();
    PropertiesChanged();
}

void TrotterPropagationNode::select_use_lowest_flipped_init_wavefunction()
{
    this->use_lowest_flipped_init_wavefunction = this->use_lowest_flipped_init_wavefunction_box->isChecked();
    PropertiesChanged();
}

void TrotterPropagationNode::select_number_qubits_state1()
{
    this->number_qubits_state1 = this->number_qubits_state1_lineedit->text().toInt();
    PropertiesChanged();
}

void TrotterPropagationNode::select_dump_statevector()
{
    this->dump_statevector = this->dump_statevector_box->isChecked();
    PropertiesChanged();
}

void TrotterPropagationNode::select_get_qite_expecation_values_without_measurement()
{
    this->get_qite_expecation_values_without_measurement = this->get_qite_expecation_values_without_measurement_checkbox->isChecked();
    PropertiesChanged();
}

void TrotterPropagationNode::select_measurments_per_qite_expection_value()
{
    this->measurments_per_qite_expection_value = this->measurments_per_qite_expection_value_lineedit->text().toInt();
    PropertiesChanged();
}