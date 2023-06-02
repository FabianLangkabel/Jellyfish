#include "VisualizationExportVideo.h"

VisualizationExportVideo::VisualizationExportVideo(QWidget *parent) : QGroupBox(parent)
{
	this->setTitle("Export Video");
	QVBoxLayout* Layout = new QVBoxLayout;

	QGridLayout* SubLayout = new QGridLayout();

	SubLayout->addWidget(new QLabel("Picture Scaling"), 0, 0);
	PictureScaling = new QLineEdit();
	PictureScaling->setValidator(new QDoubleValidator(0.1, 100, 2, this));
	PictureScaling->setText(QString::number(5));
	SubLayout->addWidget(PictureScaling, 0, 1);

	SubLayout->addWidget(new QLabel("Start Frame"), 1, 0);
	StartFrame = new QLineEdit();
	SubLayout->addWidget(StartFrame, 1, 1);

	SubLayout->addWidget(new QLabel("End Frame"), 2, 0);
	EndFrame = new QLineEdit();
	SubLayout->addWidget(EndFrame, 2, 1);

	SubLayout->addWidget(new QLabel("Skipped Frames"), 3, 0);
	SkippedFrames = new QLineEdit();
	SkippedFrames->setValidator(new QIntValidator(0, 10000000, this));
	SkippedFrames->setText(QString::number(0));
	SubLayout->addWidget(SkippedFrames, 3, 1);


	Layout->addLayout(SubLayout);
	ExportButton = new QPushButton("Export Video");
	Layout->addWidget(ExportButton);
	connect(ExportButton, &QPushButton::pressed, this, &VisualizationExportVideo::Export);

	this->setLayout(Layout);
}

VisualizationExportVideo::~VisualizationExportVideo()
{

}

void VisualizationExportVideo::Export()
{
	double picturescale = PictureScaling->text().toDouble();
	int Timestep = StartFrame->text().toInt();
	int EndTimestep = EndFrame->text().toInt();
	int SkippedFramesVal = SkippedFrames->text().toInt();

	QString DirName = QFileDialog::getExistingDirectory(nullptr, tr("Export Videoframes"), QDir::homePath());
	if (!DirName.isEmpty())
	{
		while(Timestep <= EndTimestep)
		{
			PlotObjectDocker->ChangeTimeStep(Timestep);
			PlotObjectDocker->plot();
			RenderWidget->SavePictureToFile(DirName.toStdString() + "/" + std::to_string(Timestep) + ".jpg", picturescale);
			Timestep += (1 + SkippedFramesVal);
		}
	}
}

void VisualizationExportVideo::UpdateEndTimeStep(int EndTimeStep)
{
	StartFrame->setValidator(new QIntValidator(0, EndTimeStep, this));
	StartFrame->setText(QString::number(0));
	EndFrame->setValidator(new QIntValidator(0, EndTimeStep, this));
	EndFrame->setText(QString::number(EndTimeStep));
}