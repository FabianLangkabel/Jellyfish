#pragma once

#include <QtWidgets>
#include "VisualizationRenderWidget.h"


class VisualizationExportPicture : public QGroupBox
{
public:
    explicit VisualizationExportPicture(QWidget *parent = 0);
    ~VisualizationExportPicture();
    VisualizationRenderWidget* RenderWidget;

private:
    QLineEdit* PictureScaling;
    QPushButton* ExportButton;
    void Export();
};
