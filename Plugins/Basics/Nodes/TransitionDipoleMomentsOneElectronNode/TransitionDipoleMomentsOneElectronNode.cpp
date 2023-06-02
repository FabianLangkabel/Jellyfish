#include "TransitionDipoleMomentsOneElectronNode.hpp"

TransitionDipoleMomentsOneElectronNode::TransitionDipoleMomentsOneElectronNode(std::string InpNodeTypeID) : Node()
{   
    NodeTypeID = InpNodeTypeID;
    NodeName = "Single-Electron Dipolmatrix";
    NodeDescription = "Single-Electron Dipolmatrix";
    NodeCategory = "Electronic Structure";
    Plugin = "Basics";
    HasCalulateFunction = true;
}

TransitionDipoleMomentsOneElectronNode::~TransitionDipoleMomentsOneElectronNode(){}

void TransitionDipoleMomentsOneElectronNode::LoadData(json Data, std::string file)
{
    if (State == NodeState::Calculated)
    {
        this->transitiondipolemomentsdata.clear();
        Eigen::MatrixXd temp;
        save_load::load(file, this->id, "transition_dipoles_x", &temp);
        this->transitiondipolemomentsdata.push_back(temp);
        save_load::load(file, this->id, "transition_dipoles_y", &temp);
        this->transitiondipolemomentsdata.push_back(temp);
        save_load::load(file, this->id, "transition_dipoles_z", &temp);
        this->transitiondipolemomentsdata.push_back(temp);

        _transitiondipolemoments = std::make_shared<std::vector<Eigen::MatrixXd>>(this->transitiondipolemomentsdata);
    }
}

json TransitionDipoleMomentsOneElectronNode::SaveData(std::string file)
{
    if (State == NodeState::Calculated)
    {
        save_load::save(file, this->id, "transition_dipoles_x", this->transitiondipolemomentsdata[0]);
        save_load::save(file, this->id, "transition_dipoles_y", this->transitiondipolemomentsdata[1]);
        save_load::save(file, this->id, "transition_dipoles_z", this->transitiondipolemomentsdata[2]);
    }

    json DataJson;
    return DataJson;
}

void TransitionDipoleMomentsOneElectronNode::calculate()
{
    auto basisset = *(_basisset.lock().get());
    auto hfcmatrix = *(_hfcmatrix.lock().get());

    QC::One_Electron_System_Dipole_Moment Dipole_Moment_Calc;
    QC::Integrator_libint_interface_1e Integrator;
    

    Integrator.set_basisset(basisset);
    Integrator.set_integraltype(2);
    Integrator.compute();
    Dipole_Moment_Calc.set_position_integrals_x(Integrator.get_Integrals());

    Integrator.set_basisset(basisset);
    Integrator.set_integraltype(3);
    Integrator.compute();
    Dipole_Moment_Calc.set_position_integrals_y(Integrator.get_Integrals());

    Integrator.set_basisset(basisset);
    Integrator.set_integraltype(4);
    Integrator.compute();
    Dipole_Moment_Calc.set_position_integrals_z(Integrator.get_Integrals());


    Dipole_Moment_Calc.set_HF_C_matrix(hfcmatrix);
    Dipole_Moment_Calc.compute_electronic_dipole_moment();

    this->transitiondipolemomentsdata = Dipole_Moment_Calc.get_electronic_dipole_matrices();

    _transitiondipolemoments = std::make_shared<std::vector<Eigen::MatrixXd>>(this->transitiondipolemomentsdata);
    update_AllPortData();
    
    State = NodeState::Calculated;
}

void TransitionDipoleMomentsOneElectronNode::NodeInspector(QWidget* Inspector)
{
    QVBoxLayout* lay = new QVBoxLayout();

    QPushButton* Transition_Analysis = new QPushButton("Analyse Transition Dipole Moments");
    connect(Transition_Analysis, &QPushButton::clicked, this, &TransitionDipoleMomentsOneElectronNode::start_dipole_analysis);
    lay->addWidget(Transition_Analysis);

    Inspector->setLayout(lay);
}

bool TransitionDipoleMomentsOneElectronNode::InPortAllowMultipleConnections(int Port)
{ 
    return false; 
}

unsigned int TransitionDipoleMomentsOneElectronNode::Ports(NodePortType PortType)
{
    if(PortType == NodePortType::In)
    {
        return 2;
    }
    else
    {
        return 1;
    }
}

bool TransitionDipoleMomentsOneElectronNode::IsCustomPortCaption(NodePortType PortType, int Port)
{ 
    return true; 
}

std::string TransitionDipoleMomentsOneElectronNode::CustomPortCaption(NodePortType PortType, int Port)
{ 
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "Basisset"; }
        else if(Port == 1) {return "C-Matrix"; }
    }
    else
    {
        if(Port == 0) {return "Dipolematrix"; }
    }
}

std::string TransitionDipoleMomentsOneElectronNode::DataTypeName(NodePortType PortType, int Port)
{
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "QC::Gaussian_Basisset"; }
        else if(Port == 1) {return "Eigen::MatrixXd"; }
    }
    else
    {
        if(Port == 0) {return "std::vector<Eigen::MatrixXd>"; }
    }
}

std::any TransitionDipoleMomentsOneElectronNode::getOutData(int Port) 
{ 
    std::any ret;
    ret = std::make_any<std::weak_ptr<std::vector<Eigen::MatrixXd>>>(_transitiondipolemoments);
    return ret;
}

void TransitionDipoleMomentsOneElectronNode::setInData(int Port, std::any data)
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
    }
    else
    {
        if(Port == 0){_basisset.reset();}
        else if(Port == 1){_hfcmatrix.reset();}
    }
}

void TransitionDipoleMomentsOneElectronNode::start_dipole_analysis()
{
    Transition_Dipole_Moments_Window* dipole_analysis_window = new Transition_Dipole_Moments_Window(this->transitiondipolemomentsdata);
    dipole_analysis_window->show();
};