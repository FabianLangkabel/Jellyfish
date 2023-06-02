#pragma once

#include <QtWidgets>
#include "Grid.h"


class VisualizationGridWidget : public QGroupBox
{
public:
    explicit VisualizationGridWidget(QWidget *parent = 0);
    ~VisualizationGridWidget();

public:
    std::shared_ptr<Grid> UsedGrid;
    bool UpdateGridIfNeeded();

private:
    QLineEdit* xStart;
    QLineEdit* xEnd;
    QLineEdit* yStart;
    QLineEdit* yEnd;
    QLineEdit* zStart;
    QLineEdit* zEnd;
    QLineEdit* resolutionstext;
    QLineEdit* xResolution;
    QLineEdit* yResolution;
    QLineEdit* zResolution;
    QLineEdit* truncationtext;

    void UpdateGrid();
    void updateDimensions();
};
