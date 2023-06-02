#include "EvaluateNTONode.hpp"

EvaluateNTONode::EvaluateNTONode(std::string InpNodeTypeID) : Node()
{   
    NodeTypeID = InpNodeTypeID;
    NodeName = "Exciton Descriptors";
    NodeDescription = "Exciton Descriptors";
    NodeCategory = "Evaluation";
    Plugin = "Basics";
    HasCalulateFunction = true;
}

EvaluateNTONode::~EvaluateNTONode(){}

void EvaluateNTONode::calculate()
{
    State = NodeState::Calculated;
}

void EvaluateNTONode::NodeInspector(QWidget* Inspector)
{
    QVBoxLayout* lay = new QVBoxLayout();
    QPushButton* Evaluate = new QPushButton("Evaluate");
    connect(Evaluate, &QPushButton::clicked, this, &EvaluateNTONode::show_evaluation_window);
    lay->addWidget(Evaluate);

    Inspector->setLayout(lay);
}

bool EvaluateNTONode::InPortAllowMultipleConnections(int Port)
{ 
    return false; 
}

unsigned int EvaluateNTONode::Ports(NodePortType PortType)
{
    if(PortType == NodePortType::In)
    {
        return 4;
    }
    else
    {
        return 0;
    }
}

bool EvaluateNTONode::IsCustomPortCaption(NodePortType PortType, int Port)
{ 
    return true; 
}

std::string EvaluateNTONode::CustomPortCaption(NodePortType PortType, int Port)
{ 
    if(Port == 0) {return "CIResults"; }
    else if(Port == 1) {return "C-Matrix"; }
    else if(Port == 2) {return "Basisset"; }
    else if(Port == 3) {return "TDCIVectors"; }
}

std::string EvaluateNTONode::DataTypeName(NodePortType PortType, int Port)
{
    if(Port == 0) {return "QC::CIResults"; }
    else if(Port == 1) {return "Eigen::MatrixXd"; }
    else if(Port == 2) {return "QC::Gaussian_Basisset"; }
    else if(Port == 3) {return "QC::TDCI_WF"; }
}

std::any EvaluateNTONode::getOutData(int Port) 
{ 
    std::any ret;
    return ret;
}

void EvaluateNTONode::setInData(int Port, std::any data)
{
    if(data.has_value())
    {
        if(Port == 0)
        {
            _ci_results = std::any_cast<std::weak_ptr<QC::CIResults>>(data);
        }
        if(Port == 1)
        {
            _hfcmatrix = std::any_cast<std::weak_ptr<Eigen::MatrixXd>>(data);
        }
        if(Port == 2)
        {
            _basissetdata = std::any_cast<std::weak_ptr<QC::Gaussian_Basisset>>(data);
        }
        if(Port == 3)
        {
            _tdcicoefficients = std::any_cast<std::weak_ptr<QC::TDCI_WF>>(data);
        }
    }
    else
    {
        if(Port == 0){_ci_results.reset();}
        else if(Port == 1){_hfcmatrix.reset();}
        else if(Port == 2){_basissetdata.reset();}
        else if(Port == 3){_tdcicoefficients.reset();}
    }
}

void EvaluateNTONode::show_evaluation_window()
{
    Evaluate_NTOs_Window* window = new Evaluate_NTOs_Window(_ci_results.lock(), _hfcmatrix.lock(), _basissetdata.lock(), _tdcicoefficients.lock());
    window->show();
};