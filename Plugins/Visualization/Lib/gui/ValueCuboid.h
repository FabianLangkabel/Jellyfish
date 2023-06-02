#pragma once
#include <math.h>
#include <iostream>
#include <memory>
#include <thread>
#include "Grid.h"

#include <vtkSmartPointer.h>
#include "vtkImageData.h"

#include "../../Lib/header/gaussian_basisset.h"

class AOValueCuboid
{
public:
    AOValueCuboid(
        int basisfunction_number, 
        QC::Gaussian_Basisfunction basisfunction, 
        int basissettype, 
        int angular_moment,
        std::shared_ptr<Grid> MainGrid
    );
    int basisfunction_number;
    ~AOValueCuboid();
    void AddAOToImageData(double* PictureData, double coeff);
    void AddAOSquareToImageData(double* PictureData, double coeff);

private:
    int x_low, y_low, z_low, x_high, y_high, z_high;
    int x_dim, y_dim, z_dim;
    double* Values = nullptr;
};

class AOAOValueCuboid
{
    public:
    AOAOValueCuboid(
        int basisfunction_number1, 
        QC::Gaussian_Basisfunction basisfunction1,
        int angular_moment1,
        int basisfunction_number2, 
        QC::Gaussian_Basisfunction basisfunction2,
        int angular_moment2,
        int basissettype, 
        std::shared_ptr<Grid> MainGrid
    );
    bool AOsHaveOverlap;
    int basisfunction_number1;
    int basisfunction_number2;
    ~AOAOValueCuboid();
    void AddAOAOToImageData(double* PictureData, double coeff);
    void ThreadAddAOAOToImageData(double* PictureData, double coeff, int poolsize, int id);

private:
    int x_low, y_low, z_low, x_high, y_high, z_high;
    int x_dim, y_dim, z_dim;
    double* Values = nullptr;
};