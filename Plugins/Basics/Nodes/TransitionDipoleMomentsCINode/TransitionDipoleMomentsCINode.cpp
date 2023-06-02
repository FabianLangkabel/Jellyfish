#include "TransitionDipoleMomentsCINode.hpp"

TransitionDipoleMomentsCINode::TransitionDipoleMomentsCINode(std::string InpNodeTypeID) : Node()
{   
    NodeTypeID = InpNodeTypeID;
    NodeName = "Dipolemomatrix";
    NodeDescription = "Dipolemomatrix";
    NodeCategory = "Electronic Structure";
    Plugin = "Basics";
    HasCalulateFunction = true;
}

TransitionDipoleMomentsCINode::~TransitionDipoleMomentsCINode(){}

void TransitionDipoleMomentsCINode::LoadData(json Data, std::string file)
{
    if (State == NodeState::Calculated)
    {
        transitiondipolemomentsdata.clear();
        Eigen::MatrixXd temp;
        save_load::load(file, this->id, "transition_dipoles_x", &temp);
        transitiondipolemomentsdata.push_back(temp);
        save_load::load(file, this->id, "transition_dipoles_y", &temp);
        transitiondipolemomentsdata.push_back(temp);
        save_load::load(file, this->id, "transition_dipoles_z", &temp);
        transitiondipolemomentsdata.push_back(temp);

        _transitiondipolemoments = std::make_shared<std::vector<Eigen::MatrixXd>>(this->transitiondipolemomentsdata);
    }
}

json TransitionDipoleMomentsCINode::SaveData(std::string file)
{
    if (State == NodeState::Calculated)
    {
        save_load::save(file, this->id, "transition_dipoles_x", transitiondipolemomentsdata[0]);
        save_load::save(file, this->id, "transition_dipoles_y", transitiondipolemomentsdata[1]);
        save_load::save(file, this->id, "transition_dipoles_z", transitiondipolemomentsdata[2]);
    }

    json DataJson;
    return DataJson;
}

void TransitionDipoleMomentsCINode::calculate()
{
    auto basisset = *(_basisset.lock().get());
    auto hfcmatrix = *(_hfcmatrix.lock().get());
    QC::CIResults ci_results = *(_ci_results.lock().get());

    QC::Integrator_libint_interface_1e Integrator;
    QC::Transformation_Integrals_Basisfunction_To_HFOrbitals transform1;
    QC::Transformation_Integrals_HFOrbitals_To_Spinorbitals transform2;
    QC::Configuration_Interaction_Dipole_Moment CI_Dipols;

    Integrator.set_basisset(basisset);
    Integrator.set_integraltype(2);
    Integrator.compute();
    transform1.set_one_electron_integrals_basisfunction(Integrator.get_Integrals());
    transform1.set_hf_cmatrix(hfcmatrix);
    transform1.compute_one_electron_integrals();
    transform2.set_one_electron_integrals_hforbitals(transform1.get_one_electron_integrals_hforbitals());
    transform2.compute_one_electron_integrals();
    CI_Dipols.set_one_electron_integrals_spinorbitals_x(transform2.get_one_electron_integrals_spinorbitals());

    Integrator.set_basisset(basisset);
    Integrator.set_integraltype(3);
    Integrator.compute();
    transform1.set_one_electron_integrals_basisfunction(Integrator.get_Integrals());
    transform1.set_hf_cmatrix(hfcmatrix);
    transform1.compute_one_electron_integrals();
    transform2.set_one_electron_integrals_hforbitals(transform1.get_one_electron_integrals_hforbitals());
    transform2.compute_one_electron_integrals();
    CI_Dipols.set_one_electron_integrals_spinorbitals_y(transform2.get_one_electron_integrals_spinorbitals());

    Integrator.set_basisset(basisset);
    Integrator.set_integraltype(4);
    Integrator.compute();
    transform1.set_one_electron_integrals_basisfunction(Integrator.get_Integrals());
    transform1.set_hf_cmatrix(hfcmatrix);
    transform1.compute_one_electron_integrals();
    transform2.set_one_electron_integrals_hforbitals(transform1.get_one_electron_integrals_hforbitals());
    transform2.compute_one_electron_integrals();
    CI_Dipols.set_one_electron_integrals_spinorbitals_z(transform2.get_one_electron_integrals_spinorbitals());

    CI_Dipols.set_ci_matrix(ci_results.ci_matrix);

    std::vector<QC::Configuration> configurations;
    for (int i = 0; i < ci_results.configuration_strings.size(); i++)
    {
        QC::Configuration conf;
        for (int j = 0; j < ci_results.configuration_strings[i].size() / 2; j++) {
            if (ci_results.configuration_strings[i].substr(2 * j, 1) == "1") { conf.alphastring.push_back(1); } else { conf.alphastring.push_back(0); }
            if (ci_results.configuration_strings[i].substr(2 * j + 1, 1) == "1") { conf.betastring.push_back(1); } else { conf.betastring.push_back(0); }
        }
        configurations.push_back(conf);
    }
    CI_Dipols.set_configuration(configurations);

    if (!_pointcharges.expired())
    {
        auto Pointcharges = *(_pointcharges.lock().get());
        CI_Dipols.set_pointcharges(Pointcharges);
    }
    else
    {
        std::vector<QC::Pointcharge> Pointcharges;
        CI_Dipols.set_pointcharges(Pointcharges);
    }


    CI_Dipols.compute();
    this->transitiondipolemomentsdata = CI_Dipols.get_dipole_matrices();

    _transitiondipolemoments = std::make_shared<std::vector<Eigen::MatrixXd>>(this->transitiondipolemomentsdata);
    update_AllPortData();

    State = NodeState::Calculated;
}

void TransitionDipoleMomentsCINode::NodeInspector(QWidget* Inspector)
{
    QVBoxLayout* lay = new QVBoxLayout();

    QPushButton* Transition_Analysis = new QPushButton("Analyse Transition Dipole Moments");
    connect(Transition_Analysis, &QPushButton::clicked, this, &TransitionDipoleMomentsCINode::start_dipole_analysis);
    lay->addWidget(Transition_Analysis);

    Inspector->setLayout(lay);
}

bool TransitionDipoleMomentsCINode::InPortAllowMultipleConnections(int Port)
{ 
    return false; 
}

unsigned int TransitionDipoleMomentsCINode::Ports(NodePortType PortType)
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

bool TransitionDipoleMomentsCINode::IsCustomPortCaption(NodePortType PortType, int Port)
{ 
    return true; 
}

std::string TransitionDipoleMomentsCINode::CustomPortCaption(NodePortType PortType, int Port)
{ 
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "Basisset"; }
        else if(Port == 1) {return "C-Matrix"; }
        else if(Port == 2) {return "CIResults"; }
        else if(Port == 3) {return "Pointcharges"; }
    }
    else
    {
        if(Port == 0) {return "Dipolematrix"; }
    }
}

std::string TransitionDipoleMomentsCINode::DataTypeName(NodePortType PortType, int Port)
{
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "QC::Gaussian_Basisset"; }
        else if(Port == 1) {return "Eigen::MatrixXd"; }
        else if(Port == 2) {return "QC::CIResults"; }
        else if(Port == 3) {return "std::vector<QC::Pointcharge>"; }
    }
    else
    {
        if(Port == 0) {return "std::vector<Eigen::MatrixXd>"; }
    }
}

std::any TransitionDipoleMomentsCINode::getOutData(int Port) 
{ 
    std::any ret;
    ret = std::make_any<std::weak_ptr<std::vector<Eigen::MatrixXd>>>(_transitiondipolemoments);
    return ret;
}

void TransitionDipoleMomentsCINode::setInData(int Port, std::any data)
{
    if(data.has_value())
    {
        if(Port == 0)
        {
            _basisset = std::any_cast<std::weak_ptr<QC::Gaussian_Basisset>>(data);
        }
        else if(Port == 1)
        {
            _hfcmatrix = std::any_cast<std::weak_ptr<Eigen::MatrixXd>>(data);
        }
        else if(Port == 2)
        {
            _ci_results = std::any_cast<std::weak_ptr<QC::CIResults>>(data);
        }
        else if(Port == 3)
        {
            _pointcharges = std::any_cast<std::weak_ptr<std::vector<QC::Pointcharge>>>(data);
        }
    }
    else
    {
        if(Port == 0){_basisset.reset();}
        else if(Port == 1){_hfcmatrix.reset();}
        else if(Port == 2){_ci_results.reset();}
        else if(Port == 3){_pointcharges.reset();}
    }
}

void TransitionDipoleMomentsCINode::start_dipole_analysis()
{
    Transition_Dipole_Moments_Window* dipole_analysis_window = new Transition_Dipole_Moments_Window(this->transitiondipolemomentsdata);
    dipole_analysis_window->show();
};