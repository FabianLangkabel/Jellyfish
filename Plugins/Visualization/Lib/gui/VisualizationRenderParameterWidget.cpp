#include "VisualizationRenderParameterWidget.h"

VisualizationRenderParameterWidget::VisualizationRenderParameterWidget(bool PointchargesAvailable, QWidget *parent) : QGroupBox(parent)
{
    this->setTitle("Render Settings");
    QVBoxLayout* MainLayout = new QVBoxLayout;


	coordinatebox = new QCheckBox(tr("&Coordinate System"));
	coordinatebox->setChecked(true);
	moleculebox = new QCheckBox(tr("&Molecular structure"));
	renderbondsbox = new QCheckBox(tr("&Draw Bonds"));
	if (!PointchargesAvailable)
	{
		moleculebox->setChecked(false);
        renderbondsbox->setChecked(false);
		moleculebox->setDisabled(true);
		renderbondsbox->setDisabled(true);
	}
    connect(coordinatebox, &QCheckBox::clicked, this, &VisualizationRenderParameterWidget::updateValues);
    connect(moleculebox, &QCheckBox::clicked, this, &VisualizationRenderParameterWidget::updateValues);
    connect(renderbondsbox, &QCheckBox::clicked, this, &VisualizationRenderParameterWidget::updateValues);

    MainLayout->addWidget(coordinatebox);
    MainLayout->addWidget(moleculebox);
    MainLayout->addWidget(renderbondsbox);

	MainLayout->addWidget(new QLabel("Number of Isosurfaces"));
	isodurfacenumberselctor = new QComboBox;
	isodurfacenumberselctor->addItem("1");
	isodurfacenumberselctor->addItem("2");
	isodurfacenumberselctor->addItem("3");
	MainLayout->addWidget(isodurfacenumberselctor);
	connect(isodurfacenumberselctor, &QComboBox::currentTextChanged, this, &VisualizationRenderParameterWidget::updateValues);


	//isosurfacevalue
	isovalue = new QLineEdit;
	isovalue->setText(QString::number(isosurfacethreshold));
	MainLayout->addWidget(new QLabel("Isosurface value"));
	MainLayout->addWidget(isovalue);
    connect(isovalue, &QLineEdit::textChanged, this, &VisualizationRenderParameterWidget::updateValues);

	// choose color
	color1box = new QPushButton(tr("Primary Color"));
	color2box = new QPushButton(tr("Secondary Color"));
	QIcon* coloricon1 = new QIcon;
	QPixmap* colormap1 = new QPixmap(100, 100);
	colormap1->fill(QColor(QColor(color1[0] * 255, color1[1] * 255, color1[2] * 255, 255)));
	coloricon1->addPixmap(*colormap1);
	color1box->setIcon(*coloricon1);
	QIcon* coloricon2 = new QIcon;
	QPixmap* colormap2 = new QPixmap(100, 100);
	colormap2->fill(QColor(QColor(color2[0] * 255, color2[1] * 255, color2[2] * 255)));
	coloricon2->addPixmap(*colormap2);
	color2box->setIcon(*coloricon2);
	connect(color1box, &QPushButton::clicked, this, &VisualizationRenderParameterWidget::chooseColor1);
	connect(color2box, &QPushButton::clicked, this, &VisualizationRenderParameterWidget::chooseColor2);
	color1window = new QColorDialog;
	color2window = new QColorDialog;
    MainLayout->addWidget(color1box);
    MainLayout->addWidget(color2box);


    //Update Button
    UpdateButton = new QPushButton(tr("Update Render Settings"));
	//connect(updateButton, &QPushButton::clicked, this, &VisualizationRenderParameterWidget::updateRender);
    MainLayout->addWidget(UpdateButton);


    this->setLayout(MainLayout);
}

VisualizationRenderParameterWidget::~VisualizationRenderParameterWidget()
{

}

void VisualizationRenderParameterWidget::updateValues()
{
    drawcoordsystem = coordinatebox->isChecked();
    drawmolecule = moleculebox->isChecked();
    draw_bonds = renderbondsbox->isChecked();
    isosurfacethreshold = isovalue->text().toDouble();
	isosurfacenumber = isodurfacenumberselctor->currentText().toInt();
}

void VisualizationRenderParameterWidget::chooseColor1()
{
	QColor Qcolor1 = color1window->getColor();
	color1[0] = double(Qcolor1.redF());
	color1[1] = double(Qcolor1.greenF());
	color1[2] = double(Qcolor1.blueF());
	//set icon
	QIcon* coloricon1 = new QIcon;
	QPixmap* colormap1 = new QPixmap(100, 100);
	colormap1->fill(QColor(QColor(Qcolor1)));
	coloricon1->addPixmap(*colormap1);
	color1box->setIcon(*coloricon1);

}

void VisualizationRenderParameterWidget::chooseColor2()
{
	QColor Qcolor2 = color1window->getColor();
	color2[0] = double(Qcolor2.redF());
	color2[1] = double(Qcolor2.greenF());
	color2[2] = double(Qcolor2.blueF());
	//set icon
	QIcon* coloricon2 = new QIcon;
	QPixmap* colormap2 = new QPixmap(100, 100);
	colormap2->fill(QColor(QColor(Qcolor2)));
	coloricon2->addPixmap(*colormap2);
	color2box->setIcon(*coloricon2);
}