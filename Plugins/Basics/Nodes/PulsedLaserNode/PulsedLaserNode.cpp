#include "PulsedLaserNode.hpp"

PulsedLaserNode::PulsedLaserNode(std::string InpNodeTypeID) : Node()
{   
    NodeTypeID = InpNodeTypeID;
    NodeName = "Laserpulse";
    NodeDescription = "Laserpulse";
    NodeCategory = "Electron Dynamics";
    Plugin = "Basics";
    HasCalulateFunction = true;
}

PulsedLaserNode::~PulsedLaserNode(){}

void PulsedLaserNode::LoadData(json Data, std::string file)
{
    this->inner_function_index = Data["inner_function_index"];
    this->outer_function_index = Data["outer_function_index"];
    this->amplitude = Data["amplitude"];
    this->frequency = Data["frequency"];
    this->polarization_index = Data["polarization_index"];

    this->polarization_vector = std::vector<double>({ 0,0,0 });
    this->polarization_vector[0] = Data["polarization_vectorX"];
    this->polarization_vector[1] = Data["polarization_vectorY"];
    this->polarization_vector[2] = Data["polarization_vectorZ"];

    this->pulse_duration = Data["pulse_duration"];
    this->cos_sqr_use_cycle_number = Data["cos_sqr_use_cycle_number"];
    this->cos_sqr_cycle_number = Data["cos_sqr_cycle_number"];
    this->FWHM = Data["FWHM"];
    this->cycle_number = Data["cycle_number"];
    this->ascend = Data["ascend"];

    if (State == NodeState::Calculated)
    {
        this->Laserpulse.set_type(QC::Laser::LaserType(this->outer_function_index));
        this->Laserpulse.set_oscillation_function(this->inner_function_index);
        this->Laserpulse.set_amplitude(this->amplitude);
        this->Laserpulse.set_frequency(this->frequency);
        //norm_polarization();
        this->Laserpulse.set_polarization(this->polarization_vector, this->polarization_index);
        this->Laserpulse.set_pulse_duration(this->pulse_duration);
        this->Laserpulse.set_use_cycle_number(false);
        
        if (this->outer_function_index == 0) {
        this->Laserpulse.set_ascention_time(this->ascend);

        }
        else if (this->outer_function_index == 1) {
            this->Laserpulse.set_fwhm(this->FWHM);
            this->Laserpulse.set_cycle_number(this->cycle_number);
        }
        else if (this->outer_function_index == 2) {
            this->Laserpulse.set_cycle_number(this->cos_sqr_cycle_number);
            this->Laserpulse.set_use_cycle_number(this->cos_sqr_use_cycle_number); //Hier ist noch was FALSCH
            if (this->cos_sqr_use_cycle_number) { update_cos_sqr_pulse_duration(); } //Hier ist noch was FALSCH Index zu Funktion Kontrollieren !!!
        }
    }
    _laserdata = std::make_shared<QC::Laser>(Laserpulse);
}

json PulsedLaserNode::SaveData(std::string file)
{
    if (State == NodeState::Calculated)
    {
    }

    json DataJson;
    DataJson["inner_function_index"] = this->inner_function_index;
    DataJson["outer_function_index"] = this->outer_function_index;
    DataJson["amplitude"] = this->amplitude;
    DataJson["frequency"] = this->frequency;
    DataJson["polarization_index"] = this->polarization_index;
    DataJson["polarization_vectorX"] = this->polarization_vector[0];
    DataJson["polarization_vectorY"] = this->polarization_vector[1];
    DataJson["polarization_vectorZ"] = this->polarization_vector[2];
    DataJson["pulse_duration"] = this->pulse_duration;
    DataJson["cos_sqr_use_cycle_number"] = this->cos_sqr_use_cycle_number;
    DataJson["cos_sqr_cycle_number"] = this->cos_sqr_cycle_number;
    DataJson["FWHM"] = this->FWHM;
    DataJson["cycle_number"] = this->cycle_number;
    DataJson["ascend"] = this->ascend;

    return DataJson;
}

void PulsedLaserNode::calculate()
{
    this->Laserpulse.set_type(QC::Laser::LaserType(this->outer_function_index));
    this->Laserpulse.set_oscillation_function(this->inner_function_index);
    this->Laserpulse.set_amplitude(this->amplitude);
    this->Laserpulse.set_frequency(this->frequency);
    //norm_polarization();
    this->Laserpulse.set_polarization(this->polarization_vector, this->polarization_index);
    this->Laserpulse.set_pulse_duration(this->pulse_duration);
    this->Laserpulse.set_use_cycle_number(false);
    
    if (this->outer_function_index == 0) {
     this->Laserpulse.set_ascention_time(this->ascend);

    }
    else if (this->outer_function_index == 1) {
        this->Laserpulse.set_fwhm(this->FWHM);
        this->Laserpulse.set_cycle_number(this->cycle_number);
    }
    else if (this->outer_function_index == 2) {
        this->Laserpulse.set_cycle_number(this->cos_sqr_cycle_number);
        this->Laserpulse.set_use_cycle_number(this->cos_sqr_use_cycle_number); //Hier ist noch was FALSCH
        if (this->cos_sqr_use_cycle_number) { update_cos_sqr_pulse_duration(); } //Hier ist noch was FALSCH Index zu Funktion Kontrollieren !!!
    }
    
   
    _laserdata = std::make_shared<QC::Laser>(Laserpulse);
    update_AllPortData();

    State = NodeState::Calculated;
}

void PulsedLaserNode::NodeInspector(QWidget* Inspector)
{
    QVBoxLayout* lay = new QVBoxLayout();

    inner_function = new QComboBox;
    inner_function->addItem("sin");
    inner_function->addItem("-sin");
    inner_function->addItem("cos");
    inner_function->addItem("-cos");
    inner_function->setCurrentIndex(this->inner_function_index);
    connect(inner_function, &QComboBox::currentTextChanged, this, &PulsedLaserNode::select_inner_function);

    outer_function = new QComboBox;
    outer_function->addItem("Cosine Squared");
    outer_function->addItem("Gauss");
    outer_function->addItem("Trapezoid");
    outer_function->setCurrentIndex(this->outer_function_index);
    connect(outer_function, &QComboBox::currentTextChanged, this, &PulsedLaserNode::select_outer_function);
    
    polarization = new QGridLayout;
    linearpolarizationButton = new QRadioButton(tr("&linear"));
    righthandpolarizationButton = new QRadioButton(tr("&r.h. circular"));
    lefthandpolarizationButton = new QRadioButton(tr("&l.h. circular"));
    if (polarization_index == 1){righthandpolarizationButton->setChecked(true); }
    else if (polarization_index == 2) {lefthandpolarizationButton->setChecked(true); }
    else { linearpolarizationButton->setChecked(true); }
    polarization->addWidget(linearpolarizationButton, 0, 0);
    polarization->addWidget(righthandpolarizationButton, 0, 1);
    polarization->addWidget(lefthandpolarizationButton, 0, 2);
    xPolarizationBox = new QLineEdit;
    xPolarizationBox->setValidator(new QDoubleValidator(-1000000, 10000000, 7, this));
    xPolarizationBox->setText(QString::number(this->polarization_vector[0]));
    QLabel* xPolLabel = new QLabel(QString("x"));
    yPolarizationBox = new QLineEdit;
    yPolarizationBox->setValidator(new QDoubleValidator(-1000000, 10000000, 7, this));
    yPolarizationBox->setText(QString::number(this->polarization_vector[1]));
    QLabel* yPolLabel = new QLabel(QString("y"));
    zPolarizationBox = new QLineEdit;
    zPolarizationBox->setValidator(new QDoubleValidator(-1000000, 10000000, 7, this));
    zPolarizationBox->setText(QString::number(this->polarization_vector[2]));
    QLabel* zPolLabel = new QLabel(QString("z"));
    polarization->addWidget(xPolLabel, 1, 0);
    polarization->addWidget(yPolLabel, 1, 1);
    polarization->addWidget(zPolLabel, 1, 2);
    polarization->addWidget(xPolarizationBox, 2, 0);
    polarization->addWidget(yPolarizationBox, 2, 1);
    polarization->addWidget(zPolarizationBox, 2, 2);
    connect(linearpolarizationButton, &QRadioButton::clicked, this, &PulsedLaserNode::select_polarization);
    connect(righthandpolarizationButton, &QRadioButton::clicked, this, &PulsedLaserNode::select_polarization);
    connect(righthandpolarizationButton, &QRadioButton::clicked, this, &PulsedLaserNode::select_polarization);

    connect(xPolarizationBox, &QLineEdit::textChanged, this, &PulsedLaserNode::select_polarization);
    connect(yPolarizationBox, &QLineEdit::textChanged, this, &PulsedLaserNode::select_polarization);
    connect(zPolarizationBox, &QLineEdit::textChanged, this, &PulsedLaserNode::select_polarization);
    //select_polarization();

    amplitude_lineedit = new QLineEdit;
    amplitude_lineedit->setValidator(new QDoubleValidator(0, 100000000, 7, this));
    amplitude_lineedit->setText(QString::number(amplitude));
    connect(amplitude_lineedit, &QLineEdit::textChanged, this, &PulsedLaserNode::select_amplitude);

    frequency_lineedit = new QLineEdit;
    frequency_lineedit->setValidator(new QDoubleValidator(0, 100000000, 7, this));
    frequency_lineedit->setText(QString::number(frequency));
    connect(frequency_lineedit, &QLineEdit::textChanged, this, &PulsedLaserNode::select_frequency);

    lay->addWidget(new QLabel("Select oscillation Function"));
    lay->addWidget(inner_function);

    auto line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    lay->addWidget(line);

    lay->addWidget(new QLabel("Select envelope Function"));
    lay->addWidget(outer_function);

    auto line2 = new QFrame;
    line2->setFrameShape(QFrame::HLine);
    line2->setFrameShadow(QFrame::Sunken);
    lay->addWidget(line2);

    lay->addWidget(new QLabel("SelectLaser-Polarization"));
    QWidget* polarline = new QWidget;
    polarline->setLayout(polarization);
    lay->addWidget(polarline);

    auto line3 = new QFrame;
    line3->setFrameShape(QFrame::HLine);
    line3->setFrameShadow(QFrame::Sunken);
    lay->addWidget(line3);

    lay->addWidget(new QLabel("Select Amplitude"));
    lay->addWidget(amplitude_lineedit);

    auto line4 = new QFrame;
    line4->setFrameShape(QFrame::HLine);
    line4->setFrameShadow(QFrame::Sunken);
    lay->addWidget(line4);

    lay->addWidget(new QLabel("Select Frequency"));
    lay->addWidget(frequency_lineedit);

    auto line5 = new QFrame;
    line5->setFrameShape(QFrame::HLine);
    line5->setFrameShadow(QFrame::Sunken);
    lay->addWidget(line5);

    cos_sq_exclusive_widgets.clear();
    gauss_exclusive_widgets.clear();
    trapezoid_exclusive_widgets.clear();
    //For Cos^2 Laser
    {
        cos_sqr_use_cycle_number_box = new QCheckBox;
        cos_sqr_use_cycle_number_box->setText("Use Cycle Number instead of Pulse Duration");
        cos_sqr_use_cycle_number_box->setChecked(cos_sqr_use_cycle_number);
        connect(cos_sqr_use_cycle_number_box, &QCheckBox::stateChanged, this, &PulsedLaserNode::select_cos_sqr_use_cycle_number);
        lay->addWidget(cos_sqr_use_cycle_number_box);
        cos_sq_exclusive_widgets.push_back(cos_sqr_use_cycle_number_box);


        cos_sqr_cycle_number_lineedit = new QLineEdit;
        cos_sqr_cycle_number_lineedit->setValidator(new QIntValidator(0, 100000000, this));
        cos_sqr_cycle_number_lineedit->setText(QString::number(cos_sqr_cycle_number));
        connect(cos_sqr_cycle_number_lineedit, &QLineEdit::textChanged, this, &PulsedLaserNode::select_cos_sqr_cycle_number);
        QLabel* text2 = new QLabel("Cycle Number");
        lay->addWidget(text2);
        cos_sq_exclusive_widgets.push_back(text2);
        lay->addWidget(cos_sqr_cycle_number_lineedit);
        cos_sq_exclusive_widgets.push_back(cos_sqr_cycle_number_lineedit);

        if (cos_sqr_use_cycle_number)
        {
            pulse_duration_lineedit->setEnabled(false);
        }
        else
        {
            cos_sqr_cycle_number_lineedit->setEnabled(false);
        }
    }

    //For Gauss Laser
    {
        FWHM_lineedit = new QLineEdit;
        FWHM_lineedit->setValidator(new QDoubleValidator(0, 100000000, 7, this));
        FWHM_lineedit->setText(QString::number(FWHM));
        connect(FWHM_lineedit, &QLineEdit::textChanged, this, &PulsedLaserNode::select_FWHM);
        QLabel* text = new QLabel("FWHM");
        lay->addWidget(text);
        gauss_exclusive_widgets.push_back(text);
        lay->addWidget(FWHM_lineedit);
        gauss_exclusive_widgets.push_back(FWHM_lineedit);

        auto line = new QFrame;
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        lay->addWidget(line);
        gauss_exclusive_widgets.push_back(line);

        cycle_number_lineedit = new QLineEdit;
        cycle_number_lineedit->setValidator(new QIntValidator(0, 100000000, this));
        cycle_number_lineedit->setText(QString::number(cycle_number));
        connect(cycle_number_lineedit, &QLineEdit::textChanged, this, &PulsedLaserNode::select_cycle_number);
        QLabel* text2 = new QLabel("Cycle Number");
        lay->addWidget(text2);
        gauss_exclusive_widgets.push_back(text2);
        lay->addWidget(cycle_number_lineedit);
        gauss_exclusive_widgets.push_back(cycle_number_lineedit);
    }


    //For Trapezoid Laser
    {


        ascend_lineedit = new QLineEdit;
        ascend_lineedit->setValidator(new QDoubleValidator(0, 100000000, 7, this));
        ascend_lineedit->setText(QString::number(ascend));
        connect(ascend_lineedit, &QLineEdit::textChanged, this, &PulsedLaserNode::select_ascend);
        QLabel* text2 = new QLabel("Ascention_time");
        lay->addWidget(text2);
        trapezoid_exclusive_widgets.push_back(text2);
        lay->addWidget(ascend_lineedit);
        trapezoid_exclusive_widgets.push_back(ascend_lineedit);
    }

    pulse_duration_lineedit = new QLineEdit;
    pulse_duration_lineedit->setValidator(new QDoubleValidator(0, 100000000, 7, this));
    pulse_duration_lineedit->setText(QString::number(pulse_duration));
    connect(pulse_duration_lineedit, &QLineEdit::textChanged, this, &PulsedLaserNode::select_pulse_duration);
    QLabel* text = new QLabel("Select Pulse Duration");
    lay->addWidget(text);
    lay->addWidget(pulse_duration_lineedit);

    update_exclusives_in_outer_function();

    QPushButton* Plot_Laser = new QPushButton("Plot Laser Pulse");
    connect(Plot_Laser, &QPushButton::clicked, this, &PulsedLaserNode::start_analyse_laser);
    lay->addWidget(Plot_Laser);

    QPushButton* Save_Laser = new QPushButton("Save Laser Pulse to File");
    connect(Save_Laser, &QPushButton::clicked, this, &PulsedLaserNode::save_laser);
    lay->addWidget(Save_Laser);

    Inspector->setLayout(lay);
}

bool PulsedLaserNode::InPortAllowMultipleConnections(int Port)
{ 
    return false; 
}

unsigned int PulsedLaserNode::Ports(NodePortType PortType)
{
    if(PortType == NodePortType::In)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

bool PulsedLaserNode::IsCustomPortCaption(NodePortType PortType, int Port)
{ 
    return true; 
}

std::string PulsedLaserNode::CustomPortCaption(NodePortType PortType, int Port)
{ 
    return "Laser";
}

std::string PulsedLaserNode::DataTypeName(NodePortType PortType, int Port)
{
    return "QC::Laser";
}

std::any PulsedLaserNode::getOutData(int Port) 
{ 
    std::any ret;
    if(Port == 0)
    {
        ret = std::make_any<std::weak_ptr<QC::Laser>>(_laserdata);
    }
    return ret;
}

void PulsedLaserNode::setInData(int Port, std::any data)
{
}







void PulsedLaserNode::select_inner_function()
{
    this->inner_function_index = this->inner_function->currentIndex();
    PropertiesChanged();
}

void PulsedLaserNode::select_outer_function()
{
    this->outer_function_index = this->outer_function->currentIndex();
    update_exclusives_in_outer_function();
    if (this->cos_sqr_use_cycle_number) { update_cos_sqr_pulse_duration(); }
    PropertiesChanged();
}

void PulsedLaserNode::select_polarization()
{
    xPolarization = xPolarizationBox->text().toDouble();
    yPolarization = yPolarizationBox->text().toDouble();
    zPolarization = zPolarizationBox->text().toDouble();
    
    if (linearpolarizationButton->isChecked()){
        polarization_index = 0;
    }
    else if (righthandpolarizationButton->isChecked()) {
        polarization_index = 1;
    }
    else {
        polarization_index = 2;
    }
    this->polarization_vector = std::vector<double>({ xPolarization, yPolarization, zPolarization });
    PropertiesChanged();
}

/*
void PulsedLaserNode::norm_polarization() {
    double pseudoamp = xPolarization * xPolarization + yPolarization * yPolarization + zPolarization * zPolarization;
    pseudoamp = sqrt(pseudoamp);
    double xPolarizationSave = xPolarization / pseudoamp;
    double yPolarizationSave = yPolarization / pseudoamp;
    double zPolarizationSave = zPolarization / pseudoamp;
    this->polarization_vector = std::vector<double>({ xPolarization, yPolarization, zPolarization });
    xPolarizationBox->setText(QString::number(xPolarizationSave));
    yPolarizationBox->setText(QString::number(yPolarizationSave));
    zPolarizationBox->setText(QString::number(zPolarizationSave));

}
*/

void PulsedLaserNode::select_amplitude()
{
    this->amplitude = this->amplitude_lineedit->text().toDouble();
    PropertiesChanged();
}

void PulsedLaserNode::select_frequency()
{
    this->frequency = this->frequency_lineedit->text().toDouble();
    if (this->cos_sqr_use_cycle_number) { update_cos_sqr_pulse_duration(); }
    PropertiesChanged();
}

void PulsedLaserNode::select_pulse_duration()
{
    this->pulse_duration = this->pulse_duration_lineedit->text().toDouble();
    PropertiesChanged();
}

void PulsedLaserNode::select_cos_sqr_use_cycle_number()
{
    this->cos_sqr_use_cycle_number = this->cos_sqr_use_cycle_number_box->isChecked();
    if (cos_sqr_use_cycle_number)
    {
        pulse_duration_lineedit->setEnabled(false);
        cos_sqr_cycle_number_lineedit->setEnabled(true);
        update_cos_sqr_pulse_duration();
    }
    else
    {
        cos_sqr_cycle_number_lineedit->setEnabled(false);
        pulse_duration_lineedit->setEnabled(true);
    }
    PropertiesChanged();
}

void PulsedLaserNode::select_cos_sqr_cycle_number()
{
    this->cos_sqr_cycle_number = this->cos_sqr_cycle_number_lineedit->text().toInt();
    if (this->cos_sqr_use_cycle_number) { update_cos_sqr_pulse_duration(); }
    PropertiesChanged();
}

void PulsedLaserNode::update_cos_sqr_pulse_duration()
{
    this->Laserpulse.set_frequency(this->frequency);
    this->Laserpulse.set_cycle_number(this->cos_sqr_cycle_number);
    //this->pulse_duration = this->Laserpulse.cyclenumber_and_frequency_to_pulse_duration();
    this->pulse_duration_lineedit->setText(QString::number(pulse_duration));
}

void PulsedLaserNode::select_FWHM()
{
    this->FWHM = this->FWHM_lineedit->text().toDouble();
    PropertiesChanged();
}

void PulsedLaserNode::select_cycle_number()
{
    this->cycle_number = this->cycle_number_lineedit->text().toInt();
    PropertiesChanged();
}

void PulsedLaserNode::select_ascend()
{
    this->ascend = this->ascend_lineedit->text().toDouble();
    PropertiesChanged();
}

void PulsedLaserNode::update_exclusives_in_outer_function()
{

    if (this->outer_function_index == 0) {
        // Cos squared
        for (int i = 0; i < this->cos_sq_exclusive_widgets.size(); i++) { this->cos_sq_exclusive_widgets[i]->setVisible(true); }
        for (int i = 0; i < this->gauss_exclusive_widgets.size(); i++) { this->gauss_exclusive_widgets[i]->setVisible(false); }
        for (int i = 0; i < this->trapezoid_exclusive_widgets.size(); i++) { this->trapezoid_exclusive_widgets[i]->setVisible(false); }
    }
    else if (this->outer_function_index == 1) {
        // Gauss
        for (int i = 0; i < this->cos_sq_exclusive_widgets.size(); i++) { this->cos_sq_exclusive_widgets[i]->setVisible(false); }
        for (int i = 0; i < this->gauss_exclusive_widgets.size(); i++) { this->gauss_exclusive_widgets[i]->setVisible(true); }
        for (int i = 0; i < this->trapezoid_exclusive_widgets.size(); i++) { this->trapezoid_exclusive_widgets[i]->setVisible(false); }
    }
    else if (this->outer_function_index == 2) {
        // Trapezoid
        for (int i = 0; i < this->cos_sq_exclusive_widgets.size(); i++) { this->cos_sq_exclusive_widgets[i]->setVisible(false); }
        for (int i = 0; i < this->gauss_exclusive_widgets.size(); i++) { this->gauss_exclusive_widgets[i]->setVisible(false); }
        for (int i = 0; i < this->trapezoid_exclusive_widgets.size(); i++) { this->trapezoid_exclusive_widgets[i]->setVisible(true); }
    }
}

void PulsedLaserNode::start_analyse_laser()
{
   Laser_Analysis_Window* laser_analysis_window = new Laser_Analysis_Window(std::make_shared<QC::Laser>(Laserpulse)); 
   laser_analysis_window->show();
}


void PulsedLaserNode::save_laser()
{
    QDialog dialog;
    QFormLayout form(&dialog);
    form.addRow(new QLabel("Save Laser to File"));

    QString timesteps = QString("Timesteps");
    QLineEdit* lineedit_timesteps = new QLineEdit(&dialog);
    lineedit_timesteps->setValidator(new QIntValidator(0, 99999999999, this));
    form.addRow(timesteps, lineedit_timesteps);

    QString stepsize = QString("Stepsize");
    QLineEdit* lineedit_stepsize = new QLineEdit(&dialog);
    lineedit_stepsize->setValidator(new QDoubleValidator(-99999999999, 99999999999, 7, this));
    form.addRow(stepsize, lineedit_stepsize);


    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    if (dialog.exec() == QDialog::Accepted) {
        QString FileName = QFileDialog::getSaveFileName(nullptr, tr("Save Data File"), QDir::homePath(), tr("txt File (*.txt)"));

        if (!FileName.isEmpty())
        {
            int timestep_number = lineedit_timesteps->text().toInt();
            Eigen::MatrixXd all_data(timestep_number, 4);

            for (int timestep = 0; timestep < timestep_number; timestep++)
            {
                double time = timestep * lineedit_stepsize->text().toDouble();
                all_data(timestep, 0) = time;
                all_data(timestep, 1) = Laserpulse.get_field_strengths(time)[0];
                all_data(timestep, 2) = Laserpulse.get_field_strengths(time)[1];
                all_data(timestep, 3) = Laserpulse.get_field_strengths(time)[2];
            }

            std::ofstream outfile;
            outfile.open(FileName.toStdString());
            outfile << "time x y z";
            outfile << std::endl;
            outfile << all_data;
            outfile.close();
        }
    }
};