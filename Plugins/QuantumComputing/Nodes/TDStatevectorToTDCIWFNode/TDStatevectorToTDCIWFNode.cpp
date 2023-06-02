#include "TDStatevectorToTDCIWFNode.hpp"

TDStatevectorToTDCIWFNode::TDStatevectorToTDCIWFNode(std::string InpNodeTypeID) : Node()
{   
    NodeTypeID = InpNodeTypeID;
    NodeName = "Statevector to TDCI";
    NodeDescription = "Statevector to TDCI";
    NodeCategory = "Evaluation";
    Plugin = "QuantumComputing";
    HasCalulateFunction = true;
}

TDStatevectorToTDCIWFNode::~TDStatevectorToTDCIWFNode(){}

void TDStatevectorToTDCIWFNode::LoadData(json Data, std::string file)
{
    if (State == NodeState::Calculated)
    {
        save_load::load(file, this->id, "tdci_wf", &tdci_wf);

        _tdci_coeffs = std::make_shared<QC::TDCI_WF>(tdci_wf);
    }
}

json TDStatevectorToTDCIWFNode::SaveData(std::string file)
{
    if (State == NodeState::Calculated)
    {
        save_load::save(file, this->id, "tdci_wf", tdci_wf);
    }

    json DataJson;
    return DataJson;
}

void TDStatevectorToTDCIWFNode::calculate()
{
    tdci_wf.clear();
    auto qctdstatevector = *(_qctdstatevector.lock().get());
    auto cimatrix = (*(_ci_results.lock().get())).ci_matrix;
    auto tdnorms = *(_tdnorm.lock().get());

    tdci_wf.add_state(0);
    std::vector<QC::QC_Statevector> Statevectors = qctdstatevector.get_td_statevector();
    std::vector<double> times = qctdstatevector.get_times();
    std::vector<double> norms = tdnorms.get_td_norm();
    int entrys = cimatrix.rows();

    for (int i = 0; i < times.size(); i++)
    {
        tdci_wf.add_time(times[i]);

        Eigen::MatrixXcd ci_vector_det_basis(entrys, 1);
        std::vector<QC::QC_Amplitude> Amps = Statevectors[i].get_amplitudes();
        for (int j = 0; j < entrys; j++)
        {
            ci_vector_det_basis(j, 0).real(Amps[j].real_amp);
            ci_vector_det_basis(j, 0).imag(Amps[j].imag_amp);
            ci_vector_det_basis(j, 0) = ci_vector_det_basis(j, 0) * sqrt(norms[i]);
        }
        Eigen::MatrixXcd cimatrix_complex = cimatrix;
        Eigen::VectorXcd x = cimatrix_complex.colPivHouseholderQr().solve(ci_vector_det_basis);
        tdci_wf.add_ci_vectors(x);
    }
    _tdci_coeffs = std::make_shared<QC::TDCI_WF>(tdci_wf);
    update_AllPortData();

    State = NodeState::Calculated;
}

void TDStatevectorToTDCIWFNode::NodeInspector(QWidget* Inspector)
{
    /*
    QVBoxLayout* lay = new QVBoxLayout();
    lay->addWidget(new QLabel("Dies ist Testnode"));

    Select_Integral = new QComboBox;
    Select_Integral->addItem("Overlapintegrals <i|j>");
    Select_Integral->addItem("Kinetic Energy Integrals <i|Kin|j>");
    Select_Integral->addItem("Nuclear Repulsion Integrals <i|Nuc|j>");
    Select_Integral->addItem("Kinetic + Nuclear Integrals <i|Kin + Nuc|j>");
    Select_Integral->addItem("<i|x|j>");
    Select_Integral->addItem("<i|y|j>");
    Select_Integral->addItem("<i|z|j>");
    Select_Integral->setCurrentIndex(select_integral_id);
    connect(Select_Integral, &QComboBox::currentTextChanged, this, &TDStatevectorToTDCIWFNode::selectintegral);

    lay->addWidget(new QLabel("Select Integral Type"));
    lay->addWidget(Select_Integral);

    Inspector->setLayout(lay);
    */
}

bool TDStatevectorToTDCIWFNode::InPortAllowMultipleConnections(int Port)
{ 
    return false; 
}

unsigned int TDStatevectorToTDCIWFNode::Ports(NodePortType PortType)
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

bool TDStatevectorToTDCIWFNode::IsCustomPortCaption(NodePortType PortType, int Port)
{ 
    return true; 
}

std::string TDStatevectorToTDCIWFNode::CustomPortCaption(NodePortType PortType, int Port)
{ 
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "TD Statevector"; }
        else if(Port == 1) {return "CI Results"; }
        else if(Port == 2) {return "TD Norm"; }
    }
    else
    {
        if(Port == 0) {return "TDCIVectors"; }
    }
}

std::string TDStatevectorToTDCIWFNode::DataTypeName(NodePortType PortType, int Port)
{
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "QC::QC_TD_Statevectors"; }
        else if(Port == 1) {return "QC::CIResults"; }
        else if(Port == 2) {return "QC::QC_TD_Norm"; }
    }
    else
    {
        if(Port == 0) {return "QC::TDCI_WF"; }
    }
}

std::any TDStatevectorToTDCIWFNode::getOutData(int Port) 
{ 
    std::any ret;
    if(Port == 0)
    {
        ret = std::make_any<std::weak_ptr<QC::TDCI_WF>>(_tdci_coeffs);
    }
    return ret;
}

void TDStatevectorToTDCIWFNode::setInData(int Port, std::any data)
{
    if(data.has_value())
    {
        if(Port == 0)
        {
            _qctdstatevector = std::any_cast<std::weak_ptr<QC::QC_TD_Statevectors>>(data);
        }
        if(Port == 1)
        {
            _ci_results = std::any_cast<std::weak_ptr<QC::CIResults>>(data);
        }
        if(Port == 2)
        {
            _tdnorm = std::any_cast<std::weak_ptr<QC::QC_TD_Norm>>(data);
        }
    }
    else
    {
        if(Port == 0){_qctdstatevector.reset();}
        else if(Port == 1){_ci_results.reset();}
        else if(Port == 2){_tdnorm.reset();}
    }
}


void TDStatevectorToTDCIWFNode::start_analyse_propagation()
{
    /*
    Eigen::VectorXd test;
    TDWF_Analysis_Window* tdwf_analysis_window = new TDWF_Analysis_Window(&this->tdci_wf, test, std::weak_ptr<TransitionDipoleMomentsData>(), std::weak_ptr<LaserData>());
    tdwf_analysis_window->show();
    */
}