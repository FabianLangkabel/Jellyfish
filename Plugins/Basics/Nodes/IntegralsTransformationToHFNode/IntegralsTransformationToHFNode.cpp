#include "IntegralsTransformationToHFNode.hpp"

IntegralsTransformationToHFNode::IntegralsTransformationToHFNode(std::string InpNodeTypeID) : Node()
{   
    NodeTypeID = InpNodeTypeID;
    NodeName = "Transformation to MO";
    NodeDescription = "Transformation to MO";
    NodeCategory = "Integrals";
    Plugin = "Basics";
    HasCalulateFunction = true;
}

IntegralsTransformationToHFNode::~IntegralsTransformationToHFNode(){}

void IntegralsTransformationToHFNode::LoadData(json Data, std::string file)
{
    if (State == NodeState::Calculated)
    {
        save_load::load(file, this->id, "one_e_integrals", &one_e_integrals);
        save_load::load(file, this->id, "two_e_integrals", &two_e_integrals);

        _oneelectronintegralsout = std::make_shared<Eigen::MatrixXd>(this->one_e_integrals);
        _twoelectronintegralsout = std::make_shared<Eigen::Tensor<double, 4>>(this->two_e_integrals);
    }
}

json IntegralsTransformationToHFNode::SaveData(std::string file)
{
    if (State == NodeState::Calculated)
    {
        save_load::save(file, this->id, "one_e_integrals", one_e_integrals);
        save_load::save(file, this->id, "two_e_integrals", two_e_integrals);
    }

    json DataJson;
    return DataJson;
}

void IntegralsTransformationToHFNode::calculate()
{
    QC::Transformation_Integrals_Basisfunction_To_HFOrbitals transformation_class;

    auto HFCMatrix = *(_HFCMatrix.lock().get());
    transformation_class.set_hf_cmatrix(HFCMatrix);

    if (!_oneelectronintegralsin.expired())
    {
        auto OneElectronIntegral = *(_oneelectronintegralsin.lock().get());
        transformation_class.set_one_electron_integrals_basisfunction(OneElectronIntegral);
        transformation_class.compute_one_electron_integrals();
        this->one_e_integrals = transformation_class.get_one_electron_integrals_hforbitals();
        _oneelectronintegralsout = std::make_shared<Eigen::MatrixXd>(this->one_e_integrals);
    }

    if (!_twoelectronintegralsin.expired())
    {
        auto TwoElectronIntegral = *(_twoelectronintegralsin.lock().get());
        transformation_class.set_two_electron_integrals_basisfunction(TwoElectronIntegral);
        transformation_class.compute_two_electron_integrals();
        this->two_e_integrals = transformation_class.get_two_electron_integrals_hforbitals();
        _twoelectronintegralsout = std::make_shared<Eigen::Tensor<double, 4>>(this->two_e_integrals);
    }
    update_AllPortData();
    
    State = NodeState::Calculated;
}

void IntegralsTransformationToHFNode::NodeInspector(QWidget* Inspector)
{
    QVBoxLayout* lay = new QVBoxLayout();
    Inspector->setLayout(lay);
}

bool IntegralsTransformationToHFNode::InPortAllowMultipleConnections(int Port)
{ 
    return false; 
}

unsigned int IntegralsTransformationToHFNode::Ports(NodePortType PortType)
{
    if(PortType == NodePortType::In)
    {
        return 3;
    }
    else
    {
        return 2;
    }
}

bool IntegralsTransformationToHFNode::IsCustomPortCaption(NodePortType PortType, int Port)
{ 
    return true; 
}

std::string IntegralsTransformationToHFNode::CustomPortCaption(NodePortType PortType, int Port)
{ 
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "1e Integrals"; }
        else if(Port == 1) {return "2e Integrals"; }
        else if(Port == 2) {return "C-Matrix"; }
    }
    else
    {
        if(Port == 0) {return "1e Integrals"; }
        else if(Port == 1) {return "2e Integrals"; }
    }
}

std::string IntegralsTransformationToHFNode::DataTypeName(NodePortType PortType, int Port)
{
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "Eigen::MatrixXd"; }
        else if(Port == 1) {return "Eigen::Tensor<double, 4>"; }
        else if(Port == 2) {return "Eigen::MatrixXd"; }
    }
    else
    {
        if(Port == 0) {return "Eigen::MatrixXd"; }
        else if(Port == 1) {return "Eigen::Tensor<double, 4>"; }
    }
}

std::any IntegralsTransformationToHFNode::getOutData(int Port) 
{ 
    std::any ret;
    if(Port == 0)
    {
        ret = std::make_any<std::weak_ptr<Eigen::MatrixXd>>(_oneelectronintegralsout);
    }
    else
    {
        ret = std::make_any<std::weak_ptr<Eigen::Tensor<double, 4>>>(_twoelectronintegralsout);
    }
    return ret;
}

void IntegralsTransformationToHFNode::setInData(int Port, std::any data)
{
    if(data.has_value())
    {
        if(Port == 0)
        {
            _oneelectronintegralsin = std::any_cast<std::weak_ptr<Eigen::MatrixXd>>(data);
        }
        if(Port == 1)
        {
            _twoelectronintegralsin = std::any_cast<std::weak_ptr<Eigen::Tensor<double, 4>>>(data);
        }
        if(Port == 2)
        {
            _HFCMatrix = std::any_cast<std::weak_ptr<Eigen::MatrixXd>>(data);
        }
    }
    else
    {
        if(Port == 0){_oneelectronintegralsin.reset();}
        else if(Port == 1){_twoelectronintegralsin.reset();}
        else if(Port == 2){_HFCMatrix.reset();}
    }
};