#include "VisualizationTIDCalculate.h"
#include "plot_ci_new_window.h"

VisualizationTIDCalculate::VisualizationTIDCalculate(
	std::shared_ptr <std::vector<std::string>> ciconfigurationstrings,
	std::shared_ptr <Eigen::MatrixXd> cimatrix,
	std::shared_ptr <Eigen::VectorXd> cienergys,
	std::shared_ptr <Eigen::MatrixXd> hfcmatrix,
	std::shared_ptr <QC::Gaussian_Basisset> basissetdata,
	std::shared_ptr <std::vector<QC::Pointcharge>> pointcharges,
    Plot_CI_New_Window* PlotMainWindow
	)
{
    this->ciconfigurationstrings = ciconfigurationstrings;
    this->cimatrix = cimatrix;
    this->cienergys = cienergys;
    this->hfcmatrix = hfcmatrix;
    this->basissetdata = basissetdata;
    this->pointcharges = pointcharges;
    this->PlotMainWindow = PlotMainWindow;
}

VisualizationTIDCalculate::~VisualizationTIDCalculate()
{
    if(AOValuesXYZ != nullptr){ delete[] AOValuesXYZ; }
    if(MOValuesXYZ != nullptr){ delete[] MOValuesXYZ; }
    if(NTOValuesXYZ != nullptr){ delete[] NTOValuesXYZ; }
}

void VisualizationTIDCalculate::UpdateGrid()
{
    if(XGrid == nullptr || PlotMainWindow->GridWidget->UpdateGridIfNeeded())
    {
        xdim = PlotMainWindow->GridWidget->UsedGrid->xdim;
        ydim = PlotMainWindow->GridWidget->UsedGrid->ydim;
        zdim = PlotMainWindow->GridWidget->UsedGrid->zdim;
        XGrid = PlotMainWindow->GridWidget->UsedGrid->XGrid;
        YGrid = PlotMainWindow->GridWidget->UsedGrid->YGrid;
        ZGrid = PlotMainWindow->GridWidget->UsedGrid->ZGrid;

        PlotMainWindow->RenderWidget->imageData->SetDimensions(xdim, ydim, zdim);
	    PlotMainWindow->RenderWidget->imageData->AllocateScalars(VTK_DOUBLE, 1);

        if(AOValuesXYZ != nullptr){ delete[] AOValuesXYZ; AOValuesXYZ = nullptr;}
        if(MOValuesXYZ != nullptr){ delete[] MOValuesXYZ; MOValuesXYZ = nullptr;}
        if(NTOValuesXYZ != nullptr){ delete[] NTOValuesXYZ; NTOValuesXYZ = nullptr;}
        PlotMainWindow->LogWidget->AddToLog(0, "Grid updated");
    }
}

//****************************** Helper Functions ******************************
void VisualizationTIDCalculate::PrecomputeAllAOsOnGrid()
{
    int NumberOfAOs = basissetdata->get_Basisfunctionnumber_angular_expanded();
    AOValuesXYZ = new double[NumberOfAOs*xdim*ydim*zdim];
    std::vector<std::tuple<QC::Gaussian_Basisfunction, int>> basisfunctionsandangularmoment = basissetdata->get_all_Basisfunction_with_angular_expanded();

    for(int AO = 0; AO < NumberOfAOs; AO++){
        QC::Gaussian_Basisfunction basisfunction;
        int angular_moment;
        std::tie(basisfunction, angular_moment) = basisfunctionsandangularmoment[AO];
        for (int z = 0; z < zdim; z ++) {
            for (int y = 0; y < ydim; y++) {
                for (int x = 0; x < xdim; x++) {
                    int i = AO + NumberOfAOs * (x + xdim * (y + ydim * z));
                    //get value at point in space
                    double localvalue = basisfunction.get_basisfunction_value(basissetdata->get_basisset_type(), XGrid[x] - basisfunction.get_center_x(), YGrid[y] - basisfunction.get_center_y(), ZGrid[z] - basisfunction.get_center_z(), angular_moment);
                    AOValuesXYZ[i] = localvalue;
                }
            }
        }
    }
    PlotMainWindow->LogWidget->AddToLog(0, "AOs on Grid updated");
}

void VisualizationTIDCalculate::CalculateAllAOCuboidForGrid()
{
    AOValueCuboids.clear();
    double radius = 2;

    int NumberOfAOs = basissetdata->get_Basisfunctionnumber_angular_expanded();
    std::vector<std::tuple<QC::Gaussian_Basisfunction, int>> basisfunctionsandangularmoment = basissetdata->get_all_Basisfunction_with_angular_expanded();
    for(int i = 0; i < NumberOfAOs; i++)
    {
        QC::Gaussian_Basisfunction basisfunction;
        int angular_moment;
        std::tie(basisfunction, angular_moment) = basisfunctionsandangularmoment[i];
        std::shared_ptr<AOValueCuboid> Cuboid = std::make_shared<AOValueCuboid>(i, basisfunction, basissetdata->get_basisset_type(), angular_moment, PlotMainWindow->GridWidget->UsedGrid, radius);

        AOValueCuboids.push_back(Cuboid);
    }
}

void VisualizationTIDCalculate::CalculateAllAOAOCuboidForGrid()
{
    AOAOValueCuboids.clear();
    double radius = 5;

    int NumberOfAOs = basissetdata->get_Basisfunctionnumber_angular_expanded();
    std::vector<std::tuple<QC::Gaussian_Basisfunction, int>> basisfunctionsandangularmoment = basissetdata->get_all_Basisfunction_with_angular_expanded();
    for(int i = 0; i < NumberOfAOs; i++)
    {
        for(int j = 0; j < NumberOfAOs; j++)
        {
            QC::Gaussian_Basisfunction basisfunction1;
            int angular_moment1;
            std::tie(basisfunction1, angular_moment1) = basisfunctionsandangularmoment[i];
            QC::Gaussian_Basisfunction basisfunction2;
            int angular_moment2;
            std::tie(basisfunction2, angular_moment2) = basisfunctionsandangularmoment[j];

            std::shared_ptr<AOAOValueCuboid> Cuboid = std::make_shared<AOAOValueCuboid>(i, basisfunction1, angular_moment1, j, basisfunction2, angular_moment2, basissetdata->get_basisset_type(), PlotMainWindow->GridWidget->UsedGrid, radius);

            AOAOValueCuboids.push_back(Cuboid);
        }
    }
}

void VisualizationTIDCalculate::PrecomputeAllMOsOnGrid()
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

void VisualizationTIDCalculate::CalculateAllSDInteractions()
{   
    //Create List of all Slater-Determinant Interactions != 0
    SDInteractions.clear();
    std::vector<std::string> cistrings = *ciconfigurationstrings;
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

void VisualizationTIDCalculate::Calculate1PTDM(int FinalState)
{
    std::vector<std::string> confs = *ciconfigurationstrings;
    Eigen::MatrixXd HFCMatrix = *hfcmatrix;
    Eigen::MatrixXd CIMatrix = *cimatrix;

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

    Eigen::MatrixXd OnePTDM = Eigen::MatrixXd::Zero(total_spartial_orbitals, total_spartial_orbitals);

    for (int p = 0; p < total_spartial_orbitals; p++)
    {
        for (int q = 0; q < total_spartial_orbitals; q++)
        {
            if (p >= occupied_spartial_orbitals && q < occupied_spartial_orbitals)
            {
                std::string alphastring = HF_String;
                alphastring[2 * q] = '0';
                alphastring[2 * p] = '1';

                std::string betastring = HF_String;
                betastring[2 * q + 1] = '0';
                betastring[2 * p + 1] = '1';

                double alpha_value = (1 / sqrt(2)) * CIMatrix(conf_map[alphastring], FinalState);
                double beta_value = (1 / sqrt(2)) * CIMatrix(conf_map[betastring], FinalState);
                OnePTDM(p, q) += (alpha_value - beta_value);
            }
            else if (p < occupied_spartial_orbitals && p == q)
            {
                OnePTDM(p, q) += CIMatrix(conf_map[HF_String], FinalState);
            }
            else
            {
                OnePTDM(p, q) = 0;
            }
        }
    }

    Eigen::JacobiSVD<Eigen::MatrixXd> svd(OnePTDM.adjoint(), Eigen::ComputeFullU | Eigen::ComputeFullV);

    Eigen::MatrixXd NTOs_occ = HFCMatrix * svd.matrixU();
    Eigen::MatrixXd NTOs_virt = HFCMatrix * svd.matrixV();

    //std::cout << NTOs_occ << std::endl << std::endl;
    //std::cout << NTOs_virt << std::endl << std::endl;
    //std::cout << svd.singularValues() << std::endl;

    NTO_matrix = Eigen::MatrixXd::Zero(total_spartial_orbitals, total_spartial_orbitals);
    NTO_matrix << NTOs_occ.block(0, 0, total_spartial_orbitals, occupied_spartial_orbitals).rowwise().reverse(), NTOs_virt.block(0, 0, total_spartial_orbitals, unoccupied_spartial_orbitals);

    NTOVector = Eigen::VectorXd::Zero(total_spartial_orbitals);
    Eigen::VectorXd svdvec = svd.singularValues();

    for (int i = 0; i < NTOVector.rows(); i++)
    {
        if (i < occupied_spartial_orbitals)
        {
            NTOVector[i] = svdvec[occupied_spartial_orbitals - 1 - i];
        }
        else if(i >= occupied_spartial_orbitals && i < 2 * occupied_spartial_orbitals)
        {
            NTOVector[i] = svdvec[i - occupied_spartial_orbitals];
        }
    }

    NTO_OccupiedOrbitals = occupied_spartial_orbitals;
    NTO_FinalState = FinalState;
    if(NTOValuesXYZ != nullptr){ delete[] NTOValuesXYZ; NTOValuesXYZ = nullptr;}
    PlotMainWindow->LogWidget->AddToLog(0, "1PTDM calculated");
}

void VisualizationTIDCalculate::PrecomputeAllNTOsOnGrid()
{
    int NumberOfNTOs = basissetdata->get_Basisfunctionnumber_angular_expanded();
    NTOValuesXYZ = new double[NumberOfNTOs*xdim*ydim*zdim];

    std::vector<std::tuple<QC::Gaussian_Basisfunction, int>> basisfunctionsandangularmoment = basissetdata->get_all_Basisfunction_with_angular_expanded();
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
				Eigen::MatrixXd ntovalues = basisvalues.transpose() * NTO_matrix;
				// First Index of Position in space of MO_0
				int i = NumberOfNTOs * (x + xdim * (y + ydim * z));
				// for each MO_n - set value in space
				for (int n = 0; n < NumberOfNTOs; n++)
				{
					double value = ntovalues(n);
					NTOValuesXYZ[i + n] = value;
				}
			}
		}
    }
    PlotMainWindow->LogWidget->AddToLog(0, "NTOs on Grid updated");
}

//****************************** Plot Functions ******************************
void VisualizationTIDCalculate::PlotBasisfunction(int BasisfunctionNumber)
{
    UpdateGrid();
    std::vector<std::tuple<QC::Gaussian_Basisfunction, int>> basisfunctionsandangularmoment = basissetdata->get_all_Basisfunction_with_angular_expanded();
    for (int z = 0; z < zdim; z++) {
		for (int y = 0; y < ydim; y++) {
			for (int x = 0; x < xdim; x++) {
                QC::Gaussian_Basisfunction basisfunction;
				int angular_moment;
				std::tie(basisfunction, angular_moment) = basisfunctionsandangularmoment[BasisfunctionNumber];
                double localvalue = basisfunction.get_basisfunction_value(basissetdata->get_basisset_type(), XGrid[x] - basisfunction.get_center_x(), YGrid[y] - basisfunction.get_center_y(), ZGrid[z] - basisfunction.get_center_z(), angular_moment);
                double* pixel = static_cast<double*>(PlotMainWindow->RenderWidget->imageData->GetScalarPointer(x, y, z));
				pixel[0] = localvalue;
            }
        }
    }
    PlotMainWindow->RenderWidget->UpdateScreen();
}

void VisualizationTIDCalculate::PlotMoleculeOrbital(int MoleculeOrbitalNumber)
{
    UpdateGrid();
    CalculateAllAOCuboidForGrid();
    //if(AOValuesXYZ == nullptr){ PrecomputeAllAOsOnGrid();}
    int NumberOfAOs = basissetdata->get_Basisfunctionnumber_angular_expanded();
    Eigen::MatrixXd HFCMatrix = *hfcmatrix;
    Eigen::VectorXd C_Vec = HFCMatrix.col(MoleculeOrbitalNumber);

    //Set PictureArray to Zero
    for (int z = 0; z < zdim; z ++) {
        for (int y = 0; y < ydim; y++) {
            for (int x = 0; x < xdim; x++) {
                double* pixel = static_cast<double*>(PlotMainWindow->RenderWidget->imageData->GetScalarPointer(x, y, z));
                pixel[0] = 0;
            }
        }
    }

    for(int i = 0; i < AOValueCuboids.size(); i++)
    {
        AOValueCuboids[i]->AddAOToImageData(PlotMainWindow->RenderWidget->imageData, C_Vec[AOValueCuboids[i]->basisfunction_number]);
    }
    
    /*
    for(int AO = 0; AO < NumberOfAOs; AO++){
        for (int z = 0; z < zdim; z ++) {
		    for (int y = 0; y < ydim; y++) {
			    for (int x = 0; x < xdim; x++) {
                    //get value at point in space
                    int i = AO + NumberOfAOs * (x + xdim * (y + ydim * z));
                    double localvalue = C_Vec[AO] * AOValuesXYZ[i];
                    double* pixel = static_cast<double*>(PlotMainWindow->RenderWidget->imageData->GetScalarPointer(x, y, z));
                    pixel[0] += localvalue;
                }
			}
		}
	}
    */

    /*
    if(MOValuesXYZ == nullptr){ PrecomputeAllMOsOnGrid();}
    int NumberOfMOs = basissetdata->get_Basisfunctionnumber_angular_expanded();
    for (int z = 0; z < zdim; z ++) {
		for (int y = 0; y < ydim; y++) {
			for (int x = 0; x < xdim; x++) {
				//get value at point in space
				int i = MoleculeOrbitalNumber + NumberOfMOs * (x + xdim * (y + ydim * z));
				double localvalue = MOValuesXYZ[i];
				double* pixel = static_cast<double*>(PlotMainWindow->RenderWidget->imageData->GetScalarPointer(x, y, z));
				pixel[0] = localvalue;
			}
		}
	}
    */
    PlotMainWindow->RenderWidget->UpdateScreen();
}

void VisualizationTIDCalculate::PlotMoleculeOrbitalDensity(int MoleculeOrbitalNumber)
{
    UpdateGrid();
    if(MOValuesXYZ == nullptr){PrecomputeAllMOsOnGrid();}

    int NumberOfMOs = basissetdata->get_Basisfunctionnumber_angular_expanded();
    for (int z = 0; z < zdim; z ++) {
		for (int y = 0; y < ydim; y++) {
			for (int x = 0; x < xdim; x++) {
				//get value at point in space
				int i = MoleculeOrbitalNumber + NumberOfMOs * (x + xdim * (y + ydim * z));
				double localvalue = MOValuesXYZ[i];
                double density = localvalue * localvalue;
				double* pixel = static_cast<double*>(PlotMainWindow->RenderWidget->imageData->GetScalarPointer(x, y, z));
				pixel[0] = density;
			}
		}
	}
    PlotMainWindow->RenderWidget->UpdateScreen();
}

void VisualizationTIDCalculate::PlotCIStateDensity(int CIStateDensityNumber)
{
    CalculateAllAOCuboidForGrid();    
    UpdateGrid();
    if(AOValuesXYZ == nullptr){PrecomputeAllAOsOnGrid();}
    if(MOValuesXYZ == nullptr){PrecomputeAllMOsOnGrid();}
    if(!SDInteractionsCalculated){CalculateAllSDInteractions();}

    int NumberOfMOs = basissetdata->get_Basisfunctionnumber_angular_expanded();

    //Calculates Matrix of MO*MO Entries
    Eigen::MatrixXd MOPrefactors = Eigen::MatrixXd::Zero(NumberOfMOs, NumberOfMOs);
    Eigen::MatrixXd CIMatrix = *(cimatrix);
    double cij_threshold = 0;
    for(int i = 0; i < SDInteractions.size(); i++)
    {
        double cij = CIMatrix(SDInteractions[i].SD1, CIStateDensityNumber) * CIMatrix(SDInteractions[i].SD2, CIStateDensityNumber);
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
    //std::cout << MOPrefactors << std::endl;

    //Translate MO*MO Matrix to AO*AO Matrix
    Eigen::MatrixXd HFCMatrix = *hfcmatrix;
    Eigen::MatrixXd AOPrefactors = HFCMatrix * MOPrefactors * HFCMatrix.transpose();
    //std::cout << AOPrefactors << std::endl;

    //Set PictureArray to Zero
    for (int z = 0; z < zdim; z ++) {
        for (int y = 0; y < ydim; y++) {
            for (int x = 0; x < xdim; x++) {
                double* pixel = static_cast<double*>(PlotMainWindow->RenderWidget->imageData->GetScalarPointer(x, y, z));
                pixel[0] = 0;
            }
        }
    }

    for(int i = 0; i < AOAOValueCuboids.size(); i++)
    {
        AOAOValueCuboids[i]->AddAOAOToImageData(PlotMainWindow->RenderWidget->imageData, AOPrefactors(AOAOValueCuboids[i]->basisfunction_number1, AOAOValueCuboids[i]->basisfunction_number2));
    }

    /*
    //Translate AO*AO Matrix to Density
    int NumberOfAOs = NumberOfMOs;
    double AOij_threshold = 0;
    for(int i = 0; i < AOPrefactors.rows(); i++)
    {
        for(int j = 0; j < AOPrefactors.cols(); j++)
        {
            double AO_ij = AOPrefactors(i,j);
            if(abs(AO_ij) > AOij_threshold)
            {
                for (int z = 0; z < zdim; z ++) {
		            for (int y = 0; y < ydim; y++) {
			            for (int x = 0; x < xdim; x++) {
                            //get value at point in space
                            int idx_i = i + NumberOfAOs * (x + xdim * (y + ydim * z));
                            int idx_j = j + NumberOfAOs * (x + xdim * (y + ydim * z));
                            double density = AO_ij * AOValuesXYZ[idx_i] * AOValuesXYZ[idx_j];
                            double* pixel = static_cast<double*>(PlotMainWindow->RenderWidget->imageData->GetScalarPointer(x, y, z));
                            pixel[0] += density;
                        }
                    }
                }
            }
        }
    }
    */

    /*
    //Translate MO*MO Matrix to Density
    double MOij_threshold = 0;
    for(int i = 0; i < MOPrefactors.rows(); i++)
    {
        for(int j = 0; j < MOPrefactors.cols(); j++)
        {
            double MO_ij = MOPrefactors(i,j);
            if(abs(MO_ij) > MOij_threshold)
            {
                for (int z = 0; z < zdim; z ++) {
		            for (int y = 0; y < ydim; y++) {
			            for (int x = 0; x < xdim; x++) {
                            //get value at point in space
                            int idx_i = i + NumberOfMOs * (x + xdim * (y + ydim * z));
                            int idx_j = j + NumberOfMOs * (x + xdim * (y + ydim * z));
                            double density = MO_ij * MOValuesXYZ[idx_i] * MOValuesXYZ[idx_j];
                            double* pixel = static_cast<double*>(PlotMainWindow->RenderWidget->imageData->GetScalarPointer(x, y, z));
                            pixel[0] += density;
                        }
                    }
                }
            }
        }
    }
    */
    PlotMainWindow->RenderWidget->UpdateScreen();
}

void VisualizationTIDCalculate::PlotCIStateDensityDifference(int CIStateNumber1, int CIStateNumber2)
{

    UpdateGrid();
    if(MOValuesXYZ == nullptr){PrecomputeAllMOsOnGrid();}
    if(!SDInteractionsCalculated){CalculateAllSDInteractions();}

    int NumberOfMOs = basissetdata->get_Basisfunctionnumber_angular_expanded();

    //Calculates Matrix of MO*MO Entries
    Eigen::MatrixXd MOPrefactors = Eigen::MatrixXd::Zero(NumberOfMOs, NumberOfMOs);
    Eigen::MatrixXd CIMatrix = *(cimatrix);
    double cij_threshold = 0;

    for(int i = 0; i < SDInteractions.size(); i++)
    {
        double cij = CIMatrix(SDInteractions[i].SD1, CIStateNumber1) * CIMatrix(SDInteractions[i].SD2, CIStateNumber1);
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

    for(int i = 0; i < SDInteractions.size(); i++)
    {
        double cij = CIMatrix(SDInteractions[i].SD1, CIStateNumber2) * CIMatrix(SDInteractions[i].SD2, CIStateNumber2);
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
    //std::cout << MOPrefactors << std::endl;

    //Set PictureArray to Zero
    for (int z = 0; z < zdim; z ++) {
        for (int y = 0; y < ydim; y++) {
            for (int x = 0; x < xdim; x++) {
                double* pixel = static_cast<double*>(PlotMainWindow->RenderWidget->imageData->GetScalarPointer(x, y, z));
                pixel[0] = 0;
            }
        }
    }

    //Translate MO*MO Matrix to Density
    double MOij_threshold = 0;
    for(int i = 0; i < MOPrefactors.rows(); i++)
    {
        for(int j = 0; j < MOPrefactors.cols(); j++)
        {
            double MO_ij = MOPrefactors(i,j);
            if(abs(MO_ij) > MOij_threshold)
            {
                for (int z = 0; z < zdim; z ++) {
		            for (int y = 0; y < ydim; y++) {
			            for (int x = 0; x < xdim; x++) {
                            //get value at point in space
                            int idx_i = i + NumberOfMOs * (x + xdim * (y + ydim * z));
                            int idx_j = j + NumberOfMOs * (x + xdim * (y + ydim * z));
                            double density = MO_ij * MOValuesXYZ[idx_i] * MOValuesXYZ[idx_j];
                            double* pixel = static_cast<double*>(PlotMainWindow->RenderWidget->imageData->GetScalarPointer(x, y, z));
                            pixel[0] += density;
                        }
                    }
                }
            }
        }
    }
    PlotMainWindow->RenderWidget->UpdateScreen();
}

void VisualizationTIDCalculate::PlotNTO(int FinalState, int NTONumber)
{
    UpdateGrid();
    if(NTONumber == -1){ return; }
    if(FinalState != this->NTO_FinalState){ Calculate1PTDM(FinalState); }
	if(NTOValuesXYZ == nullptr){ PrecomputeAllNTOsOnGrid();}

    int NumberOfNTOs = basissetdata->get_Basisfunctionnumber_angular_expanded();
    for (int z = 0; z < zdim; z ++) {
		for (int y = 0; y < ydim; y++) {
			for (int x = 0; x < xdim; x++) {
				int i = NTONumber + NumberOfNTOs * (x + xdim * (y + ydim * z));
				double localvalue = NTOValuesXYZ[i];
				double* pixel = static_cast<double*>(PlotMainWindow->RenderWidget->imageData->GetScalarPointer(x, y, z));
				pixel[0] = localvalue;
			}
		}
	}
    PlotMainWindow->RenderWidget->UpdateScreen();
}

void VisualizationTIDCalculate::PlotNTODensity(int FinalState, int NTODensityNumber)
{
    UpdateGrid();
    if(FinalState != this->NTO_FinalState){ Calculate1PTDM(FinalState); }
	if(NTOValuesXYZ == nullptr){ PrecomputeAllNTOsOnGrid();}

    int NumberOfNTOs = basissetdata->get_Basisfunctionnumber_angular_expanded();
    for (int z = 0; z < zdim; z ++) {
		for (int y = 0; y < ydim; y++) {
			for (int x = 0; x < xdim; x++) {
                double localvalue = 0;
                if (NTODensityNumber == 0)
				{
					for (int nto = 0; nto < NTO_OccupiedOrbitals; nto++)
					{
						int i = nto + NumberOfNTOs * (x + xdim * (y + ydim * z));
						double val = NTOValuesXYZ[i];
						localvalue += (val*val) * NTOVector(nto) * NTOVector(nto);
					}
				}
				else if (NTODensityNumber == 1)
				{
					for (int nto = NTO_OccupiedOrbitals; nto < 2 * NTO_OccupiedOrbitals; nto++)
					{
						int i = nto + NumberOfNTOs * (x + xdim * (y + ydim * z));
						double val = NTOValuesXYZ[i];
						localvalue += (val * val) * NTOVector(nto) * NTOVector(nto);
					}
				}
				double* pixel = static_cast<double*>(PlotMainWindow->RenderWidget->imageData->GetScalarPointer(x, y, z));
				pixel[0] = localvalue;
			}
		}
	}
    PlotMainWindow->RenderWidget->UpdateScreen();
}