#pragma once

#include <QtWidgets>
#include <QMainWindow>
#include <QWidget>
#include <QtWidgets>

#include "VisualizationTIDCalculate.h"
#include "VisualizationTIDPlotObjectDocker.h"
#include "../../Lib/gui/VisualizationGridWidget.h"
#include "../../Lib/gui/VisualizationLogWidget.h"
#include "../../Lib/gui/VisualizationRenderParameterWidget.h"
#include "../../Lib/gui/VisualizationRenderWidget.h"
#include "../../Lib/gui/VisualizationExportPicture.h"

#include "../../Lib/header/gaussian_basisset.h"
#include "../../Lib/header/pointcharge.h"
#include "../../Lib/header/CIResults.h"


class Plot_CI_New_Window : public QMainWindow
{
public:
    explicit Plot_CI_New_Window(
        std::shared_ptr <QC::CIResults> ciresults,
        std::shared_ptr <Eigen::MatrixXd> hfcmatrix,
        std::shared_ptr <QC::Gaussian_Basisset> basissetdata,
        std::shared_ptr <std::vector<QC::Pointcharge>> pointcharges
    );

private:
    std::shared_ptr<VisualizationTIDCalculate> Calculator;

public:
    VisualizationGridWidget* GridWidget;
    VisualizationLogWidget* LogWidget;
    VisualizationRenderParameterWidget* RenderParameterWidget;
    VisualizationRenderWidget* RenderWidget;
    VisualizationTIDPlotObjectDocker* TIDPlotObjectDocker;
    VisualizationExportPicture* ExportPicture;
/*
private:
    //Menu Bar
    QAction* saveAct;
    QAction* aboutAct;
    void saveFile();
    void about();
*/
};
