#pragma once

#include <QtWidgets>
#include "VisualizationTIDCalculate.h"
#include "VisualizationTIDPlotObjects.h"
#include "../../Lib/header/gaussian_basisset.h"
#include "../../Lib/header/pointcharge.h"

class VisualizationTIDPlotObjectDocker : public QDockWidget
{
public:
    explicit VisualizationTIDPlotObjectDocker(	
        std::shared_ptr <VisualizationTIDCalculate> Calculator,
        QWidget *parent = 0);
    ~VisualizationTIDPlotObjectDocker();

private:
    std::shared_ptr <VisualizationTIDCalculate> Calculator;

private:
    QWidget* DockerWidget;
    QComboBox* SelectPlotObject;
    QPushButton* PlotButton;
    VisualizationTIDPlotBasisfunctionWidget* PlotBasisfunctionWidget;
    VisualizationTIDPlotMoleculeOrbitalsWidget* PlotMoleculeOrbitalsWidget;
    VisualizationTIDPlotMoleculeOrbitalDensityWidget* PlotMoleculeOrbitalDensityWidget;
    VisualizationTIDPlotCIStateDensityWidget* PlotCIStateDensityWidget;
    VisualizationTIDPlotCIStateDensityDifferenceWidget* PlotCIStateDensityDifferenceWidget;
    VisualizationTIDPlotNTOWidget* PlotNTOWidget;
    VisualizationTIDPlotNTODensityWidget* PlotNTODensityWidget;

    bool CanPlotBasisfunction = false;
    bool CanPlotMOs = false;
    bool CanPlotCIDensities = false;
    bool CanPlotNTOs = false;

    void PlotObjectChanged();
    void plot();
};
