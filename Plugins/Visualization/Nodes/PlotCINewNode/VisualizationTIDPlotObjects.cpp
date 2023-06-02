#include "VisualizationTIDPlotObjects.h"



//************************* Basisfunction *************************
VisualizationTIDPlotBasisfunctionWidget::VisualizationTIDPlotBasisfunctionWidget(std::shared_ptr<QC::Gaussian_Basisset> basisset, QWidget *parent) : QGroupBox(parent)
{
    QVBoxLayout* Layout = new QVBoxLayout;
    BasisfunctionList = new QListWidget();

    std::vector<std::tuple<QC::Gaussian_Basisfunction, int>> basisfunctionsandangularmoment = basisset->get_all_Basisfunction_with_angular_expanded();
    for(int i = 0; i < basisfunctionsandangularmoment.size(); i++)
    {
        QC::Gaussian_Basisfunction basisfunction;
		int angular_moment;
        std::tie(basisfunction, angular_moment) = basisfunctionsandangularmoment[i];
        std::string ItemString = "Basisfunction " + std::to_string(i) + " (l = " + std::to_string(basisfunction.get_total_angular_moment()) + ", m = " + std::to_string(angular_moment) + ")";
        BasisfunctionList->addItem(QString::fromStdString(ItemString));
    }
    BasisfunctionList->setCurrentRow(0);
    connect(BasisfunctionList, &QListWidget::currentRowChanged, this, &VisualizationTIDPlotBasisfunctionWidget::ParameterChanged);
    Layout->addWidget(new QLabel("Select Basisfunction"));
    Layout->addWidget(BasisfunctionList);
    setLayout(Layout);
}

void VisualizationTIDPlotBasisfunctionWidget::ParameterChanged()
{
    selected_Basisfunction = BasisfunctionList->currentRow();
}

//************************* Molecule Orbitals *************************
VisualizationTIDPlotMoleculeOrbitalsWidget::VisualizationTIDPlotMoleculeOrbitalsWidget(int MONumbers, QWidget *parent) : QGroupBox(parent)
{
    QVBoxLayout* Layout = new QVBoxLayout;
    MOList = new QListWidget();
    for(int i = 0; i < MONumbers; i++)
    {
        std::string ItemString = "Molecule Orbital " + std::to_string(i);
        MOList->addItem(QString::fromStdString(ItemString));
    }
    MOList->setCurrentRow(0);
    connect(MOList, &QListWidget::currentRowChanged, this, &VisualizationTIDPlotMoleculeOrbitalsWidget::ParameterChanged);
    Layout->addWidget(new QLabel("Molecule Orbital"));
    Layout->addWidget(MOList);
    setLayout(Layout);
}

void VisualizationTIDPlotMoleculeOrbitalsWidget::ParameterChanged()
{
    selected_MO = MOList->currentRow();
}

//************************* Molecule Orbital Density *************************
VisualizationTIDPlotMoleculeOrbitalDensityWidget::VisualizationTIDPlotMoleculeOrbitalDensityWidget(int MONumbers, QWidget *parent) : QGroupBox(parent)
{
    QVBoxLayout* Layout = new QVBoxLayout;
    MOList = new QListWidget();
    for(int i = 0; i < MONumbers; i++)
    {
        std::string ItemString = "Molecule Orbital " + std::to_string(i);
        MOList->addItem(QString::fromStdString(ItemString));
    }
    MOList->setCurrentRow(0);
    connect(MOList, &QListWidget::currentRowChanged, this, &VisualizationTIDPlotMoleculeOrbitalDensityWidget::ParameterChanged);
    Layout->addWidget(new QLabel("Select Molecule Orbital"));
    Layout->addWidget(MOList);
    setLayout(Layout);
}

void VisualizationTIDPlotMoleculeOrbitalDensityWidget::ParameterChanged()
{
    selected_MO = MOList->currentRow();
}

//************************* CI-State Density *************************
VisualizationTIDPlotCIStateDensityWidget::VisualizationTIDPlotCIStateDensityWidget(std::shared_ptr <QC::CIResults> ciresults, QWidget *parent) : QGroupBox(parent)
{
    QVBoxLayout* Layout = new QVBoxLayout;
    CIStateList = new QListWidget();
    Eigen::VectorXd Energys = (*(ciresults.get())).ci_values;
    for(int i = 0; i < Energys.size(); i++)
    {
        std::string ItemString = "CI-State " + std::to_string(i) + " (Energy: " + std::to_string(Energys[i]) + ")";
        CIStateList->addItem(QString::fromStdString(ItemString));
    }
    CIStateList->setCurrentRow(0);
    connect(CIStateList, &QListWidget::currentRowChanged, this, &VisualizationTIDPlotCIStateDensityWidget::ParameterChanged);
    Layout->addWidget(new QLabel("Select CI-State"));
    Layout->addWidget(CIStateList);
    setLayout(Layout);
}

void VisualizationTIDPlotCIStateDensityWidget::ParameterChanged()
{
    selected_State = CIStateList->currentRow();
}

//************************* CI-State Density Difference *************************
VisualizationTIDPlotCIStateDensityDifferenceWidget::VisualizationTIDPlotCIStateDensityDifferenceWidget(std::shared_ptr <QC::CIResults> ciresults, QWidget *parent) : QGroupBox(parent)
{
    QVBoxLayout* Layout = new QVBoxLayout;
    CIStateList1 = new QListWidget();
    CIStateList2 = new QListWidget();
    Eigen::VectorXd Energys = (*(ciresults.get())).ci_values;
    for(int i = 0; i < Energys.size(); i++)
    {
        std::string ItemString = "CI-State " + std::to_string(i) + " (Energy: " + std::to_string(Energys[i]) + ")";
        CIStateList1->addItem(QString::fromStdString(ItemString));
        CIStateList2->addItem(QString::fromStdString(ItemString));
    }
    CIStateList1->setCurrentRow(0);
    CIStateList2->setCurrentRow(0);
    connect(CIStateList1, &QListWidget::currentRowChanged, this, &VisualizationTIDPlotCIStateDensityDifferenceWidget::ParameterChanged);
    connect(CIStateList2, &QListWidget::currentRowChanged, this, &VisualizationTIDPlotCIStateDensityDifferenceWidget::ParameterChanged);
    Layout->addWidget(new QLabel("Select CI-State 1"));
    Layout->addWidget(CIStateList1);
    Layout->addWidget(new QLabel("Select CI-State 2"));
    Layout->addWidget(CIStateList2);
    setLayout(Layout);
}

void VisualizationTIDPlotCIStateDensityDifferenceWidget::ParameterChanged()
{
    selected_State1 = CIStateList1->currentRow();
    selected_State2 = CIStateList2->currentRow();
}

//************************* Natural Transition Orbitals *************************
VisualizationTIDPlotNTOWidget::VisualizationTIDPlotNTOWidget(std::shared_ptr <QC::CIResults> ciresults, std::shared_ptr<VisualizationTIDCalculate> Calculator, QWidget *parent) : QGroupBox(parent)
{
    this->Calculator = Calculator;
    QVBoxLayout* Layout = new QVBoxLayout;
    StateList = new QListWidget();
    Layout->addWidget(new QLabel("Select Final State"));
    Eigen::VectorXd Energys = (*(ciresults.get())).ci_values;
    for(int i = 1; i < Energys.size(); i++)
    {
        std::string ItemString = "CI-State " + std::to_string(i) + " (Energy: " + std::to_string(Energys[i]) + ")";
        StateList->addItem(QString::fromStdString(ItemString));
    }
    StateList->setCurrentRow(0);
    Layout->addWidget(StateList);
    connect(StateList, &QListWidget::currentRowChanged, this, &VisualizationTIDPlotNTOWidget::SelectedStateChanged);

    Calculate1PTDM = new QPushButton("Calculate 1PTDM");
    Layout->addWidget(Calculate1PTDM);
    connect(Calculate1PTDM, &QPushButton::clicked, this, &VisualizationTIDPlotNTOWidget::Update1PTDM);

    SelectNTOLabel = new QLabel("Select NTO for final state: ");
    Layout->addWidget(SelectNTOLabel);
    NTOList = new QListWidget();
    NTOList->setCurrentRow(0);
    connect(NTOList, &QListWidget::currentRowChanged, this, &VisualizationTIDPlotNTOWidget::SelectedNTOChanged);
    Layout->addWidget(NTOList);
    setLayout(Layout);
}

void VisualizationTIDPlotNTOWidget::SelectedStateChanged()
{
    selected_State_Index = StateList->currentRow();
}

void VisualizationTIDPlotNTOWidget::Update1PTDM()
{
    Calculator->Calculate1PTDM(selected_State_Index+1);
    std::string NTOLabelString = "Select NTO for final state: " + std::to_string(selected_State_Index+1);
    SelectNTOLabel->setText(QString::fromStdString(NTOLabelString));
    NTOList->clear();
    for(int i = 0; i < Calculator->NTO_OccupiedOrbitals; i++)
    {
        std::string ItemString = "Hole-NTO " + std::to_string(i) + " (Occupation: " + std::to_string(Calculator->NTOVector[i]) + ")";
        NTOList->addItem(QString::fromStdString(ItemString));
    }
    for(int i = 0; i < Calculator->NTO_OccupiedOrbitals; i++)
    {
        std::string ItemString = "Particle-NTO " + std::to_string(i) + " (Occupation: " + std::to_string(Calculator->NTOVector[Calculator->NTO_OccupiedOrbitals + i]) + ")";
        NTOList->addItem(QString::fromStdString(ItemString));
    }
}

void VisualizationTIDPlotNTOWidget::SelectedNTOChanged()
{
    selected_NTO = NTOList->currentRow();
}

//************************* Natural Transition Orbital Density *************************
VisualizationTIDPlotNTODensityWidget::VisualizationTIDPlotNTODensityWidget(std::shared_ptr <QC::CIResults> ciresults, QWidget *parent) : QGroupBox(parent)
{
    QVBoxLayout* Layout = new QVBoxLayout;
    StateList = new QListWidget();
    Layout->addWidget(new QLabel("Select Final State"));
    Eigen::VectorXd Energys = (*(ciresults.get())).ci_values;
    for(int i = 1; i < Energys.size(); i++)
    {
        std::string ItemString = "CI-State " + std::to_string(i) + " (Energy: " + std::to_string(Energys[i]) + ")";
        StateList->addItem(QString::fromStdString(ItemString));
    }
    StateList->setCurrentRow(0);
    connect(StateList, &QListWidget::currentRowChanged, this, &VisualizationTIDPlotNTODensityWidget::ParameterChanged);
    Layout->addWidget(StateList);

    NTODensityList = new QListWidget();
    Layout->addWidget(new QLabel("Select NTO Density"));
    NTODensityList->addItem("NTO Hole Density");
    NTODensityList->addItem("NTO Particle Density");
    NTODensityList->setCurrentRow(0);
    connect(NTODensityList, &QListWidget::currentRowChanged, this, &VisualizationTIDPlotNTODensityWidget::ParameterChanged);
    Layout->addWidget(NTODensityList);
    setLayout(Layout);
}

void VisualizationTIDPlotNTODensityWidget::ParameterChanged()
{
    selected_State_Index = StateList->currentRow();
    selected_NTO_Density = NTODensityList->currentRow();
}