#include "CreateStateFromCINode.hpp"

CreateStateFromCINode::CreateStateFromCINode(std::string InpNodeTypeID) : Node()
{   
    NodeTypeID = InpNodeTypeID;
    NodeName = "Statevector from CI State";
    NodeDescription = "Statevector from CI State";
    NodeCategory = "State Preparation";
    Plugin = "QuantumComputing";
    HasCalulateFunction = true;
}

CreateStateFromCINode::~CreateStateFromCINode(){}

void CreateStateFromCINode::LoadData(json Data, std::string file)
{
    this->state = Data["state"];

    if (State == NodeState::Calculated)
    {
        save_load::load(file, this->id, "statevector", &Statevector);
        _initial_state = std::make_shared<QC::QC_Statevector>(this->Statevector);
    }
}

json CreateStateFromCINode::SaveData(std::string file)
{
    
    if (State == NodeState::Calculated)
    {
        save_load::save(file, this->id, "statevector", Statevector);
    }

    json DataJson;
    DataJson["state"] = this->state;
    return DataJson;
}

void CreateStateFromCINode::calculate()
{
    QC::CIResults ci_results = *(_ci_results.lock().get());

    this->Statevector.clear_amplitudes();
    Eigen::VectorXd state_vec_from_ci_matrix = ci_results.ci_matrix.col(this->state);
    for (int i = 0; i < ci_results.configuration_strings.size(); i++)
    {
        std::string confstring = ci_results.configuration_strings[i];
        reverse(confstring.begin(), confstring.end());

        int decimal = std::stoi(confstring, 0, 2);
        /*
        long long int binary = std::stoll(confstring);

        //Binary to Decimal
        int decimal = 0;
        {
            int base = 1;
            long long int temp = binary;
            while (temp) {
                int lastDigit = temp % 10;
                temp = temp / 10;
                decimal += lastDigit * base;
                base = base * 2;
            }
        }
        */
        this->Statevector.add_amplitude(decimal, state_vec_from_ci_matrix[i], 0);
    }

    _initial_state = std::make_shared<QC::QC_Statevector>(this->Statevector);
    update_AllPortData();

    State = NodeState::Calculated;
}

void CreateStateFromCINode::NodeInspector(QWidget* Inspector)
{
    QVBoxLayout* lay = new QVBoxLayout();
    state_lineedit = new QLineEdit;
    state_lineedit->setValidator(new QIntValidator(0, 100000000, this));
    state_lineedit->setText(QString::number(this->state));
    connect(state_lineedit, &QLineEdit::textChanged, this, &CreateStateFromCINode::select_state);
    lay->addWidget(new QLabel("Select a State from all CI-States"));
    lay->addWidget(state_lineedit);

    Inspector->setLayout(lay);
}

bool CreateStateFromCINode::InPortAllowMultipleConnections(int Port)
{ 
    return false; 
}

unsigned int CreateStateFromCINode::Ports(NodePortType PortType)
{
    if(PortType == NodePortType::In)
    {
        return 1;
    }
    else
    {
        return 1;
    }
}

bool CreateStateFromCINode::IsCustomPortCaption(NodePortType PortType, int Port)
{ 
    return true; 
}

std::string CreateStateFromCINode::CustomPortCaption(NodePortType PortType, int Port)
{ 
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "CI Results"; }
    }
    else
    {
        if(Port == 0) {return "Statevector"; }
    }
}

std::string CreateStateFromCINode::DataTypeName(NodePortType PortType, int Port)
{
    if(PortType == NodePortType::In)
    {
        if(Port == 0) {return "QC::CIResults"; }
    }
    else
    {
        if(Port == 0) {return "QC::QC_Statevector"; }
    }
}

std::any CreateStateFromCINode::getOutData(int Port) 
{ 
    std::any ret;
    if(Port == 0)
    {
        ret = std::make_any<std::weak_ptr<QC::QC_Statevector>>(_initial_state);
    }
    return ret;
}

void CreateStateFromCINode::setInData(int Port, std::any data)
{
    if(data.has_value())
    {
        if(Port == 0)
        {
            _ci_results = std::any_cast<std::weak_ptr<QC::CIResults>>(data);
        }
    }
    else
    {
        if(Port == 0){_ci_results.reset();}
    }
}

void CreateStateFromCINode::select_state()
{
    this->state = this->state_lineedit->text().toInt();
    PropertiesChanged();
}