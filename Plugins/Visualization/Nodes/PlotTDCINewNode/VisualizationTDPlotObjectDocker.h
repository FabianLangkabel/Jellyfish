#pragma once

#include <QtWidgets>
#include "VisualizationTDCalculate.h"
#include "VisualizationTDPlotObjects.h"
#include "../../Lib/header/gaussian_basisset.h"
#include "../../Lib/header/pointcharge.h"

class Plot_TDCI_New_Window;
class VisualizationTDPlotObjectDocker : public QDockWidget
{
public:
    explicit VisualizationTDPlotObjectDocker(	
        std::shared_ptr <VisualizationTDCalculate> Calculator,
        Plot_TDCI_New_Window* TDCI_Mainwindow,
        QWidget *parent = 0);
    ~VisualizationTDPlotObjectDocker();

private:
    std::shared_ptr<VisualizationTDCalculate> Calculator;
    Plot_TDCI_New_Window* TDCI_Mainwindow;

public:
    void ChangeTimeStep(int newTimestep);
    void plot();

private:
    QComboBox* SelectInitialState;
    int InitialStateIdx = 0;
    void InitialStateChanged();
    QLineEdit* TimestepLineEdit;
    int Timestep = 0;
    QLineEdit* TimeLineEdit;
    double Time = 0;
    QSlider* TimeSlider;
    QPushButton* PlotTimeStepButton;
    QComboBox* SelectTimeLabel;
    void TimestepChanged();
    void TimeSliderChanged();

    QLineEdit* FrameRateLineEdit;
    double FrameRate = 30;
    QLineEdit* SkippedFramesLineEdit;
    int SkippedFrames = 0;
    void FrameSettingsChanged();

    QPushButton* PlayButton;
    bool IsPlaying = false;
    bool VideoFirstPlay = true;
    bool VideoFrameRendering = false;
    void SwitchPlaying();
    void StartPlay();
    void Propagate();

    QWidget* DockerWidget;
    QComboBox* SelectPlotObject;

    VisualizationTDPlotDensityWidget* PlotDensityWidget;
    VisualizationTDPlotDensityDifferenceWidget* PlotDensityDifferenceWidget;
    VisualizationTDPlotDensityGradientWidget* PlotDensityGradientWidget;
    VisualizationTDPlotNTODensityWidget* PlotNTODensityWidget;

    void PlotObjectChanged();
};
