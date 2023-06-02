#include "PlotCINewNode.hpp"

PlotCINewNode::PlotCINewNode(std::string InpNodeTypeID) : Node()
{   
    NodeTypeID = InpNodeTypeID;
    NodeName = "Plot Orbitals and States";
    NodeDescription = "Plot Orbitals and States";
    NodeCategory = "Time Independent";
    Plugin = "Visualization";
    HasCalulateFunction = true;
}

PlotCINewNode::~PlotCINewNode(){}

void PlotCINewNode::LoadData(json Data, std::string file)
{

}

json PlotCINewNode::SaveData(std::string file)
{
    json DataJson;
    return DataJson;
}

void PlotCINewNode::calculate()
{
    State = NodeState::Calculated;
}

void PlotCINewNode::NodeInspector(QWidget* Inspector)
{
    QVBoxLayout* lay = new QVBoxLayout();

    QPushButton* Plot_CI_States = new QPushButton("Plot");
    connect(Plot_CI_States, &QPushButton::clicked, this, &PlotCINewNode::plot);
    lay->addWidget(Plot_CI_States);

    Inspector->setLayout(lay);
}

bool PlotCINewNode::InPortAllowMultipleConnections(int Port)
{ 
    return false; 
}

unsigned int PlotCINewNode::Ports(NodePortType PortType)
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

bool PlotCINewNode::IsCustomPortCaption(NodePortType PortType, int Port)
{ 
    return true; 
}

std::string PlotCINewNode::CustomPortCaption(NodePortType PortType, int Port)
{ 
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "CIResults"; }
        else if(Port == 1) {return "C-Matrix"; }
        else if(Port == 2) {return "Basisset"; }
        else if(Port == 3) {return "Pointcharges"; }
    }
}

std::string PlotCINewNode::DataTypeName(NodePortType PortType, int Port)
{
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "QC::CIResults"; }
        else if(Port == 1) {return "Eigen::MatrixXd"; }
        else if(Port == 2) {return "QC::Gaussian_Basisset"; }
        else if(Port == 3) {return "std::vector<QC::Pointcharge>"; }
    }
}

std::any PlotCINewNode::getOutData(int Port) 
{ 
    std::any ret;
    return ret;
}

void PlotCINewNode::setInData(int Port, std::any data)
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
            _pointcharges = std::any_cast<std::weak_ptr<std::vector<QC::Pointcharge>>>(data);
        }
    }
    else
    {
        if(Port == 0){_ci_results.reset();}
        else if(Port == 1){_hfcmatrix.reset();}
        else if(Port == 2){_basissetdata.reset();}
        else if(Port == 3){_pointcharges.reset();}
    }
}

void PlotCINewNode::plot()
{
    Plot_CI_New_Window *plotwindow = new Plot_CI_New_Window(_ci_results.lock(), _hfcmatrix.lock(), _basissetdata.lock(), _pointcharges.lock());
    plotwindow->showNormal();
};