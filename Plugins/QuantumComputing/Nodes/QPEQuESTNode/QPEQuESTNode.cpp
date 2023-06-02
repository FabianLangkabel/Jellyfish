#include "QPEQuESTNode.hpp"

QPEQuESTNode::QPEQuESTNode(std::string InpNodeTypeID) : Node()
{   
    NodeTypeID = InpNodeTypeID;
    NodeName = "Quantum Phase Estimation (QuEST)";
    NodeDescription = "Quantum Phase Estimation (QuEST)";
    NodeCategory = "Algorithms";
    Plugin = "QuantumComputing";
    HasCalulateFunction = true;
}

QPEQuESTNode::~QPEQuESTNode(){}

void QPEQuESTNode::LoadData(json Data, std::string file)
{
    this->phases.clear();
    this->positive_energys.clear();
    this->negative_energys.clear();

    this->runs = Data["runs"];
    this->trotter_order = Data["trotter_order"];
    this->trotter_timestep = Data["trotter_timestep"];
    this->prec_qubits = Data["prec_qubits"];
    this->use_lowest_flipped_init_wavefunction = Data["use_lowest_flipped_init_wavefunction"];
    this->number_qubits_state1 = Data["number_qubits_state1"];

    if (State == NodeState::Calculated)
    {
        json results = Data["results"];
        for (int i = 0; i < this->runs; i++)
        {
            json result = results[std::to_string(i)];
            this->phases.push_back(result[0]);
            this->positive_energys.push_back(result[1]);
            this->negative_energys.push_back(result[2]);
        }
    }
}

json QPEQuESTNode::SaveData(std::string file)
{
    json DataJson;
    DataJson["runs"] = this->runs;
    DataJson["trotter_order"] = this->trotter_order;
    DataJson["trotter_timestep"] = this->trotter_timestep;
    DataJson["prec_qubits"] = this->prec_qubits;
    DataJson["use_lowest_flipped_init_wavefunction"] = this->use_lowest_flipped_init_wavefunction;
    DataJson["number_qubits_state1"] = this->number_qubits_state1;

    if (State == NodeState::Calculated)
    {
        json results;
        for (int i = 0; i < this->runs; i++)
        {
            std::vector<double> result;
            result.push_back(this->phases[i]);
            result.push_back(this->positive_energys[i]);
            result.push_back(this->negative_energys[i]);
            results[std::to_string(i)] = result;
        }
        DataJson["results"] = results;
    }

    return DataJson;
}

void QPEQuESTNode::calculate()
{
    auto PauliOperator = *(_PauliOperator.lock().get());

    QC::quantum_phase_estimation_quest qpe;
    qpe.set_number_runs(this->runs);
    qpe.set_Pauli_Operator(PauliOperator);
    qpe.set_precision_qubits(this->prec_qubits);
    qpe.set_trotter_parameter(this->trotter_timestep, this->trotter_order);

    if (this->use_lowest_flipped_init_wavefunction) {
        qpe.set_initial_state_type(0);
        qpe.set_number_qubits_to_flipp_initial(this->number_qubits_state1);
    }
    else {
        auto InitState = *(_InitStateSeq.lock().get());
        
        qpe.set_initial_state_type(1);
        qpe.set_initial_statevector(InitState);
    }
    qpe.compute();

    this->phases = qpe.get_phases();
    this->positive_energys = qpe.get_positive_energys();
    this->negative_energys = qpe.get_negative_energys();
    update_AllPortData();

    State = NodeState::Calculated;
}

void QPEQuESTNode::NodeInspector(QWidget* Inspector)
{
    QVBoxLayout* lay = new QVBoxLayout();
    runs_lineedit = new QLineEdit;
    runs_lineedit->setValidator(new QIntValidator(1, 100000000, this));
    runs_lineedit->setText(QString::number(this->runs));
    connect(runs_lineedit, &QLineEdit::textChanged, this, &QPEQuESTNode::select_runs);

    trotter_order_lineedit = new QLineEdit;
    trotter_order_lineedit->setValidator(new QIntValidator(1, 10, this));
    trotter_order_lineedit->setText(QString::number(this->trotter_order));
    connect(trotter_order_lineedit, &QLineEdit::textChanged, this, &QPEQuESTNode::select_trotter_order);

    trotter_timestep_lineedit = new QLineEdit;
    trotter_timestep_lineedit->setValidator(new QDoubleValidator(0.0, 10, 3, this));
    trotter_timestep_lineedit->setText(QString::number(this->trotter_timestep));
    connect(trotter_timestep_lineedit, &QLineEdit::textChanged, this, &QPEQuESTNode::select_trotter_timestep);

    prec_qubits_lineedit = new QLineEdit;
    prec_qubits_lineedit->setValidator(new QIntValidator(1, 100000000, this));
    prec_qubits_lineedit->setText(QString::number(this->prec_qubits));
    connect(prec_qubits_lineedit, &QLineEdit::textChanged, this, &QPEQuESTNode::select_prec_qubits);

    use_lowest_flipped_init_wavefunction_box = new QCheckBox;
    use_lowest_flipped_init_wavefunction_box->setText("Use first n Qubits in 1 as Init-WF");
    use_lowest_flipped_init_wavefunction_box->setChecked(use_lowest_flipped_init_wavefunction);
    connect(use_lowest_flipped_init_wavefunction_box, &QCheckBox::stateChanged, this, &QPEQuESTNode::select_use_lowest_flipped_init_wavefunction);

    number_qubits_state1_lineedit = new QLineEdit;
    number_qubits_state1_lineedit->setValidator(new QIntValidator(1, 100000000, this));
    number_qubits_state1_lineedit->setText(QString::number(this->number_qubits_state1));
    connect(number_qubits_state1_lineedit, &QLineEdit::textChanged, this, &QPEQuESTNode::select_number_qubits_state1);

    lay->addWidget(new QLabel("Select number of runs of the algorithm"));
    lay->addWidget(runs_lineedit);
    {
        auto line = new QFrame;
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        lay->addWidget(line);
    }
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
    lay->addWidget(new QLabel("Select number of Precision-Qubits"));
    lay->addWidget(prec_qubits_lineedit);
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

    Inspector->setLayout(lay);
}

bool QPEQuESTNode::InPortAllowMultipleConnections(int Port)
{ 
    return false; 
}

unsigned int QPEQuESTNode::Ports(NodePortType PortType)
{
    if(PortType == NodePortType::In)
    {
        return 2;
    }
    else
    {
        return 0;
    }
}

bool QPEQuESTNode::IsCustomPortCaption(NodePortType PortType, int Port)
{ 
    return true; 
}

std::string QPEQuESTNode::CustomPortCaption(NodePortType PortType, int Port)
{ 
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "Pauli Operator"; }
        else if(Port == 1) {return "Statevector"; }
    }
}

std::string QPEQuESTNode::DataTypeName(NodePortType PortType, int Port)
{
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "QC::Pauli_Operator"; }
        else if(Port == 1) {return "QC::QC_Statevector"; }
    }
}

std::any QPEQuESTNode::getOutData(int Port) 
{ 
    std::any ret;
    return ret;
}

void QPEQuESTNode::setInData(int Port, std::any data)
{
    if(data.has_value())
    {
        if(Port == 0)
        {
            _PauliOperator = std::any_cast<std::weak_ptr<QC::Pauli_Operator>>(data);
        }
        if(Port == 1)
        {
            _InitStateSeq = std::any_cast<std::weak_ptr<QC::QC_Statevector>>(data);
        }
    }
    else
    {
        if(Port == 0){_PauliOperator.reset();}
        else if(Port == 1){_InitStateSeq.reset();}
    }
}

void QPEQuESTNode::select_runs()
{
    this->runs = this->runs_lineedit->text().toInt();
    PropertiesChanged();
}

void QPEQuESTNode::select_trotter_order()
{
    this->trotter_order = this->trotter_order_lineedit->text().toInt();
    PropertiesChanged();
}

void QPEQuESTNode::select_trotter_timestep()
{
    this->trotter_timestep = this->trotter_timestep_lineedit->text().toDouble();
    PropertiesChanged();
}

void QPEQuESTNode::select_prec_qubits()
{
    this->prec_qubits = this->prec_qubits_lineedit->text().toInt();
    PropertiesChanged();
}

void QPEQuESTNode::select_use_lowest_flipped_init_wavefunction()
{
    this->use_lowest_flipped_init_wavefunction = this->use_lowest_flipped_init_wavefunction_box->isChecked();
    PropertiesChanged();
}

void QPEQuESTNode::select_number_qubits_state1()
{
    this->number_qubits_state1 = this->number_qubits_state1_lineedit->text().toInt();
    PropertiesChanged();
}

void QPEQuESTNode::show_results()
{
    results_dialog = new QWidget;
    QFormLayout* form = new QFormLayout(results_dialog);
    form->addRow(new QLabel("Results of the " + QString::number(this->runs) + " run(s)"));

    QTableWidget* result_table = new QTableWidget;
    result_table->setRowCount(this->runs);
    result_table->setColumnCount(3);
    result_table->setHorizontalHeaderItem(0, new QTableWidgetItem("Phase"));
    result_table->setHorizontalHeaderItem(1, new QTableWidgetItem("Value for positive Phase"));
    result_table->setHorizontalHeaderItem(2, new QTableWidgetItem("Value for negative Phase"));
    for (int i = 0; i < this->runs; i++)
    {
        result_table->setVerticalHeaderItem(i+1, new QTableWidgetItem("Run " + QString::number(i)));
        result_table->setItem(i+1, 0, new QTableWidgetItem(QString::number(this->phases[i])));
        result_table->setItem(i+1, 1, new QTableWidgetItem(QString::number(this->positive_energys[i])));
        result_table->setItem(i+1, 2, new QTableWidgetItem(QString::number(this->negative_energys[i])));
    }

    result_table->setVerticalHeaderItem(0, new QTableWidgetItem("Expectation Value"));
    double exp_phase = 0;
    double exp_pos = 0;
    double exp_neg = 0;
    for (int i = 0; i < this->runs; i++)
    {
        exp_phase += this->phases[i] / this->runs;
        exp_pos += this->positive_energys[i] / this->runs;
        exp_neg += this->negative_energys[i] / this->runs;
    }
    result_table->setItem(0, 0, new QTableWidgetItem(QString::number(exp_phase)));
    result_table->setItem(0, 1, new QTableWidgetItem(QString::number(exp_pos)));
    result_table->setItem(0, 2, new QTableWidgetItem(QString::number(exp_neg)));

    form->addRow(result_table);
    results_dialog->show();
}