#include "plot_tdci_new_window.h"


Plot_TDCI_New_Window::Plot_TDCI_New_Window(
	std::shared_ptr <QC::CIResults> ciresults,
	std::shared_ptr <Eigen::MatrixXd> hfcmatrix,
	std::shared_ptr <QC::Gaussian_Basisset> basissetdata,
	std::shared_ptr <std::vector<QC::Pointcharge>> pointcharges,
	std::shared_ptr <QC::TDCI_WF> TDCICoefficients
) : QMainWindow()
{
	LogWidget = new VisualizationLogWidget(this);
	QDockWidget* LogDocker = new QDockWidget(this);
	LogDocker->setWidget(LogWidget);
	LogDocker->setAllowedAreas(Qt::BottomDockWidgetArea);
	addDockWidget(Qt::BottomDockWidgetArea, LogDocker);

	Calculator = std::make_shared<VisualizationTDCalculate>(ciresults, hfcmatrix, basissetdata, pointcharges, TDCICoefficients, this);

	TDPlotObjectDocker = new VisualizationTDPlotObjectDocker(Calculator, this, this);
	TDPlotObjectDocker->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	//TIDPlotObjectDocker->setMinimumWidth(250);
    addDockWidget(Qt::LeftDockWidgetArea, TDPlotObjectDocker);

	GridWidget = new VisualizationGridWidget(this);
    RenderParameterWidget = new VisualizationRenderParameterWidget(pointcharges != nullptr, this);
	ExportPicture = new VisualizationExportPicture(this);
	ExportVideo = new VisualizationExportVideo(this);
	QDockWidget* OptionsDocker = new QDockWidget(this);
	QWidget* OptionsWidget = new QWidget();
	QVBoxLayout* OptionsWidgetLayout = new QVBoxLayout;
	OptionsWidgetLayout->setAlignment(Qt::AlignTop);
	OptionsWidgetLayout->addWidget(RenderParameterWidget);
	OptionsWidgetLayout->addWidget(GridWidget);
	OptionsWidgetLayout->addWidget(ExportPicture);
	OptionsWidgetLayout->addWidget(ExportVideo);
	OptionsWidget->setLayout(OptionsWidgetLayout);
	OptionsDocker->setWidget(OptionsWidget);
	OptionsDocker->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	addDockWidget(Qt::RightDockWidgetArea, OptionsDocker);

	RenderWidget = new VisualizationRenderWidget(RenderParameterWidget, GridWidget, pointcharges, LogWidget, this);
	setCentralWidget(RenderWidget);
	connect(RenderParameterWidget->UpdateButton, &QPushButton::clicked, RenderWidget, &VisualizationRenderWidget::UpdateScreen);
	ExportPicture->RenderWidget = RenderWidget;
	ExportVideo->RenderWidget = RenderWidget;
	ExportVideo->PlotObjectDocker = TDPlotObjectDocker;
	ExportVideo->UpdateEndTimeStep(TDCICoefficients->get_times().size() - 1);

	/*
	// Menu Bar Top
	// saving the Scene as image
	saveAct = new QAction(QIcon(":/icons/save_1.svg"), tr("&Save"), this);
	saveAct->setShortcuts(QKeySequence::Save);
	saveAct->setStatusTip(tr("Export"));
	connect(saveAct, &QAction::triggered, this, &Plot_CI_New_Window::saveFile);
	// about
	aboutAct = new QAction(QIcon(":/icons/info_cr.svg"), tr("&About"), this);
	aboutAct->setStatusTip(tr("Show the application's About box."));
	connect(aboutAct, &QAction::triggered, this, &Plot_CI_New_Window::about);

	// build Top bar
	auto fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(saveAct);
	auto helpMenu = menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(aboutAct);

	//ToolBar
	QToolBar* toolbar = addToolBar("Toolbar");
	toolbar->addAction(saveAct);
	toolbar->addAction(aboutAct);
	*/

	setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
	setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
}