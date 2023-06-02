#pragma once

#include <QtWidgets>

class VisualizationRenderParameterWidget : public QGroupBox
{
public:
    explicit VisualizationRenderParameterWidget(bool PointchargesAvailable, QWidget *parent = 0);
    ~VisualizationRenderParameterWidget();

public:
    QPushButton* UpdateButton;
    double isosurfacethreshold = 0.1;
    double color1[3] = { 1.0, 1.0, 0.0 };
    double color2[3] = { 1.0, 0.1, 1.0 };
    bool drawcoordsystem = true;
    bool drawmolecule = false;
    bool draw_bonds = false;
    int isosurfacenumber = 1;

private:
    QCheckBox* coordinatebox;
    QCheckBox* moleculebox;
    QCheckBox* renderbondsbox;
    QLineEdit* isovalue;
    QComboBox* isodurfacenumberselctor;

    // Color Widgets
    QPushButton* color1box;
    QPushButton* color2box;
    QColorDialog* color1window;
    QColorDialog* color2window;

    void updateValues();
    void chooseColor1();
    void chooseColor2();
};
