#include "ValueCuboid.h"

AOValueCuboid::AOValueCuboid(
    int basisfunction_number, 
    QC::Gaussian_Basisfunction basisfunction, 
    int basissettype, 
    int angular_moment, 
    std::shared_ptr<Grid> MainGrid)
{
    this->basisfunction_number = basisfunction_number;
    this->x_dim = MainGrid->xdim;
    this->y_dim = MainGrid->ydim;
    this->z_dim = MainGrid->zdim;
    //Calculate nearest Grid Point for Basisfunctioncenter
    int nearest_center_x = int(std::round((basisfunction.get_center_x() - MainGrid->realxmin) / MainGrid->dx));
    int nearest_center_y = int(std::round((basisfunction.get_center_y() - MainGrid->realymin) / MainGrid->dy));
    int nearest_center_z = int(std::round((basisfunction.get_center_z() - MainGrid->realzmin) / MainGrid->dz));

    //std::cout << "z-Center: " << basisfunction.get_center_z() << std::endl;
    //std::cout << "Nearest-GridPoint: " << MainGrid->ZGrid[nearest_center_z] << "Lower: " << MainGrid->ZGrid[nearest_center_z-1] << "Higher: " << MainGrid->ZGrid[nearest_center_z+1] << std::endl;

    //Transform Radius from FWHM to au
    double FWHM = 2.35482 * sqrt(1 / (2 * basisfunction.get_smallestexponent()));
    double radius_au = FWHM * MainGrid->truncationradius;

    //Calculate indices for radius
    x_low = int(std::floor((basisfunction.get_center_x() - radius_au - MainGrid->realxmin) / MainGrid->dx)); //Round down
    y_low = int(std::floor((basisfunction.get_center_y() - radius_au - MainGrid->realymin) / MainGrid->dy)); //Round down
    z_low = int(std::floor((basisfunction.get_center_z() - radius_au - MainGrid->realzmin) / MainGrid->dz)); //Round down 

    x_high = int(std::ceil((basisfunction.get_center_x() + radius_au - MainGrid->realxmin) / MainGrid->dx)); //Round up
    y_high = int(std::ceil((basisfunction.get_center_y() + radius_au - MainGrid->realymin) / MainGrid->dy)); //Round up
    z_high = int(std::ceil((basisfunction.get_center_z() + radius_au - MainGrid->realzmin) / MainGrid->dz)); //Round up

    //Cut off values at the edge
    x_low = std::max(x_low, 0);
    y_low = std::max(y_low, 0);
    z_low = std::max(z_low, 0);

    x_high = std::min(x_high, MainGrid->xdim-1);
    y_high = std::min(y_high, MainGrid->ydim-1);
    z_high = std::min(z_high, MainGrid->zdim-1);

    //std::cout << "z-Center: " << basisfunction.get_center_z() << std::endl;
    //std::cout << "z_low: " << MainGrid->ZGrid[z_low] << " z_high: " << MainGrid->ZGrid[z_high] << std::endl;

    //Calculate Values in Cube and save
    Values = new double[(1 + x_high - x_low) * (1 + y_high - y_low) * (1 + z_high - z_low)];

    int idx = 0;
    for(int x = x_low; x <= x_high; x++){
        for(int y = y_low; y <= y_high; y++){
            for(int z = z_low; z <= z_high; z++){
                Values[idx] = basisfunction.get_basisfunction_value(basissettype, MainGrid->XGrid[x] - basisfunction.get_center_x(), MainGrid->YGrid[y] - basisfunction.get_center_y(), MainGrid->ZGrid[z] - basisfunction.get_center_z(), angular_moment);
                idx++;
            }
        }
    }
}

AOValueCuboid::~AOValueCuboid()
{
    delete[] Values;
}

void AOValueCuboid::AddAOToImageData(double* PictureData, double coeff)
{
    int yz_vals = (1 + y_high - y_low) * (1 + z_high - z_low);
    int z_vals = 1 + z_high - z_low;

    int xy_dim = x_dim * y_dim;
    for(int x = x_low; x <= x_high; x++){
        for(int y = y_low; y <= y_high; y++){
            for(int z = z_low; z <= z_high; z++){
                int idx = ((x - x_low) * yz_vals) + ((y - y_low) * z_vals) + (z - z_low);
                PictureData[z * xy_dim + y * x_dim + x] += coeff * Values[idx];
            }
        }
    }
}

void AOValueCuboid::AddAOSquareToImageData(double* PictureData, double coeff)
{
    int yz_vals = (1 + y_high - y_low) * (1 + z_high - z_low);
    int z_vals = 1 + z_high - z_low;

    int xy_dim = x_dim * y_dim;
    for(int x = x_low; x <= x_high; x++){
        for(int y = y_low; y <= y_high; y++){
            for(int z = z_low; z <= z_high; z++){
                int idx = ((x - x_low) * yz_vals) + ((y - y_low) * z_vals) + (z - z_low);
                PictureData[z * xy_dim + y * x_dim + x] += coeff * coeff * Values[idx] * Values[idx];
            }
        }
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////

AOAOValueCuboid::AOAOValueCuboid(
    int basisfunction_number1, 
    QC::Gaussian_Basisfunction basisfunction1,
    int angular_moment1,
    int basisfunction_number2, 
    QC::Gaussian_Basisfunction basisfunction2,
    int angular_moment2,
    int basissettype, 
    std::shared_ptr<Grid> MainGrid)
{
    this->basisfunction_number1 = basisfunction_number1;
    this->basisfunction_number2 = basisfunction_number2;

    this->x_dim = MainGrid->xdim;
    this->y_dim = MainGrid->ydim;
    this->z_dim = MainGrid->zdim;

    //Transform Radius from FWHM to au for both Basisfunctions
    double FWHM1 = 2.35482 * sqrt(1 / (2 * basisfunction1.get_smallestexponent()));
    double FWHM2 = 2.35482 * sqrt(1 / (2 * basisfunction2.get_smallestexponent()));
    double radius_au_1 = FWHM1 * MainGrid->truncationradius;
    double radius_au_2 = FWHM2 * MainGrid->truncationradius;

    double distance_x = basisfunction1.get_center_x() - basisfunction2.get_center_x();
    double distance_y = basisfunction1.get_center_y() - basisfunction2.get_center_y();
    double distance_z = basisfunction1.get_center_z() - basisfunction2.get_center_z();
    double distance = sqrt(distance_x * distance_x + distance_y * distance_y + distance_z * distance_z);
    if(radius_au_1 + radius_au_2 < distance)
    {
        AOsHaveOverlap = false;
        return;
    }
    else
    {
        AOsHaveOverlap = true;
    }


    //Calculate indices for radius for Basisfunction 1
    int x_low_1 = int(std::floor((basisfunction1.get_center_x() - radius_au_1 - MainGrid->realxmin) / MainGrid->dx)); //Round down
    int y_low_1 = int(std::floor((basisfunction1.get_center_y() - radius_au_1 - MainGrid->realymin) / MainGrid->dy)); //Round down
    int z_low_1 = int(std::floor((basisfunction1.get_center_z() - radius_au_1 - MainGrid->realzmin) / MainGrid->dz)); //Round down 

    int x_high_1 = int(std::ceil((basisfunction1.get_center_x() + radius_au_1 - MainGrid->realxmin) / MainGrid->dx)); //Round up
    int y_high_1 = int(std::ceil((basisfunction1.get_center_y() + radius_au_1 - MainGrid->realymin) / MainGrid->dy)); //Round up
    int z_high_1 = int(std::ceil((basisfunction1.get_center_z() + radius_au_1 - MainGrid->realzmin) / MainGrid->dz)); //Round up

    //Cut off values at the edge
    x_low_1 = std::max(x_low_1, 0);
    y_low_1 = std::max(y_low_1, 0);
    z_low_1 = std::max(z_low_1, 0);

    x_low_1 = std::min(x_low_1, MainGrid->xdim-1);
    y_low_1 = std::min(y_low_1, MainGrid->ydim-1);
    z_low_1 = std::min(z_low_1, MainGrid->zdim-1);

    x_high_1 = std::max(x_high_1, 0);
    y_high_1 = std::max(y_high_1, 0);
    z_high_1 = std::max(z_high_1, 0);

    x_high_1 = std::min(x_high_1, MainGrid->xdim-1);
    y_high_1 = std::min(y_high_1, MainGrid->ydim-1);
    z_high_1 = std::min(z_high_1, MainGrid->zdim-1);


    //Calculate indices for radius for Basisfunction 2
    int x_low_2 = int(std::floor((basisfunction2.get_center_x() - radius_au_2 - MainGrid->realxmin) / MainGrid->dx)); //Round down
    int y_low_2 = int(std::floor((basisfunction2.get_center_y() - radius_au_2 - MainGrid->realymin) / MainGrid->dy)); //Round down
    int z_low_2 = int(std::floor((basisfunction2.get_center_z() - radius_au_2 - MainGrid->realzmin) / MainGrid->dz)); //Round down 

    int x_high_2 = int(std::ceil((basisfunction2.get_center_x() + radius_au_2 - MainGrid->realxmin) / MainGrid->dx)); //Round up
    int y_high_2 = int(std::ceil((basisfunction2.get_center_y() + radius_au_2 - MainGrid->realymin) / MainGrid->dy)); //Round up
    int z_high_2 = int(std::ceil((basisfunction2.get_center_z() + radius_au_2 - MainGrid->realzmin) / MainGrid->dz)); //Round up

    //Cut off values at the edge
    x_low_2 = std::max(x_low_2, 0);
    y_low_2 = std::max(y_low_2, 0);
    z_low_2 = std::max(z_low_2, 0);

    x_low_2 = std::min(x_low_2, MainGrid->xdim-1);
    y_low_2 = std::min(y_low_2, MainGrid->ydim-1);
    z_low_2 = std::min(z_low_2, MainGrid->zdim-1);

    x_high_2 = std::max(x_high_2, 0);
    y_high_2 = std::max(y_high_2, 0);
    z_high_2 = std::max(z_high_2, 0);

    x_high_2 = std::min(x_high_2, MainGrid->xdim-1);
    y_high_2 = std::min(y_high_2, MainGrid->ydim-1);
    z_high_2 = std::min(z_high_2, MainGrid->zdim-1);

    //Calculate Final indices
    x_low = std::max(x_low_1, x_low_2);
    y_low = std::max(y_low_1, y_low_2);
    z_low = std::max(z_low_1, z_low_2);

    x_high = std::min(x_high_1, x_high_2);
    y_high = std::min(y_high_1, y_high_2);
    z_high = std::min(z_high_1, z_high_2);

    //Calculate Values in Cube and save
    int ValueNumber = (1 + x_high - x_low) * (1 + y_high - y_low) * (1 + z_high - z_low);
    Values = new double[ValueNumber];

    int idx = 0;
    for(int x = x_low; x <= x_high; x++){
        for(int y = y_low; y <= y_high; y++){
            for(int z = z_low; z <= z_high; z++){
                double Basisval_1 = basisfunction1.get_basisfunction_value(basissettype, MainGrid->XGrid[x] - basisfunction1.get_center_x(), MainGrid->YGrid[y] - basisfunction1.get_center_y(), MainGrid->ZGrid[z] - basisfunction1.get_center_z(), angular_moment1);
                double Basisval_2 = basisfunction2.get_basisfunction_value(basissettype, MainGrid->XGrid[x] - basisfunction2.get_center_x(), MainGrid->YGrid[y] - basisfunction2.get_center_y(), MainGrid->ZGrid[z] - basisfunction2.get_center_z(), angular_moment2);
                Values[idx] = Basisval_1 * Basisval_2;
                idx++;
            }
        }
    }

    //std::cout << 1 + x_high - x_low << " * "  << 1 + y_high - y_low << " * " << 1 + z_high - z_low << " Points in Cube" << std::endl;
}

AOAOValueCuboid::~AOAOValueCuboid()
{
    delete[] Values;
}

void AOAOValueCuboid::AddAOAOToImageData(double* PictureData, double coeff)
{
    if(basisfunction_number1 != basisfunction_number2){coeff = 2 * coeff;}
    //Unparallel variant (faster)
    int yz_vals = (1 + y_high - y_low) * (1 + z_high - z_low);
    int z_vals = 1 + z_high - z_low;

    int xy_dim = x_dim * y_dim;
    for(int x = x_low; x <= x_high; x++){
        for(int y = y_low; y <= y_high; y++){
            for(int z = z_low; z <= z_high; z++){
                int idx = ((x - x_low) * yz_vals) + ((y - y_low) * z_vals) + (z - z_low);
                PictureData[z * xy_dim + y * x_dim + x] += coeff * Values[idx];

                //double pixel = coeff * Values[idx];
                //double* pixel = static_cast<double*>(imageData->GetScalarPointer(x, y, z));
                //pixel[0] += coeff * Values[idx];
            }
        }
    }

    /*
    //Parallel variant: too few elements per thread to get an advantage
    int poolSize = std::thread::hardware_concurrency();
    std::vector<std::thread> ts(poolSize);
	for (int i = 0; i < poolSize; i++) {
		//ts[i] = std::thread([this](int i, double* minvals, double* maxvals) {ThreadCalculateHFDensity(i, minvals, maxvals); }, i, minvals, maxvals);
        ts[i] = std::thread(&AOAOValueCuboid::ThreadAddAOAOToImageData, this, PictureData, coeff, poolSize, i);
	}
	//wait for threads to finish
	for (int i = 0; i < poolSize; i++) {
		ts[i].join();
	}
    */
}

void AOAOValueCuboid::ThreadAddAOAOToImageData(double* PictureData, double coeff, int poolsize, int id)
{
    int yz_vals = (1 + y_high - y_low) * (1 + z_high - z_low);
    int z_vals = 1 + z_high - z_low;

    int xy_dim = x_dim * y_dim;
    for(int x = x_low + id; x <= x_high; x+= poolsize){
        for(int y = y_low; y <= y_high; y++){
            for(int z = z_low; z <= z_high; z++){
                int idx = ((x - x_low) * yz_vals) + ((y - y_low) * z_vals) + (z - z_low);
                PictureData[z * xy_dim + y * x_dim + x] += coeff * Values[idx];

                //double pixel = coeff * Values[idx];
                //double* pixel = static_cast<double*>(imageData->GetScalarPointer(x, y, z));
                //pixel[0] += coeff * Values[idx];
            }
        }
    }
}