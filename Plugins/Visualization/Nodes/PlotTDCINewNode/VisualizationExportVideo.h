#pragma once

#include <QtWidgets>
#include "../../Lib/gui/VisualizationRenderWidget.h"
#include "VisualizationTDPlotObjectDocker.h"



class VisualizationExportVideo : public QGroupBox
{
public:
    explicit VisualizationExportVideo(QWidget *parent = 0);
    ~VisualizationExportVideo();
    VisualizationRenderWidget* RenderWidget;
    VisualizationTDPlotObjectDocker* PlotObjectDocker;

private:
    QLineEdit* PictureScaling;
    QLineEdit* StartFrame;
    QLineEdit* EndFrame;
    QLineEdit* SkippedFrames;
    QPushButton* ExportButton;
    void Export();

public:
    void UpdateEndTimeStep(int EndTimeStep);
};
