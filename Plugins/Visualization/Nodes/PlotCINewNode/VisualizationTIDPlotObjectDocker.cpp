#include "VisualizationTIDPlotObjectDocker.h"

VisualizationTIDPlotObjectDocker::VisualizationTIDPlotObjectDocker(
    std::shared_ptr <VisualizationTIDCalculate> Calculator,
    QWidget *parent
) : QDockWidget(parent)
{
    this->Calculator = Calculator;
    DockerWidget = new QWidget();
    QVBoxLayout* Layout = new QVBoxLayout;
    Layout->setAlignment(Qt::AlignTop);
	Layout->addWidget(new QLabel("Select Plot Object"));

    SelectPlotObject = new QComboBox();
    Layout->addWidget(SelectPlotObject);


    if(Calculator->basissetdata != nullptr)
    {
        SelectPlotObject->addItem(tr("Basisfunction"));
        PlotBasisfunctionWidget = new VisualizationTIDPlotBasisfunctionWidget(Calculator->basissetdata);
        Layout->addWidget(PlotBasisfunctionWidget);

        CanPlotBasisfunction = true;
    }
    if(Calculator->basissetdata != nullptr && Calculator->hfcmatrix != nullptr)
    {
        int MONumber = Calculator->basissetdata->get_all_Basisfunction_with_angular_expanded().size();

        SelectPlotObject->addItem(tr("Molecule Orbitals"));
        PlotMoleculeOrbitalsWidget = new VisualizationTIDPlotMoleculeOrbitalsWidget(MONumber);
        Layout->addWidget(PlotMoleculeOrbitalsWidget);

        SelectPlotObject->addItem(tr("Molecule Orbital Density"));
        PlotMoleculeOrbitalDensityWidget = new VisualizationTIDPlotMoleculeOrbitalDensityWidget(MONumber);
        Layout->addWidget(PlotMoleculeOrbitalDensityWidget);

        CanPlotMOs = true;
    }
    if(Calculator->basissetdata != nullptr && Calculator->hfcmatrix != nullptr && Calculator->ciresults != nullptr)
    {
        SelectPlotObject->addItem(tr("CI-State Density"));
        PlotCIStateDensityWidget = new VisualizationTIDPlotCIStateDensityWidget(Calculator->ciresults);
        Layout->addWidget(PlotCIStateDensityWidget);

        SelectPlotObject->addItem(tr("CI-State Density Difference"));
        PlotCIStateDensityDifferenceWidget = new VisualizationTIDPlotCIStateDensityDifferenceWidget(Calculator->ciresults);
        Layout->addWidget(PlotCIStateDensityDifferenceWidget);

        CanPlotCIDensities = true;
    }
    if(Calculator->basissetdata != nullptr && Calculator->hfcmatrix != nullptr && Calculator->ciresults != nullptr)
    {
        SelectPlotObject->addItem(tr("Natural Transition Orbitals"));
        PlotNTOWidget = new VisualizationTIDPlotNTOWidget(Calculator->ciresults, Calculator);
        Layout->addWidget(PlotNTOWidget);

        SelectPlotObject->addItem(tr("Natural Transition Orbital Density"));
        PlotNTODensityWidget = new VisualizationTIDPlotNTODensityWidget(Calculator->ciresults);
        Layout->addWidget(PlotNTODensityWidget);
        
        CanPlotNTOs = true;
    }


    connect(SelectPlotObject, &QComboBox::currentTextChanged, this, &VisualizationTIDPlotObjectDocker::PlotObjectChanged);
    PlotObjectChanged();

    PlotButton = new QPushButton("Plot");
    Layout->addWidget(PlotButton);
    connect(PlotButton, &QPushButton::clicked, this, &VisualizationTIDPlotObjectDocker::plot);

    DockerWidget->setLayout(Layout);
    this->setWidget(DockerWidget);
}

VisualizationTIDPlotObjectDocker::~VisualizationTIDPlotObjectDocker()
{

}

void VisualizationTIDPlotObjectDocker::PlotObjectChanged()
{
    //Disable all
    if(CanPlotBasisfunction)
    {
        PlotBasisfunctionWidget->setVisible(false); 
    }
    if(CanPlotMOs)
    {
        PlotMoleculeOrbitalsWidget->setVisible(false);
        PlotMoleculeOrbitalDensityWidget->setVisible(false);
    }
    if(CanPlotCIDensities)
    {
        PlotCIStateDensityWidget->setVisible(false);
        PlotCIStateDensityDifferenceWidget->setVisible(false);
    }
    if(CanPlotNTOs)
    {
        PlotNTOWidget->setVisible(false);
        PlotNTODensityWidget->setVisible(false);
    }


    //Enable Selected
    if(SelectPlotObject->currentText() == "Basisfunction"){PlotBasisfunctionWidget->setVisible(true);}
    else if(SelectPlotObject->currentText() == "Molecule Orbitals"){PlotMoleculeOrbitalsWidget->setVisible(true);}
    else if(SelectPlotObject->currentText() == "Molecule Orbital Density"){PlotMoleculeOrbitalDensityWidget->setVisible(true);}
    else if(SelectPlotObject->currentText() == "CI-State Density"){PlotCIStateDensityWidget->setVisible(true);}
    else if(SelectPlotObject->currentText() == "CI-State Density Difference"){PlotCIStateDensityDifferenceWidget->setVisible(true);}
    else if(SelectPlotObject->currentText() == "Natural Transition Orbitals"){PlotNTOWidget->setVisible(true);}
    else if(SelectPlotObject->currentText() == "Natural Transition Orbital Density"){PlotNTODensityWidget->setVisible(true);}
}

void VisualizationTIDPlotObjectDocker::plot()
{
    if(SelectPlotObject->count() > 0)
    {
        if(SelectPlotObject->currentText() == "Basisfunction"){Calculator->PlotBasisfunction(PlotBasisfunctionWidget->selected_Basisfunction);}
        else if(SelectPlotObject->currentText() == "Molecule Orbitals"){Calculator->PlotMoleculeOrbital(PlotMoleculeOrbitalsWidget->selected_MO);}
        else if(SelectPlotObject->currentText() == "Molecule Orbital Density"){Calculator->PlotMoleculeOrbitalDensity(PlotMoleculeOrbitalDensityWidget->selected_MO);}
        else if(SelectPlotObject->currentText() == "CI-State Density"){Calculator->PlotCIStateDensity(PlotCIStateDensityWidget->selected_State);}
        else if(SelectPlotObject->currentText() == "CI-State Density Difference"){Calculator->PlotCIStateDensityDifference(PlotCIStateDensityDifferenceWidget->selected_State1, PlotCIStateDensityDifferenceWidget->selected_State2);}
        else if(SelectPlotObject->currentText() == "Natural Transition Orbitals"){Calculator->PlotNTO(PlotNTOWidget->selected_State_Index + 1, PlotNTOWidget->selected_NTO);}
        else if(SelectPlotObject->currentText() == "Natural Transition Orbital Density"){Calculator->PlotNTODensity(PlotNTODensityWidget->selected_State_Index + 1, PlotNTODensityWidget->selected_NTO_Density);}
    }
}