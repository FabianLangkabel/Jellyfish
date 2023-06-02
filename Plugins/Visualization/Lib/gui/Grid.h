#pragma once

class Grid
{
public:
    Grid(){};
    ~Grid()
    {
        delete XGrid;
        delete YGrid;
        delete ZGrid;
    }
    int xdim; 
    int ydim;
    int zdim;
    double* XGrid = nullptr;
    double* YGrid = nullptr;
    double* ZGrid = nullptr;
    double resolution = 4;
    double realxmin = -7.0;
    double realxmax = 7.0;
    double realymin = -7.0;
    double realymax = 7.0;
    double realzmin = -7.0;
    double realzmax = 7.0;
    double ActiveGrid[8];
    double dx;
    double dy;
    double dz;
    double truncationradius = 3;
};