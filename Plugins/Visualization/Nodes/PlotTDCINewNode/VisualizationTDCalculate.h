#pragma once

#include <QtWidgets>
#include "../../Lib/header/gaussian_basisset.h"
#include "../../Lib/header/pointcharge.h"
#include "../../Lib/header/tdci_wf.h"
#include "../../Lib/header/CIResults.h"
#include "../../Lib/gui/SDInteraction.h"
#include "../../Lib/gui/ValueCuboid.h"


class Plot_TDCI_New_Window;
class VisualizationTDCalculate
{
public:
    VisualizationTDCalculate(
        std::shared_ptr <QC::CIResults> ciresults,
        std::shared_ptr <Eigen::MatrixXd> hfcmatrix,
        std::shared_ptr <QC::Gaussian_Basisset> basissetdata,
        std::shared_ptr <std::vector<QC::Pointcharge>> pointcharges,
        std::shared_ptr <QC::TDCI_WF> TDCICoefficients,
        Plot_TDCI_New_Window* PlotMainWindow
    );
    ~VisualizationTDCalculate();

public:
    std::shared_ptr <QC::CIResults> ciresults;
    std::shared_ptr <Eigen::MatrixXd> hfcmatrix;
    std::shared_ptr <QC::Gaussian_Basisset> basissetdata;
    std::shared_ptr <std::vector<QC::Pointcharge>> pointcharges;
    std::shared_ptr <QC::TDCI_WF> TDCICoefficients;

private:
    Plot_TDCI_New_Window* PlotMainWindow;
    int xdim; 
    int ydim;
    int zdim;
    double* XGrid = nullptr;
    double* YGrid = nullptr;
    double* ZGrid = nullptr;

    double* PictureData = nullptr;

private:
    void PrecomputeAllMOsOnGrid();
    double* MOValuesXYZ = nullptr; //All Values in Space of all HF-MOs

    bool AOCuboidForGridMustBeUpdated = true;
    void CalculateAllAOCuboidForGrid();
    std::vector<std::shared_ptr<AOValueCuboid>> AOValueCuboids;
    bool AOAOCuboidForGridMustBeUpdated = true;
    void CalculateAllAOAOCuboidForGrid();
    std::vector<std::shared_ptr<AOAOValueCuboid>> AOAOValueCuboids;

    std::vector<SDInteraction> SDInteractions;
    void CalculateAllSDInteractions();
    bool SDInteractionsCalculated = false;

    //std::vector<Eigen::MatrixXd> TDMOPrefactors;
    //void CalculateTDMOPrefactors();
    //bool TDMOPrefactorsCalculated = false;

public:
    void UpdateGrid();
    void PlotDensity(int Timestep);
    void PlotDensityDifference(int Timestep, int Referencestate);
    void PlotDensityGradient(int Timestep);
    void PlotNTODensity(int Timestep, int DensityType);
};
