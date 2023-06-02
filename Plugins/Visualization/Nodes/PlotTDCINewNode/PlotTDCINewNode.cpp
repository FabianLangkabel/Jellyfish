#include "PlotTDCINewNode.hpp"

PlotTDCINewNode::PlotTDCINewNode(std::string InpNodeTypeID) : Node()
{   
    NodeTypeID = InpNodeTypeID;
    NodeName = "Plot TDCI Densities";
    NodeDescription = "Plot TDCI Densities";
    NodeCategory = "Time Dependent";
    Plugin = "Visualization";
    HasCalulateFunction = true;
}

PlotTDCINewNode::~PlotTDCINewNode(){}

void PlotTDCINewNode::LoadData(json Data, std::string file)
{

}

json PlotTDCINewNode::SaveData(std::string file)
{
    json DataJson;
    return DataJson;
}

void PlotTDCINewNode::calculate()
{
    State = NodeState::Calculated;
}

void PlotTDCINewNode::NodeInspector(QWidget* Inspector)
{
    QVBoxLayout* lay = new QVBoxLayout();

    QPushButton* Plot_TDCI = new QPushButton("Plot");
    connect(Plot_TDCI, &QPushButton::clicked, this, &PlotTDCINewNode::plot);
    lay->addWidget(Plot_TDCI);

    Inspector->setLayout(lay);
}

bool PlotTDCINewNode::InPortAllowMultipleConnections(int Port)
{ 
    return false; 
}

unsigned int PlotTDCINewNode::Ports(NodePortType PortType)
{
    if(PortType == NodePortType::In)
    {
        return 5;
    }
    else
    {
        return 0;
    }
}

bool PlotTDCINewNode::IsCustomPortCaption(NodePortType PortType, int Port)
{ 
    return true; 
}

std::string PlotTDCINewNode::CustomPortCaption(NodePortType PortType, int Port)
{ 
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "CIResults"; }
        else if(Port == 1) {return "C-Matrix"; }
        else if(Port == 2) {return "Basisset"; }
        else if(Port == 3) {return "Pointcharges"; }
        else if(Port == 4) {return "TDCIVectors"; }
    }
}

std::string PlotTDCINewNode::DataTypeName(NodePortType PortType, int Port)
{
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "QC::CIResults"; }
        else if(Port == 1) {return "Eigen::MatrixXd"; }
        else if(Port == 2) {return "QC::Gaussian_Basisset"; }
        else if(Port == 3) {return "std::vector<QC::Pointcharge>"; }
        else if(Port == 4) {return "QC::TDCI_WF"; }
    }
}

std::any PlotTDCINewNode::getOutData(int Port) 
{ 
    std::any ret;
    return ret;
}

void PlotTDCINewNode::setInData(int Port, std::any data)
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
        if(Port == 4)
        {
            _tdcicoefficients = std::any_cast<std::weak_ptr<QC::TDCI_WF>>(data);
        }
    }
    else
    {
        if(Port == 0){_ci_results.reset();}
        else if(Port == 1){_hfcmatrix.reset();}
        else if(Port == 2){_basissetdata.reset();}
        else if(Port == 3){_pointcharges.reset();}
        else if(Port == 4){_tdcicoefficients.reset();}
    }
}

void PlotTDCINewNode::plot()
{
    Plot_TDCI_New_Window *plotwindow = new Plot_TDCI_New_Window(_ci_results.lock(), _hfcmatrix.lock(), _basissetdata.lock(), _pointcharges.lock(), _tdcicoefficients.lock());
    plotwindow->show();
};