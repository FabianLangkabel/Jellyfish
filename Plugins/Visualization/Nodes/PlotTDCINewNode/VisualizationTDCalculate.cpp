#include "VisualizationTDCalculate.h"
#include "plot_tdci_new_window.h"

VisualizationTDCalculate::VisualizationTDCalculate(
	std::shared_ptr <QC::CIResults> ciresults,
	std::shared_ptr <Eigen::MatrixXd> hfcmatrix,
	std::shared_ptr <QC::Gaussian_Basisset> basissetdata,
	std::shared_ptr <std::vector<QC::Pointcharge>> pointcharges,
    std::shared_ptr <QC::TDCI_WF> TDCICoefficients,
    Plot_TDCI_New_Window* PlotMainWindow
	)
{
    this->ciresults = ciresults;
    this->hfcmatrix = hfcmatrix;
    this->basissetdata = basissetdata;
    this->pointcharges = pointcharges;
    this->TDCICoefficients = TDCICoefficients;
    this->PlotMainWindow = PlotMainWindow;
}

VisualizationTDCalculate::~VisualizationTDCalculate()
{
    if(XGrid != nullptr){ delete[] XGrid; }
    if(YGrid != nullptr){ delete[] YGrid; }
    if(ZGrid != nullptr){ delete[] ZGrid; }

    if(MOValuesXYZ != nullptr){ delete[] MOValuesXYZ; }

    if(PictureData != nullptr){ delete[] PictureData; }
}

void VisualizationTDCalculate::UpdateGrid()
{
    if(XGrid == nullptr || PlotMainWindow->GridWidget->UpdateGridIfNeeded())
    {
        xdim = PlotMainWindow->GridWidget->UsedGrid->xdim;
        ydim = PlotMainWindow->GridWidget->UsedGrid->ydim;
        zdim = PlotMainWindow->GridWidget->UsedGrid->zdim;
        XGrid = PlotMainWindow->GridWidget->UsedGrid->XGrid;
        YGrid = PlotMainWindow->GridWidget->UsedGrid->YGrid;
        ZGrid = PlotMainWindow->GridWidget->UsedGrid->ZGrid;

        PictureData = new double[xdim*ydim*zdim];

        if(MOValuesXYZ != nullptr){ delete[] MOValuesXYZ; MOValuesXYZ = nullptr;}

        AOCuboidForGridMustBeUpdated = true;
        AOAOCuboidForGridMustBeUpdated = true;

        PlotMainWindow->LogWidget->AddToLog(0, "Grid updated");
    }
}

//****************************** Helper Functions ******************************
void VisualizationTDCalculate::PrecomputeAllMOsOnGrid()
{
    int NumberOfMOs = basissetdata->get_Basisfunctionnumber_angular_expanded();
    MOValuesXYZ = new double[NumberOfMOs*xdim*ydim*zdim];
    std::vector<std::tuple<QC::Gaussian_Basisfunction, int>> basisfunctionsandangularmoment = basissetdata->get_all_Basisfunction_with_angular_expanded();

    Eigen::MatrixXd basisvalues = Eigen::MatrixXd(NumberOfMOs, 1);
	for (int z = 0; z < zdim; z ++) {
		for (int y = 0; y < ydim; y++) {
			for (int x = 0; x < xdim; x++) {
				// for all basisfunctions
				for (int j = 0; j < NumberOfMOs; j++)
				{
					//get basisfunction
					QC::Gaussian_Basisfunction basisfunction;
					int angular_moment;
					std::tie(basisfunction, angular_moment) = basisfunctionsandangularmoment[j];
					//get value at point in space
					double localvalue = basisfunction.get_basisfunction_value(basissetdata->get_basisset_type(), XGrid[x] - basisfunction.get_center_x(), YGrid[y] - basisfunction.get_center_y(), ZGrid[z] - basisfunction.get_center_z(), angular_moment);
					basisvalues(j) = localvalue;
				}
				// 
				Eigen::MatrixXd movalues = basisvalues.transpose() * *(hfcmatrix);
				// First Index of Position in space of MO_0
				int i = NumberOfMOs * (x + xdim * (y + ydim * z));
				// for each MO_n - set value in space
				for (int n = 0; n < NumberOfMOs; n++)
				{
					double value = movalues(n);
					MOValuesXYZ[i+n] = value;
				}
			}
		}
	}
    PlotMainWindow->LogWidget->AddToLog(0, "MOs on Grid updated");
}

void VisualizationTDCalculate::CalculateAllSDInteractions()
{   
    //Create List of all Slater-Determinant Interactions != 0
    SDInteractions.clear();
    std::vector<std::string> cistrings = (*ciresults).configuration_strings;
    for(int i = 0; i < cistrings.size(); i++)
    {
        for(int j = 0; j < cistrings.size(); j++)
        {   
            //Apply Slater-Condon Rules
            std::string cistring1 = cistrings[i];
            std::string cistring2 = cistrings[j];

            //Calculate Excitation
            int exc = 0;
            for (int k = 0; k < cistring1.size(); k++)
            {
                if (cistring1[k] != cistring2[k]) { exc++; }
            }
            exc = exc / 2;

            //Same SlaterDeterminant
            if(exc == 0)
            {
                std::vector<int> occupied_spinorbitals;
                for (int k = 0; k < cistring1.size(); k++)
                {
                    if(cistring1[k] == '1') { occupied_spinorbitals.push_back(k); }
                }
                SDInteractions.push_back(SDInteraction(i,i, occupied_spinorbitals, true));
            }
            //One SpinOrbital is different
            else if(exc == 1)
            {
                //Find Hole and Particle
                int hole, particle;
                for (int k = 0; k < cistring1.size(); k++)
                {
                    if (cistring1[k] == '1' && cistring2[k] == '0') { hole = k; }
                    else if (cistring1[k] == '0' && cistring2[k] == '1') { particle = k; }
                }
                //Calculate Phase
                bool PositivSign = true;
                int start, end;
                if (hole < particle) { start = hole; end = particle; }
                else { start = particle; end = hole; }
                for (int k = start + 1; k < end; k++)
                {
                    if (cistring1[k] == '1') { PositivSign = !PositivSign; }
                }

                //Add to SDInteractions
                SDInteractions.push_back(SDInteraction(i,j, std::vector<int>{hole, particle}, PositivSign));
            }

        }
    }
    /*
    for(int i = 0; i < SDInteractions.size(); i++)
    {
        std::vector<std::string> cistrings = *ciconfigurationstrings;
        SDInteraction AC = SDInteractions[i];
        std::cout << cistrings[AC.SD1] << " " << cistrings[AC.SD2] << " " << AC.Orbitals[0] << " " << AC.Orbitals[1] << " " << AC.PositivSign << std::endl;
    }
    */
    SDInteractionsCalculated = true;
    PlotMainWindow->LogWidget->AddToLog(0, "All Slaterdeterminant-Interactions calculated");
}

/*
void VisualizationTDCalculate::CalculateTDMOPrefactors()
{
    TDMOPrefactors.clear();
    int NumberOfMOs = basissetdata->get_Basisfunctionnumber_angular_expanded();
    Eigen::MatrixXd CIMatrix = *(cimatrix);
    std::vector<Eigen::VectorXcd> B_Vectors = TDCICoefficients->get_ci_vectors_for_state(0);

    //for(int timestep = 0; timestep < TDCICoefficients->get_times().size(); timestep++)
    for(int timestep = 1000; timestep < 1001; timestep++)
    {
        std::cout << "Calculate Prefactors for Timestep: " << timestep << "/" << TDCICoefficients->get_times().size() - 1 << std::endl;
        Eigen::MatrixXd MOPrefactors = Eigen::MatrixXd::Zero(NumberOfMOs, NumberOfMOs);
        double cij_threshold = 0;

        //Step 1a: Calculate Bi * Bj Matrix and Transform to CiCj Matrix (OLD !!!)
        //Eigen::VectorXcd B_vec = B_Vectors[timestep];
        //Eigen::MatrixXcd CiCj_Matrix = Eigen::MatrixXd::Zero(B_vec.rows(), B_vec.rows());
        //for(int Bi = 0; Bi < B_vec.rows(); Bi++)
        //{
        //    for(int Bj = 0; Bj < B_vec.rows(); Bj++)
        //    {
        //        std::complex<double> BiBj = B_vec[Bi] * std::conj(B_vec[Bj]);
        //        Eigen::VectorXd CIi_vec = CIMatrix.col(Bi);
        //        Eigen::VectorXd CIj_vec = CIMatrix.col(Bj);
        //        for(int Ci = 0; Ci < CIi_vec.rows(); Ci++)
        //        {
        //            for(int Cj = 0; Cj < CIj_vec.rows(); Cj++)
        //            {
        //                double CiCj = CIi_vec[Ci] * CIj_vec[Cj];
        //                CiCj_Matrix(Ci, Cj) += BiBj * CiCj;
        //            }
        //        }
        //    }
        //}

        //Step 1b: (alternative): Transform B-Vec in SD-Space and calculate CiCj Matrix
        Eigen::VectorXcd B_vec = B_Vectors[timestep];
        Eigen::MatrixXcd CiCj_Matrix = Eigen::MatrixXd::Zero(B_vec.rows(), B_vec.rows());
        Eigen::VectorXcd TD_VEC_transformed = CIMatrix * B_vec;
        for(int Ci = 0; Ci < TD_VEC_transformed.rows(); Ci++)
        {
            for(int Cj = 0; Cj < TD_VEC_transformed.rows(); Cj++)
            {
                std::complex<double> CiCj = TD_VEC_transformed[Ci] * std::conj(TD_VEC_transformed[Cj]);
                CiCj_Matrix(Ci, Cj) = CiCj;
            }
        }
        std::cout << "Mit Transformation: " << std::endl;
        for(int i = 0; i < CiCj_Matrix.rows(); i++)
        {
            for(int j = 0; j < CiCj_Matrix.rows(); j++)
            {
                if(abs(CiCj_Matrix(i, j)) > 0.01)
                {
                    std::cout << i << " " << j << " : " << CiCj_Matrix(i, j) << std::endl;
                }
            }
        }
        //Step 2: Calculates Matrix of MO*MO Entries (as in TID)


        //Step 3: AddMOPrefactors to Vector
        TDMOPrefactors.push_back(MOPrefactors);
    }
}
*/

void VisualizationTDCalculate::CalculateAllAOCuboidForGrid()
{
    AOValueCuboids.clear();

    int NumberOfAOs = basissetdata->get_Basisfunctionnumber_angular_expanded();
    std::vector<std::tuple<QC::Gaussian_Basisfunction, int>> basisfunctionsandangularmoment = basissetdata->get_all_Basisfunction_with_angular_expanded();
    for(int i = 0; i < NumberOfAOs; i++)
    {
        QC::Gaussian_Basisfunction basisfunction;
        int angular_moment;
        std::tie(basisfunction, angular_moment) = basisfunctionsandangularmoment[i];
        std::shared_ptr<AOValueCuboid> Cuboid = std::make_shared<AOValueCuboid>(i, basisfunction, basissetdata->get_basisset_type(), angular_moment, PlotMainWindow->GridWidget->UsedGrid);
        AOValueCuboids.push_back(Cuboid);
    }

    AOCuboidForGridMustBeUpdated = false;
    PlotMainWindow->LogWidget->AddToLog(0, "AOs on Grid updated");
}

void VisualizationTDCalculate::CalculateAllAOAOCuboidForGrid()
{
    AOAOValueCuboids.clear();

    int NumberOfAOs = basissetdata->get_Basisfunctionnumber_angular_expanded();
    std::vector<std::tuple<QC::Gaussian_Basisfunction, int>> basisfunctionsandangularmoment = basissetdata->get_all_Basisfunction_with_angular_expanded();
    for(int i = 0; i < NumberOfAOs; i++)
    {
        QC::Gaussian_Basisfunction basisfunction;
        int angular_moment;
        std::tie(basisfunction, angular_moment) = basisfunctionsandangularmoment[i];
        std::shared_ptr<AOAOValueCuboid> Cuboid = std::make_shared<AOAOValueCuboid>(i, basisfunction, angular_moment, i, basisfunction, angular_moment, basissetdata->get_basisset_type(), PlotMainWindow->GridWidget->UsedGrid);
        AOAOValueCuboids.push_back(Cuboid);
    }

    for(int i = 0; i < NumberOfAOs; i++)
    {
        for(int j = 0; j < i; j++)
        {
            QC::Gaussian_Basisfunction basisfunction1;
            int angular_moment1;
            std::tie(basisfunction1, angular_moment1) = basisfunctionsandangularmoment[i];
            QC::Gaussian_Basisfunction basisfunction2;
            int angular_moment2;
            std::tie(basisfunction2, angular_moment2) = basisfunctionsandangularmoment[j];

            std::shared_ptr<AOAOValueCuboid> Cuboid = std::make_shared<AOAOValueCuboid>(i, basisfunction1, angular_moment1, j, basisfunction2, angular_moment2, basissetdata->get_basisset_type(), PlotMainWindow->GridWidget->UsedGrid);
            if(Cuboid->AOsHaveOverlap)
            {
                AOAOValueCuboids.push_back(Cuboid);
            }
        }
    }

    AOAOCuboidForGridMustBeUpdated = false;
    PlotMainWindow->LogWidget->AddToLog(0, "AOs*AOs on Grid updated");
}

//****************************** Plot Functions ******************************
void VisualizationTDCalculate::PlotDensity(int Timestep)
{
    UpdateGrid();
    if(AOAOCuboidForGridMustBeUpdated){CalculateAllAOAOCuboidForGrid();} 
    if(!SDInteractionsCalculated){CalculateAllSDInteractions();}

    int NumberOfMOs = basissetdata->get_Basisfunctionnumber_angular_expanded();
    Eigen::MatrixXd CIMatrix = (*ciresults).ci_matrix;
    Eigen::VectorXcd B_vec = TDCICoefficients->get_ci_vectors_for_state(0)[Timestep];

    //1. Transform to State in SD-Basis
    Eigen::VectorXcd TD_VEC_transformed = CIMatrix * B_vec;

    //2. Calculate MOPrefactor-Matrix
    Eigen::MatrixXcd MOPrefactors = Eigen::MatrixXd::Zero(NumberOfMOs, NumberOfMOs);
    double cij_threshold = 0;
    for(int i = 0; i < SDInteractions.size(); i++)
    {
        std::complex<double> cij = TD_VEC_transformed(SDInteractions[i].SD1) * std::conj(TD_VEC_transformed(SDInteractions[i].SD2));
        if(abs(cij) > cij_threshold)
        {
            if(SDInteractions[i].SD1 == SDInteractions[i].SD2)
            {   
                for(int j = 0; j < SDInteractions[i].Orbitals.size(); j++)
                {
                    int SpartialOrb = SDInteractions[i].Orbitals[j] / 2;
                    MOPrefactors(SpartialOrb, SpartialOrb) += cij;
                }
            }
            else
            {
                int HoleSpartialOrb = SDInteractions[i].Orbitals[0] / 2;
                int ParticleSpartialOrb = SDInteractions[i].Orbitals[1] / 2;
                if(SDInteractions[i].PositivSign){ MOPrefactors(HoleSpartialOrb, ParticleSpartialOrb) += cij; }
                else{ MOPrefactors(HoleSpartialOrb, ParticleSpartialOrb) -= cij; }
            }
        }
    }

    //std::cout << "MOPrefactor-Matrix" << std::endl;
    //std::cout << MOPrefactors << std::endl;

    //Translate MO*MO Matrix to AO*AO Matrix
    Eigen::MatrixXd HFCMatrix = *hfcmatrix;
    Eigen::MatrixXcd AOPrefactors = HFCMatrix * MOPrefactors * HFCMatrix.transpose();

    //3. Set PictureArray to Zero
    for (int i = 0; i < xdim * ydim * zdim; i ++) {
        PictureData[i] = 0;
    }

    //4. Translate AO*AO Matrix to Density
    double PlotAOAOTresh = 0.00000001;
    for(int i = 0; i < AOAOValueCuboids.size(); i++)
    {
        double AOAOPrefac = AOPrefactors(AOAOValueCuboids[i]->basisfunction_number1, AOAOValueCuboids[i]->basisfunction_number2).real();
        if(abs(AOAOPrefac) > PlotAOAOTresh)
        {
            AOAOValueCuboids[i]->AddAOAOToImageData(PictureData, AOAOPrefac);
        }
    }

    PlotMainWindow->RenderWidget->UpdateScreenWithExternalImageData(PictureData);
}

void VisualizationTDCalculate::PlotDensityDifference(int Timestep, int Referencestate)
{
    UpdateGrid();
    if(AOAOCuboidForGridMustBeUpdated){CalculateAllAOAOCuboidForGrid();} 
    if(!SDInteractionsCalculated){CalculateAllSDInteractions();}

    int NumberOfMOs = basissetdata->get_Basisfunctionnumber_angular_expanded();
    Eigen::MatrixXd CIMatrix = (*ciresults).ci_matrix;
    Eigen::VectorXcd B_vec = TDCICoefficients->get_ci_vectors_for_state(0)[Timestep];

    //1. Transform to State in SD-Basis
    Eigen::VectorXcd TD_VEC_transformed = CIMatrix * B_vec;

    //2. Calculate MOPrefactor-Matrix
    Eigen::MatrixXcd MOPrefactors = Eigen::MatrixXd::Zero(NumberOfMOs, NumberOfMOs);
    double cij_threshold = 0;
    for(int i = 0; i < SDInteractions.size(); i++)
    {
        std::complex<double> cij = TD_VEC_transformed(SDInteractions[i].SD1) * std::conj(TD_VEC_transformed(SDInteractions[i].SD2));
        if(abs(cij) > cij_threshold)
        {
            if(SDInteractions[i].SD1 == SDInteractions[i].SD2)
            {   
                for(int j = 0; j < SDInteractions[i].Orbitals.size(); j++)
                {
                    int SpartialOrb = SDInteractions[i].Orbitals[j] / 2;
                    MOPrefactors(SpartialOrb, SpartialOrb) += cij;
                }
            }
            else
            {
                int HoleSpartialOrb = SDInteractions[i].Orbitals[0] / 2;
                int ParticleSpartialOrb = SDInteractions[i].Orbitals[1] / 2;
                if(SDInteractions[i].PositivSign){ MOPrefactors(HoleSpartialOrb, ParticleSpartialOrb) += cij; }
                else{ MOPrefactors(HoleSpartialOrb, ParticleSpartialOrb) -= cij; }
            }
        }
    }

    //2b. Calculate MOPrefactor-Matrix for Reference
    for(int i = 0; i < SDInteractions.size(); i++)
    {
        double cij = CIMatrix(SDInteractions[i].SD1, Referencestate) * CIMatrix(SDInteractions[i].SD2, Referencestate);
        if(abs(cij) > cij_threshold)
        {
            if(SDInteractions[i].SD1 == SDInteractions[i].SD2)
            {   
                for(int j = 0; j < SDInteractions[i].Orbitals.size(); j++)
                {
                    int SpartialOrb = SDInteractions[i].Orbitals[j] / 2;
                    MOPrefactors(SpartialOrb, SpartialOrb) -= cij;
                }
            }
            else
            {
                int HoleSpartialOrb = SDInteractions[i].Orbitals[0] / 2;
                int ParticleSpartialOrb = SDInteractions[i].Orbitals[1] / 2;
                if(SDInteractions[i].PositivSign){ MOPrefactors(HoleSpartialOrb, ParticleSpartialOrb) -= cij; }
                else{ MOPrefactors(HoleSpartialOrb, ParticleSpartialOrb) += cij; }
            }
        }
    }

    //std::cout << "MOPrefactor-Matrix" << std::endl;
    //std::cout << MOPrefactors << std::endl;

    //Translate MO*MO Matrix to AO*AO Matrix
    Eigen::MatrixXd HFCMatrix = *hfcmatrix;
    Eigen::MatrixXcd AOPrefactors = HFCMatrix * MOPrefactors * HFCMatrix.transpose();

    //3. Set PictureArray to Zero
    for (int i = 0; i < xdim * ydim * zdim; i ++) {
        PictureData[i] = 0;
    }

    //4. Translate MO*MO Matrix to Density
    double PlotAOAOTresh = 0.00000001;
    for(int i = 0; i < AOAOValueCuboids.size(); i++)
    {
        double AOAOPrefac = AOPrefactors(AOAOValueCuboids[i]->basisfunction_number1, AOAOValueCuboids[i]->basisfunction_number2).real();
        if(abs(AOAOPrefac) > PlotAOAOTresh)
        {
            AOAOValueCuboids[i]->AddAOAOToImageData(PictureData, AOAOPrefac);
        }
    }
    PlotMainWindow->RenderWidget->UpdateScreenWithExternalImageData(PictureData);
}

void VisualizationTDCalculate::PlotDensityGradient(int Timestep)
{
    if(Timestep > 0)
    {
        UpdateGrid();
        if(AOAOCuboidForGridMustBeUpdated){CalculateAllAOAOCuboidForGrid();} 
        if(!SDInteractionsCalculated){CalculateAllSDInteractions();}

        int NumberOfMOs = basissetdata->get_Basisfunctionnumber_angular_expanded();
        Eigen::MatrixXd CIMatrix = (*ciresults).ci_matrix;
        Eigen::VectorXcd B_vec = TDCICoefficients->get_ci_vectors_for_state(0)[Timestep];
        Eigen::VectorXcd B_vec_before = TDCICoefficients->get_ci_vectors_for_state(0)[Timestep - 1];

        //1. Transform to State in SD-Basis
        Eigen::VectorXcd TD_VEC_transformed = CIMatrix * B_vec;
        Eigen::VectorXcd TD_VEC_before_transformed = CIMatrix * B_vec_before;

        //2. Calculate MOPrefactor-Matrix
        Eigen::MatrixXcd MOPrefactors = Eigen::MatrixXd::Zero(NumberOfMOs, NumberOfMOs);
        double cij_threshold = 0;
        for(int i = 0; i < SDInteractions.size(); i++)
        {
            std::complex<double> cij = TD_VEC_transformed(SDInteractions[i].SD1) * std::conj(TD_VEC_transformed(SDInteractions[i].SD2));
            if(abs(cij) > cij_threshold)
            {
                if(SDInteractions[i].SD1 == SDInteractions[i].SD2)
                {   
                    for(int j = 0; j < SDInteractions[i].Orbitals.size(); j++)
                    {
                        int SpartialOrb = SDInteractions[i].Orbitals[j] / 2;
                        MOPrefactors(SpartialOrb, SpartialOrb) += cij;
                    }
                }
                else
                {
                    int HoleSpartialOrb = SDInteractions[i].Orbitals[0] / 2;
                    int ParticleSpartialOrb = SDInteractions[i].Orbitals[1] / 2;
                    if(SDInteractions[i].PositivSign){ MOPrefactors(HoleSpartialOrb, ParticleSpartialOrb) += cij; }
                    else{ MOPrefactors(HoleSpartialOrb, ParticleSpartialOrb) -= cij; }
                }
            }
        }

        //2b. Calculate MOPrefactor-Matrix for Step before
        for(int i = 0; i < SDInteractions.size(); i++)
        {
            std::complex<double> cij = TD_VEC_before_transformed(SDInteractions[i].SD1) * std::conj(TD_VEC_before_transformed(SDInteractions[i].SD2));
            if(abs(cij) > cij_threshold)
            {
                if(SDInteractions[i].SD1 == SDInteractions[i].SD2)
                {   
                    for(int j = 0; j < SDInteractions[i].Orbitals.size(); j++)
                    {
                        int SpartialOrb = SDInteractions[i].Orbitals[j] / 2;
                        MOPrefactors(SpartialOrb, SpartialOrb) -= cij;
                    }
                }
                else
                {
                    int HoleSpartialOrb = SDInteractions[i].Orbitals[0] / 2;
                    int ParticleSpartialOrb = SDInteractions[i].Orbitals[1] / 2;
                    if(SDInteractions[i].PositivSign){ MOPrefactors(HoleSpartialOrb, ParticleSpartialOrb) -= cij; }
                    else{ MOPrefactors(HoleSpartialOrb, ParticleSpartialOrb) += cij; }
                }
            }
        }

        //std::cout << "MOPrefactor-Matrix" << std::endl;
        //std::cout << MOPrefactors << std::endl;

        //Translate MO*MO Matrix to AO*AO Matrix
        Eigen::MatrixXd HFCMatrix = *hfcmatrix;
        Eigen::MatrixXcd AOPrefactors = HFCMatrix * MOPrefactors * HFCMatrix.transpose();

        //3. Set PictureArray to Zero
        for (int i = 0; i < xdim * ydim * zdim; i ++) {
            PictureData[i] = 0;
        }

        //4. Translate MO*MO Matrix to Density
        double PlotAOAOTresh = 0.00000001;
        for(int i = 0; i < AOAOValueCuboids.size(); i++)
        {
            double AOAOPrefac = AOPrefactors(AOAOValueCuboids[i]->basisfunction_number1, AOAOValueCuboids[i]->basisfunction_number2).real();
            if(abs(AOAOPrefac) > PlotAOAOTresh)
            {
                AOAOValueCuboids[i]->AddAOAOToImageData(PictureData, AOAOPrefac);
            }
        }
        PlotMainWindow->RenderWidget->UpdateScreenWithExternalImageData(PictureData);
    }
}

void VisualizationTDCalculate::PlotNTODensity(int Timestep, int DensityType)
{
    UpdateGrid();
    if(AOAOCuboidForGridMustBeUpdated){CalculateAllAOAOCuboidForGrid();} 

	//************************************
	//Start Calculate NTOMatrix + NTOVector
    Eigen::MatrixXd CIMatrix = (*ciresults).ci_matrix;
    Eigen::MatrixXd HFCMatrix = *hfcmatrix;
    std::vector<std::string> confs = (*ciresults).configuration_strings;
    std::vector<Eigen::MatrixXcd> TDCIcomplexMatrix = TDCICoefficients->get_ci_vectors();
    

    int occupied_spin_orbitals = 0;
    std::string HF_String = confs[0];
    for (int i = 0; i < HF_String.size(); i++)
    {
        if (HF_String[i] == '1') { occupied_spin_orbitals++; }
    }
    int unoccupied_spin_orbitals = HF_String.size() - occupied_spin_orbitals;
    int occupied_spartial_orbitals = occupied_spin_orbitals / 2;
    int unoccupied_spartial_orbitals = unoccupied_spin_orbitals / 2;
    int total_spartial_orbitals = occupied_spartial_orbitals + unoccupied_spartial_orbitals;

    std::map<std::string, int> conf_map;
    for (int i = 0; i < confs.size(); i++)
    {
        std::string conf = confs[i];
        conf_map[conf] = i;
    }
    
    //OnePTDM Ground State Projected Out
    Eigen::MatrixXcd OnePTDM = Eigen::MatrixXcd::Zero(occupied_spartial_orbitals, unoccupied_spartial_orbitals);

    for (int occ = 0; occ < occupied_spartial_orbitals; occ++)
    {
        for (int virt = 0; virt < unoccupied_spartial_orbitals; virt++)
        {
            std::string alphastring = HF_String;
            alphastring[2 * occ] = '0';
            alphastring[2 * (occupied_spartial_orbitals + virt)] = '1';

            std::string betastring = HF_String;
            betastring[2 * occ + 1] = '0';
            betastring[2 * (occupied_spartial_orbitals + virt) + 1] = '1';


            for (int statenum = 0; statenum < CIMatrix.cols(); statenum++)
            {
                double alpha_value = (1 / sqrt(2)) * CIMatrix(conf_map[alphastring], statenum);
                double beta_value = (1 / sqrt(2)) * CIMatrix(conf_map[betastring], statenum);
                OnePTDM(occ, virt) += TDCIcomplexMatrix[Timestep](statenum, 0) * (alpha_value - beta_value);
            }
        }
    }

    Eigen::JacobiSVD<Eigen::MatrixXcd> svd(OnePTDM, Eigen::ComputeFullU | Eigen::ComputeFullV);

    Eigen::MatrixXcd NTOs_occ = HFCMatrix.block(0, 0, total_spartial_orbitals, occupied_spartial_orbitals) * svd.matrixU();
    Eigen::MatrixXcd NTOs_virt = HFCMatrix.block(0, occupied_spartial_orbitals, total_spartial_orbitals, unoccupied_spartial_orbitals) * svd.matrixV();

    Eigen::MatrixXcd NTO_matrix = Eigen::MatrixXcd::Zero(total_spartial_orbitals, total_spartial_orbitals);
    NTO_matrix << NTOs_occ.rowwise().reverse(), NTOs_virt;

    Eigen::VectorXd NTOVector = Eigen::VectorXd::Zero(total_spartial_orbitals);
    Eigen::VectorXd svdvec = svd.singularValues();


    for (int i = 0; i < svdvec.rows(); i++)
    {
        NTOVector[occupied_spartial_orbitals - 1 - i] = svdvec[i];
        NTOVector[occupied_spartial_orbitals + i] = svdvec[i];
    }


    Eigen::MatrixXd NTOPrefactors = Eigen::MatrixXd::Zero(total_spartial_orbitals, total_spartial_orbitals);
    if (DensityType == 0)
    {
        for (int nto = 0; nto < occupied_spartial_orbitals; nto++)
        {
            NTOPrefactors(nto, nto) = NTOVector(nto) * NTOVector(nto);
        }
    }
    else if (DensityType == 1)
    {
        for (int nto = occupied_spartial_orbitals; nto < 2 * occupied_spartial_orbitals; nto++)
        {
            NTOPrefactors(nto, nto) = NTOVector(nto) * NTOVector(nto);
        }
    }

    Eigen::MatrixXcd AOPrefactors = NTO_matrix * NTOPrefactors * NTO_matrix.adjoint();

    //3. Set PictureArray to Zero
    for (int i = 0; i < xdim * ydim * zdim; i ++) {
        PictureData[i] = 0;
    }

    //4. Translate AO*AO Matrix to Density
    double PlotAOAOTresh = 0.00000001;
    for(int i = 0; i < AOAOValueCuboids.size(); i++)
    {
        double AOAOPrefac = AOPrefactors(AOAOValueCuboids[i]->basisfunction_number1, AOAOValueCuboids[i]->basisfunction_number2).real();
        if(abs(AOAOPrefac) > PlotAOAOTresh)
        {
            AOAOValueCuboids[i]->AddAOAOToImageData(PictureData, AOAOPrefac);
        }
    }

    PlotMainWindow->RenderWidget->UpdateScreenWithExternalImageData(PictureData);

    /*
	//Start Calculate NTOValuesXYZ
    int NumberOfNTOs = basissetdata->get_Basisfunctionnumber_angular_expanded();
    std::vector<std::tuple<QC::Gaussian_Basisfunction, int>> basisfunctionsandangularmoment = basissetdata->get_all_Basisfunction_with_angular_expanded();
    std::complex<double>* NTOValuesXYZ = new std::complex<double>[NumberOfNTOs * xdim * ydim * zdim];
    Eigen::MatrixXd basisvalues = Eigen::MatrixXd(NumberOfNTOs, 1);
    for (int z = 0; z < zdim; z ++) {
        for (int y = 0; y < ydim; y++) {
            for (int x = 0; x < xdim; x++) {
                // for all basisfunctions
                for (int j = 0; j < NumberOfNTOs; j++)
                {
                    //get basisfunction
                    QC::Gaussian_Basisfunction basisfunction;
                    int angular_moment;
                    std::tie(basisfunction, angular_moment) = basisfunctionsandangularmoment[j];
                    //get value at point in space
                    double localvalue = basisfunction.get_basisfunction_value(basissetdata->get_basisset_type(), XGrid[x] - basisfunction.get_center_x(), YGrid[y] - basisfunction.get_center_y(), ZGrid[z] - basisfunction.get_center_z(), angular_moment);
                    basisvalues(j) = localvalue;
                }
                // 
                Eigen::MatrixXcd ntovalues = basisvalues.transpose() * NTO_matrix;
                // First Index of Position in space of MO_0
                int i = NumberOfNTOs * (x + xdim * (y + ydim * z));
                // for each MO_n - set value in space
                for (int n = 0; n < NumberOfNTOs; n++)
                {
                    std::complex<double> value = ntovalues(n);
                    NTOValuesXYZ[i + n] = value;
                }
            }
        }
    }
	//End Calculate NTOValuesXYZ
	//Start Calculate NTODensity
    for (int x = 0; x < xdim; x ++)
	{
		for (int y = 0; y < ydim; y++)
		{
			for (int z = 0; z < zdim; z++)
			{
				//get value at point in space
				double localvalue = 0;
				if (DensityType == 0)
				{
					for (int nto = 0; nto < occupied_spartial_orbitals; nto++)
					{
						int i = nto + NumberOfNTOs * (x + xdim * (y + ydim * z));
						double val = (NTOValuesXYZ[i] * std::conj(NTOValuesXYZ[i])).real();
						localvalue += val * NTOVector(nto) * NTOVector(nto);
					}
				}
				else if (DensityType == 1)
				{
					for (int nto = occupied_spartial_orbitals; nto < 2 * occupied_spartial_orbitals; nto++)
					{
						int i = nto + NumberOfNTOs * (x + xdim * (y + ydim * z));
						double val = (NTOValuesXYZ[i] * std::conj(NTOValuesXYZ[i])).real();
						localvalue += val * NTOVector(nto) * NTOVector(nto);
					}
				}

				// write density to pixel of ImageData
                PictureData[z * xdim * ydim + y * xdim + x] = localvalue;
			}
		}
	}

   delete[] NTOValuesXYZ;
   PlotMainWindow->RenderWidget->UpdateScreenWithExternalImageData(PictureData);
   */
}