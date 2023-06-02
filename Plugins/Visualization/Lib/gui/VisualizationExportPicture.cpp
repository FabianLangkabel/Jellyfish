#include "VisualizationExportPicture.h"

VisualizationExportPicture::VisualizationExportPicture(QWidget *parent) : QGroupBox(parent)
{
	this->setTitle("Export Picture");
	QVBoxLayout* Layout = new QVBoxLayout;

	QGridLayout* SubLayout = new QGridLayout();
	SubLayout->addWidget(new QLabel("Picture Scaling"), 0, 0);
	PictureScaling = new QLineEdit();
	PictureScaling->setValidator(new QDoubleValidator(0.1, 100, 2, this));
	PictureScaling->setText(QString::number(5));
	SubLayout->addWidget(PictureScaling, 0, 1);
	Layout->addLayout(SubLayout);
	ExportButton = new QPushButton("Export Picture");
	Layout->addWidget(ExportButton);
	connect(ExportButton, &QPushButton::pressed, this, &VisualizationExportPicture::Export);

	this->setLayout(Layout);
}

VisualizationExportPicture::~VisualizationExportPicture()
{

}

void VisualizationExportPicture::Export()
{
	double picturescale = PictureScaling->text().toDouble();

	QString FileName = QFileDialog::getSaveFileName(nullptr, tr("Export Picture"), QDir::homePath(), tr("Picture (*.jpg, *.png)"));
	if (!FileName.isEmpty())
	{
		RenderWidget->SavePictureToFile(FileName.toStdString(), picturescale);
	}
}