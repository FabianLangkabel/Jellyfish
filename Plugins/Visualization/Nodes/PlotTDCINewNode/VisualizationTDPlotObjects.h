#pragma once

#include <QtWidgets>
#include <iostream>

#include "VisualizationTDCalculate.h"
#include "../../Lib/header/gaussian_basisset.h"
#include "../../Lib/header/pointcharge.h"
#include "../../Lib/header/CIResults.h"


//************************* Density *************************
class VisualizationTDPlotDensityWidget : public QWidget
{
public:
    explicit VisualizationTDPlotDensityWidget(QWidget *parent = 0){};
    ~VisualizationTDPlotDensityWidget(){};
};

//************************* DensityDifference *************************
class VisualizationTDPlotDensityDifferenceWidget : public QWidget
{
public:
    explicit VisualizationTDPlotDensityDifferenceWidget(std::shared_ptr <QC::CIResults> ciresults, QWidget *parent = 0);
    ~VisualizationTDPlotDensityDifferenceWidget(){};

private:
    QListWidget* ReferenceCIStateList;
    void ParameterChanged();

public:
    int ReferenceCIState = 0;
};

//************************* DensityGradient *************************
class VisualizationTDPlotDensityGradientWidget : public QWidget
{
public:
    explicit VisualizationTDPlotDensityGradientWidget(QWidget *parent = 0){};
    ~VisualizationTDPlotDensityGradientWidget(){};
};

//************************* NTODensity *************************
class VisualizationTDPlotNTODensityWidget : public QWidget
{
public:
    explicit VisualizationTDPlotNTODensityWidget(QWidget *parent = 0);
    ~VisualizationTDPlotNTODensityWidget(){};

private:
    QComboBox* DensitySelector;
    void ParameterChanged();

public:
    int SelectedDensity = 0;

};