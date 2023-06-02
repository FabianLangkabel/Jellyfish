#pragma once

#include <QtWidgets>
#include <iostream>

#include "VisualizationTIDCalculate.h"
#include "../../Lib/header/gaussian_basisset.h"
#include "../../Lib/header/pointcharge.h"

//************************* Basisfunction *************************
class VisualizationTIDPlotBasisfunctionWidget : public QGroupBox
{
public:
    explicit VisualizationTIDPlotBasisfunctionWidget(std::shared_ptr <QC::Gaussian_Basisset> basisset, QWidget *parent = 0);
    ~VisualizationTIDPlotBasisfunctionWidget(){};

private:
    QListWidget* BasisfunctionList;
    void ParameterChanged();

public:
    int selected_Basisfunction = 0;
};

//************************* Molecule Orbitals *************************
class VisualizationTIDPlotMoleculeOrbitalsWidget : public QGroupBox
{
public:
    explicit VisualizationTIDPlotMoleculeOrbitalsWidget(int MONumbers, QWidget *parent = 0);
    ~VisualizationTIDPlotMoleculeOrbitalsWidget(){};

private:
    QListWidget* MOList;
    void ParameterChanged();

public:
    int selected_MO = 0;
};

//************************* Molecule Orbital Density *************************
class VisualizationTIDPlotMoleculeOrbitalDensityWidget : public QGroupBox
{
public:
    explicit VisualizationTIDPlotMoleculeOrbitalDensityWidget(int MONumbers, QWidget *parent = 0);
    ~VisualizationTIDPlotMoleculeOrbitalDensityWidget(){};

private:
    QListWidget* MOList;
    void ParameterChanged();

public:
    int selected_MO = 0;
};

//************************* CI-State Density *************************
class VisualizationTIDPlotCIStateDensityWidget : public QGroupBox
{
public:
    explicit VisualizationTIDPlotCIStateDensityWidget(std::shared_ptr <QC::CIResults> ciresults, QWidget *parent = 0);
    ~VisualizationTIDPlotCIStateDensityWidget(){};

private:
    QListWidget* CIStateList;
    void ParameterChanged();

public:
    int selected_State = 0;
};

//************************* CI-State Density Difference *************************
class VisualizationTIDPlotCIStateDensityDifferenceWidget : public QGroupBox
{
public:
    explicit VisualizationTIDPlotCIStateDensityDifferenceWidget(std::shared_ptr <QC::CIResults> ciresults, QWidget *parent = 0);
    ~VisualizationTIDPlotCIStateDensityDifferenceWidget(){};

private:
    QListWidget* CIStateList1;
    QListWidget* CIStateList2;
    void ParameterChanged();

public:
    int selected_State1 = 0;
    int selected_State2 = 0;
};

//************************* Natural Transition Orbitals *************************
class VisualizationTIDPlotNTOWidget : public QGroupBox
{
public:
    explicit VisualizationTIDPlotNTOWidget(std::shared_ptr <QC::CIResults> ciresults, std::shared_ptr<VisualizationTIDCalculate> Calculator, QWidget *parent = 0);
    ~VisualizationTIDPlotNTOWidget(){};

private:
    std::shared_ptr<VisualizationTIDCalculate> Calculator;
    QListWidget* StateList;
    QPushButton* Calculate1PTDM;
    QListWidget* NTOList;
    QLabel* SelectNTOLabel;
    void SelectedStateChanged();
    void Update1PTDM();
    void SelectedNTOChanged();

public:
    int selected_State_Index = 0;
    int selected_NTO = -1;
};

//************************* Natural Transition Orbital Density *************************
class VisualizationTIDPlotNTODensityWidget : public QGroupBox
{
public:
    explicit VisualizationTIDPlotNTODensityWidget(std::shared_ptr <QC::CIResults> ciresults, QWidget *parent = 0);
    ~VisualizationTIDPlotNTODensityWidget(){};

private:
    QListWidget* StateList;
    QListWidget* NTODensityList;
    void ParameterChanged();

public:
    int selected_State_Index = 0;
    int selected_NTO_Density = 0;
};
