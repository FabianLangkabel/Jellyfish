#include "TDExpHadamardTestQuESTNode.hpp"

TDExpHadamardTestQuESTNode::TDExpHadamardTestQuESTNode(std::string InpNodeTypeID) : Node()
{   
    NodeTypeID = InpNodeTypeID;
    NodeName = "TD Hadamard Test (QuEST)";
    NodeDescription = "TD Hadamard Test (QuEST)";
    NodeCategory = "Algorithms";
    Plugin = "QuantumComputing";
    HasCalulateFunction = true;
}

TDExpHadamardTestQuESTNode::~TDExpHadamardTestQuESTNode(){}

void TDExpHadamardTestQuESTNode::LoadData(json Data, std::string file)
{
    this->skipped_PropaTimeSteps = Data["skipped_PropaTimeSteps"];
    this->runs = Data["runs"];
    this->prefactor = Data["prefactor"];

    if (State == NodeState::Calculated)
    {
        save_load::load(file, this->id, "Times", &times);
        save_load::load(file, this->id, "TD_Values", &td_vals);
    }
}

json TDExpHadamardTestQuESTNode::SaveData(std::string file)
{
    if (State == NodeState::Calculated)
    {
        save_load::save(file, this->id, "Times", times);
        save_load::save(file, this->id, "TD_Values", td_vals);
    }

    json DataJson;
    DataJson["skipped_PropaTimeSteps"] = this->skipped_PropaTimeSteps;
    DataJson["runs"] = this->runs;
    DataJson["prefactor"] = this->prefactor;
    return DataJson;
}

void TDExpHadamardTestQuESTNode::calculate()
{
    auto PauliOperator = *(_PauliOperator.lock().get());
    auto qctdstatevector = *(_tdstatevectors.lock().get());

    std::vector<QC::QC_Statevector> TDStatevectors = qctdstatevector.get_td_statevector();
    std::vector<double> times = qctdstatevector.get_times();
    int timesteps = times.size();
    int number_timesteps_to_evaluate = timesteps / (this->skipped_PropaTimeSteps + 1);
    this->times = Eigen::VectorXd(number_timesteps_to_evaluate);
    this->td_vals = Eigen::VectorXd(number_timesteps_to_evaluate);

    for (int i = 0; i < number_timesteps_to_evaluate; i++)
    {
        int m = i * (this->skipped_PropaTimeSteps + 1);
        std::cout << "Evaluate Propagation-Step: " << m << std::endl;

        QC::hadamard_test_pauli_sum_quest had_test;
        had_test.set_prefactor(this->prefactor);
        had_test.set_number_runs(this->runs);
        had_test.set_Pauli_Operator(PauliOperator);
        had_test.set_initial_statevector(TDStatevectors[m]);
        had_test.compute();

        this->times(i) = times[m];
        this->td_vals(i) = had_test.get_expectation_value();
    }

    /*
    std::vector<QC::QC_Statevector> TDStatevectors = qctdstatevector.get_td_statevector();

    int timesteps = qctdstatevector.get_times().size();
    int number_timesteps_to_evaluate = (timesteps - 1) / (this->skipped_PropaTimeSteps + 1);
    this->td_phases = Eigen::MatrixXd(this->runs, number_timesteps_to_evaluate);
    this->td_pos_values = Eigen::MatrixXd(this->runs, number_timesteps_to_evaluate);
    this->td_neg_values = Eigen::MatrixXd(this->runs, number_timesteps_to_evaluate);

    for(int i = 0; i < number_timesteps_to_evaluate; i++)
    {
        int m = i * (this->skipped_PropaTimeSteps + 1);
        std::cout << "Evaluate Propagation-Step: " << m << std::endl;

        QC::quantum_phase_estimation_quest qpe;
        qpe.set_number_runs(this->runs);
        qpe.set_Pauli_Operator(PauliOperator);
        qpe.set_precision_qubits(this->prec_qubits);
        qpe.set_trotter_parameter(this->trotter_timestep, this->trotter_order);
        qpe.set_initial_state_type(1);
        qpe.set_initial_statevector(TDStatevectors[m]);
        qpe.compute();

        std::vector<double> phases = qpe.get_phases();
        std::vector<double> pos_values = qpe.get_positive_energys();
        std::vector<double> neg_values = qpe.get_negative_energys();

        for (int j = 0; j < this->runs; j++)
        {
            this->td_phases(j, i) = phases[j];
            this->td_pos_values(j, i) = pos_values[j];
            this->td_neg_values(j, i) = neg_values[j];
        }
    }
    */
    update_AllPortData();

    State = NodeState::Calculated;
}

void TDExpHadamardTestQuESTNode::NodeInspector(QWidget* Inspector)
{
    QVBoxLayout* lay = new QVBoxLayout();
    skipped_PropaTimeSteps_lineedit = new QLineEdit;
    skipped_PropaTimeSteps_lineedit->setValidator(new QIntValidator(0, 100000000, this));
    skipped_PropaTimeSteps_lineedit->setText(QString::number(this->skipped_PropaTimeSteps));
    connect(skipped_PropaTimeSteps_lineedit, &QLineEdit::textChanged, this, &TDExpHadamardTestQuESTNode::select_skipped_PropaTimeSteps);

    runs_lineedit = new QLineEdit;
    runs_lineedit->setValidator(new QIntValidator(1, 100000000, this));
    runs_lineedit->setText(QString::number(this->runs));
    connect(runs_lineedit, &QLineEdit::textChanged, this, &TDExpHadamardTestQuESTNode::select_runs);

    prefactor_lineedit = new QLineEdit;
    prefactor_lineedit->setValidator(new QDoubleValidator(0.0, 10, 10, this));
    prefactor_lineedit->setText(QString::number(this->prefactor));
    connect(prefactor_lineedit, &QLineEdit::textChanged, this, &TDExpHadamardTestQuESTNode::select_prefactor);

    {
        QLabel* info = new QLabel("Select number of timesteps to skip between between the propagation steps that are evaluated");
        info->setWordWrap(true);
        lay->addWidget(info);
        lay->addWidget(skipped_PropaTimeSteps_lineedit);
    }
    {
        auto line = new QFrame;
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        lay->addWidget(line);
    }
    lay->addWidget(new QLabel("Select number of runs of the algorithm"));
    lay->addWidget(runs_lineedit);
    {
        auto line = new QFrame;
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        lay->addWidget(line);
    }
    lay->addWidget(new QLabel("Select prefactor"));
    lay->addWidget(prefactor_lineedit);

    Inspector->setLayout(lay);
}

bool TDExpHadamardTestQuESTNode::InPortAllowMultipleConnections(int Port)
{ 
    return false; 
}

unsigned int TDExpHadamardTestQuESTNode::Ports(NodePortType PortType)
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

bool TDExpHadamardTestQuESTNode::IsCustomPortCaption(NodePortType PortType, int Port)
{ 
    return true; 
}

std::string TDExpHadamardTestQuESTNode::CustomPortCaption(NodePortType PortType, int Port)
{ 
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "Pauli Operator"; }
        else if(Port == 1) {return "TD Statevector"; }
    }
}

std::string TDExpHadamardTestQuESTNode::DataTypeName(NodePortType PortType, int Port)
{
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "QC::Pauli_Operator"; }
        else if(Port == 1) {return "QC::QC_TD_Statevectors"; }
    }
}

std::any TDExpHadamardTestQuESTNode::getOutData(int Port) 
{ 
    std::any ret;
    return ret;
}

void TDExpHadamardTestQuESTNode::setInData(int Port, std::any data)
{
    if(data.has_value())
    {
        if(Port == 0)
        {
            _PauliOperator = std::any_cast<std::weak_ptr<QC::Pauli_Operator>>(data);
        }
        if(Port == 1)
        {
            _tdstatevectors = std::any_cast<std::weak_ptr<QC::QC_TD_Statevectors>>(data);
        }
    }
    else
    {
        if(Port == 0){_PauliOperator.reset();}
        else if(Port == 1){_tdstatevectors.reset();}
    }
}


void TDExpHadamardTestQuESTNode::select_runs()
{
    this->runs = this->runs_lineedit->text().toInt();
    PropertiesChanged();
}

void TDExpHadamardTestQuESTNode::select_skipped_PropaTimeSteps()
{
    this->skipped_PropaTimeSteps = this->skipped_PropaTimeSteps_lineedit->text().toInt();
    PropertiesChanged();
}

void TDExpHadamardTestQuESTNode::select_prefactor()
{
    this->prefactor = this->prefactor_lineedit->text().toDouble();
    PropertiesChanged();
}

void TDExpHadamardTestQuESTNode::show_results()
{
    results_dialog = new QWidget;
    QFormLayout* form = new QFormLayout(results_dialog);

    QChart* chart = new QChart;
    QChartView* chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    chart->setTitle("Results");
    QSplineSeries* series = new QSplineSeries();

    for (int timestep = 0; timestep < this->times.size(); timestep++)
    {
        double time = this->times[timestep];
        series->append(time, this->td_vals[timestep]);
    }
    chart->addSeries(series);
    chart->createDefaultAxes();
    form->addRow(chartView);

    QPushButton* save_button = new QPushButton("Save");
    connect(save_button, &QPushButton::clicked, this, &TDExpHadamardTestQuESTNode::save_results);
    form->addRow(save_button);

    results_dialog->show();
}

void TDExpHadamardTestQuESTNode::save_results()
{
    QString FileName = QFileDialog::getSaveFileName(nullptr, tr("Save Data File"), QDir::homePath(), tr("txt File (*.txt)"));

    if (!FileName.isEmpty())
    {
        std::ofstream outfile;
        outfile.open(FileName.toStdString());
        outfile << "Time ExpectationValue" << std::endl;
        for (int i = 0; i < this->times.size(); i++)
        {
            outfile << this->times[i] << " " << this->td_vals[i] << std::endl;
        }
        outfile.close();
    }
}