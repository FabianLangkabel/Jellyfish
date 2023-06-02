#pragma once

#include <chrono>

#include <QtWidgets>
#include "../../Lib/header/gaussian_basisset.h"
#include "../../Lib/header/pointcharge.h"
#include "../../Lib/header/CIResults.h"
#include "../../Lib/gui/SDInteraction.h"
#include "../../Lib/gui/ValueCuboid.h"

class Plot_CI_New_Window;
class VisualizationTIDCalculate
{
public:
    VisualizationTIDCalculate(
        std::shared_ptr <QC::CIResults> ciresults,
        std::shared_ptr <Eigen::MatrixXd> hfcmatrix,
        std::shared_ptr <QC::Gaussian_Basisset> basissetdata,
        std::shared_ptr <std::vector<QC::Pointcharge>> pointcharges,
        Plot_CI_New_Window* PlotMainWindow
    );
    ~VisualizationTIDCalculate();

public:
    std::shared_ptr <QC::CIResults> ciresults;
    std::shared_ptr <Eigen::MatrixXd> hfcmatrix;
    std::shared_ptr <QC::Gaussian_Basisset> basissetdata;
    std::shared_ptr <std::vector<QC::Pointcharge>> pointcharges;

private:
    Plot_CI_New_Window* PlotMainWindow;
    int xdim; 
    int ydim;
    int zdim;
    double* XGrid = nullptr;
    double* YGrid = nullptr;
    double* ZGrid = nullptr;

    double* PictureData = nullptr;

private:
    void PrecomputeAllAOsOnGrid();
    double* AOValuesXYZ = nullptr; //All Values in Space of all AOs
    bool AOCuboidForGridMustBeUpdated = true;
    void CalculateAllAOCuboidForGrid();
    std::vector<std::shared_ptr<AOValueCuboid>> AOValueCuboids;
    bool AOAOCuboidForGridMustBeUpdated = true;
    void CalculateAllAOAOCuboidForGrid();
    std::vector<std::shared_ptr<AOAOValueCuboid>> AOAOValueCuboids;

    void PrecomputeAllMOsOnGrid();
    double* MOValuesXYZ = nullptr; //All Values in Space of all HF-MOs
    std::vector<SDInteraction> SDInteractions;
    void CalculateAllSDInteractions();
    bool SDInteractionsCalculated = false;
    Eigen::MatrixXd NTO_matrix;
    void PrecomputeAllNTOsOnGrid();
    double* NTOValuesXYZ = nullptr; //All Values in Space of all NTOs

public:
    void Calculate1PTDM(int FinalState);
    int NTO_FinalState = -1;
    int NTO_OccupiedOrbitals;
    Eigen::VectorXd NTOVector;

public:
    void UpdateGrid();
    void PlotBasisfunction(int BasisfunctionNumber);
    void PlotMoleculeOrbital(int MoleculeOrbitalNumber);
    void PlotMoleculeOrbitalDensity(int MoleculeOrbitalNumber);
    void PlotCIStateDensity(int CIStateDensityNumber);
    void PlotCIStateDensityDifference(int CIStateNumber1, int CIStateNumber2);
    void PlotNTO(int FinalState, int NTONumber);
    void PlotNTODensity(int FinalState, int NTODensityNumber);
};
