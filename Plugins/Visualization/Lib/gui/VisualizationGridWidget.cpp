#include "VisualizationGridWidget.h"
#include <iostream>

VisualizationGridWidget::VisualizationGridWidget(QWidget *parent) : QGroupBox(parent)
{
	UsedGrid = std::make_shared<Grid>();
    UpdateGrid();
    this->setTitle("Grid");

    QVBoxLayout* GridLayout = new QVBoxLayout;

	QGridLayout* dimensionsgrid = new QGridLayout();
	dimensionsgrid->addWidget(new QLabel("x-Min"), 0, 0);
	dimensionsgrid->addWidget(new QLabel("x-Max"), 0, 1);
	xStart = new QLineEdit;
	xStart->setText(QString::number(UsedGrid->realxmin));
	xStart->connect(xStart, &QLineEdit::textChanged, this, &VisualizationGridWidget::updateDimensions);
	dimensionsgrid->addWidget(xStart, 1,0);
	xEnd = new QLineEdit;
	xEnd->setText(QString::number(UsedGrid->realxmax));
	xEnd->connect(xEnd, &QLineEdit::textChanged, this, &VisualizationGridWidget::updateDimensions);
	dimensionsgrid->addWidget(xEnd, 1, 1);
	dimensionsgrid->addWidget(new QLabel("y-Min"), 2, 0);
	dimensionsgrid->addWidget(new QLabel("y-Max"), 2, 1);
	yStart = new QLineEdit;
	yStart->setText(QString::number(UsedGrid->realymin));
	yStart->connect(yStart, &QLineEdit::textChanged, this, &VisualizationGridWidget::updateDimensions);
	dimensionsgrid->addWidget(yStart, 3, 0);
	yEnd = new QLineEdit;
	yEnd->setText(QString::number(UsedGrid->realymax));
	yEnd->connect(yEnd, &QLineEdit::textChanged, this, &VisualizationGridWidget::updateDimensions);
	dimensionsgrid->addWidget(yEnd, 3, 1);
	dimensionsgrid->addWidget(new QLabel("z-Min"), 4, 0);
	dimensionsgrid->addWidget(new QLabel("z-Max"), 4, 1);
	zStart = new QLineEdit;
	zStart->setText(QString::number(UsedGrid->realzmin));
	zStart->connect(zStart, &QLineEdit::textChanged, this, &VisualizationGridWidget::updateDimensions);
	dimensionsgrid->addWidget(zStart, 5, 0);
	zEnd = new QLineEdit;
	zEnd->setText(QString::number(UsedGrid->realzmax));
	zEnd->connect(zEnd, &QLineEdit::textChanged, this, &VisualizationGridWidget::updateDimensions);
	dimensionsgrid->addWidget(zEnd, 5, 1);
    GridLayout->addLayout(dimensionsgrid);

    QVBoxLayout* ResolutionLayout = new QVBoxLayout;
    ResolutionLayout->addWidget(new QLabel("Resolution"));
    resolutionstext = new QLineEdit;
	resolutionstext->setText(QString::number(UsedGrid->resolution));
    resolutionstext->connect(resolutionstext, &QLineEdit::textChanged, this, &VisualizationGridWidget::updateDimensions);
    ResolutionLayout->addWidget(resolutionstext);
    GridLayout->addLayout(ResolutionLayout);


	QGridLayout* resolutionsgrid = new QGridLayout();
	resolutionsgrid->addWidget(new QLabel("x-Dim"), 0, 0);
	resolutionsgrid->addWidget(new QLabel("y-Dim"), 0, 1);
	resolutionsgrid->addWidget(new QLabel("z-Dim"), 0, 2);
	xResolution = new QLineEdit;
	xResolution->setText(QString::number(UsedGrid->xdim));
	xResolution->setDisabled(true);
	yResolution = new QLineEdit;
	yResolution->setText(QString::number(UsedGrid->ydim));
	yResolution->setDisabled(true);
	zResolution = new QLineEdit;
	zResolution->setText(QString::number(UsedGrid->zdim));
	zResolution->setDisabled(true);
	resolutionsgrid->addWidget(xResolution, 1, 0);
	resolutionsgrid->addWidget(yResolution, 1, 1);
	resolutionsgrid->addWidget(zResolution, 1, 2);
    GridLayout->addLayout(resolutionsgrid);

	QVBoxLayout* TruncationLayout = new QVBoxLayout;
    TruncationLayout->addWidget(new QLabel("Basisfunction-Truncationradius in FWHM"));
    truncationtext = new QLineEdit;
	truncationtext->setText(QString::number(UsedGrid->truncationradius));
    truncationtext->connect(truncationtext, &QLineEdit::textChanged, this, &VisualizationGridWidget::updateDimensions);
    TruncationLayout->addWidget(truncationtext);
    GridLayout->addLayout(TruncationLayout);

    this->setLayout(GridLayout);
}

VisualizationGridWidget::~VisualizationGridWidget()
{
    if(UsedGrid->XGrid != nullptr) { delete[] UsedGrid->XGrid; }
    if(UsedGrid->YGrid != nullptr) { delete[] UsedGrid->YGrid; }
    if(UsedGrid->ZGrid != nullptr) { delete[] UsedGrid->ZGrid; }
}

void VisualizationGridWidget::UpdateGrid()
{
    UsedGrid->ActiveGrid[0] = UsedGrid->resolution;
	UsedGrid->ActiveGrid[1] = UsedGrid->realxmin;
	UsedGrid->ActiveGrid[2] = UsedGrid->realxmax;
	UsedGrid->ActiveGrid[3] = UsedGrid->realymin;
	UsedGrid->ActiveGrid[4] = UsedGrid->realymax;
	UsedGrid->ActiveGrid[5] = UsedGrid->realzmin;
	UsedGrid->ActiveGrid[6] = UsedGrid->realzmax;
	UsedGrid->ActiveGrid[7] = UsedGrid->truncationradius;

	UsedGrid->xdim = int((UsedGrid->realxmax - UsedGrid->realxmin) * UsedGrid->resolution) + 1;
	UsedGrid->ydim = int((UsedGrid->realymax - UsedGrid->realymin) * UsedGrid->resolution) + 1;
	UsedGrid->zdim = int((UsedGrid->realzmax - UsedGrid->realzmin) * UsedGrid->resolution) + 1;

    if(UsedGrid->XGrid != nullptr) { delete[] UsedGrid->XGrid; }
    if(UsedGrid->YGrid != nullptr) { delete[] UsedGrid->YGrid; }
    if(UsedGrid->ZGrid != nullptr) { delete[] UsedGrid->ZGrid; }

    UsedGrid->XGrid = new double[UsedGrid->xdim]; //1D grid
	UsedGrid->dx = (UsedGrid->realxmax - UsedGrid->realxmin) / (UsedGrid->xdim - 1); //gridspacing
	for (int i = 0; i < UsedGrid->xdim; i++)
	{
		UsedGrid->XGrid[i] = UsedGrid->realxmin + i * UsedGrid->dx;
	}
	UsedGrid->YGrid = new double[UsedGrid->ydim]; //1D grid
	UsedGrid->dy = (UsedGrid->realymax - UsedGrid->realymin) / (UsedGrid->ydim - 1); //gridspacing
	for (int i = 0; i < UsedGrid->ydim; i++)
	{
		UsedGrid->YGrid[i] = UsedGrid->realymin + i * UsedGrid->dy;
	}
	UsedGrid->ZGrid = new double[UsedGrid->zdim]; // 1D grid
	UsedGrid->dz = (UsedGrid->realzmax - UsedGrid->realzmin) / (UsedGrid->zdim - 1); //gridspacing
	for (int i = 0; i < UsedGrid->zdim; i++)
	{
		UsedGrid->ZGrid[i] = UsedGrid->realzmin + i * UsedGrid->dz;
	}
}

void VisualizationGridWidget::updateDimensions()
{
    UsedGrid->resolution = resolutionstext->text().toDouble();
	UsedGrid->realxmin = xStart->text().toDouble();
	UsedGrid->realxmax = xEnd->text().toDouble();
	UsedGrid->realymin = yStart->text().toDouble();
	UsedGrid->realymax = yEnd->text().toDouble();
	UsedGrid->realzmin = zStart->text().toDouble();
	UsedGrid->realzmax = zEnd->text().toDouble();
	UsedGrid->xdim = int((UsedGrid->realxmax - UsedGrid->realxmin) * UsedGrid->resolution) + 1;
	UsedGrid->ydim = int((UsedGrid->realymax - UsedGrid->realymin) * UsedGrid->resolution) + 1;
	UsedGrid->zdim = int((UsedGrid->realzmax - UsedGrid->realzmin) * UsedGrid->resolution) + 1;
	xResolution->clear();
	xResolution->setText(QString::number(UsedGrid->xdim));
	yResolution->clear();
	yResolution->setText(QString::number(UsedGrid->ydim));
	zResolution->clear();
	zResolution->setText(QString::number(UsedGrid->zdim));
	UsedGrid->truncationradius = truncationtext->text().toDouble();
}

bool VisualizationGridWidget::UpdateGridIfNeeded()
{
	if(    
		UsedGrid->ActiveGrid[0] != UsedGrid->resolution ||
		UsedGrid->ActiveGrid[1] != UsedGrid->realxmin ||
		UsedGrid->ActiveGrid[2] != UsedGrid->realxmax ||
		UsedGrid->ActiveGrid[3] != UsedGrid->realymin ||
		UsedGrid->ActiveGrid[4] != UsedGrid->realymax ||
		UsedGrid->ActiveGrid[5] != UsedGrid->realzmin ||
		UsedGrid->ActiveGrid[6] != UsedGrid->realzmax ||
		UsedGrid->ActiveGrid[7] != UsedGrid->truncationradius
	) { UpdateGrid(); return true; }
	else {return false; }
}