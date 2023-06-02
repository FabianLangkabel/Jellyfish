#include "VisualizationTDPlotObjectDocker.h"
#include "plot_tdci_new_window.h"

VisualizationTDPlotObjectDocker::VisualizationTDPlotObjectDocker(
    std::shared_ptr <VisualizationTDCalculate> Calculator,
    Plot_TDCI_New_Window* TDCI_Mainwindow,
    QWidget *parent
) : QDockWidget(parent)
{
    this->Calculator = Calculator;
    this->TDCI_Mainwindow = TDCI_Mainwindow;

    DockerWidget = new QWidget();
    QVBoxLayout* MainLayout = new QVBoxLayout;
    MainLayout->setAlignment(Qt::AlignTop);
    
    QGroupBox* TimeBox = new QGroupBox("Time", this);
    QVBoxLayout* TimeBoxLayout = new QVBoxLayout;
    TimeBoxLayout->addWidget(new QLabel("Select Initial State"));
    SelectInitialState = new QComboBox();
    connect(SelectInitialState, &QComboBox::currentTextChanged, this, &VisualizationTDPlotObjectDocker::InitialStateChanged);
    std::vector<int> States = Calculator->TDCICoefficients->get_states();
    for(int i = 0; i < States.size(); i++)
    {
        std::string ItemCont = "CI-State: " + std::to_string(States[i]);
        SelectInitialState->addItem(QString::fromStdString(ItemCont));
    }
    TimeBoxLayout->addWidget(SelectInitialState);

    QGridLayout* TimeSubgrid = new QGridLayout;
    TimeSubgrid->addWidget(new QLabel("Timestep"), 0, 0);
    TimeSubgrid->addWidget(new QLabel("Time"), 0, 1);
    TimestepLineEdit = new QLineEdit();
    TimeLineEdit = new QLineEdit();
    TimestepLineEdit->setText(QString::number(Timestep));
    TimestepLineEdit->setValidator(new QIntValidator(0, Calculator->TDCICoefficients->get_times().size() - 1));
    TimeLineEdit->setText(QString::number(Time));
    TimeLineEdit->setEnabled(false);
    TimeLineEdit->setReadOnly(true);
    connect(TimestepLineEdit, &QLineEdit::textChanged, this, &VisualizationTDPlotObjectDocker::TimestepChanged);
    TimeSubgrid->addWidget(TimestepLineEdit, 1, 0);
    TimeSubgrid->addWidget(TimeLineEdit, 1, 1);
    TimeBoxLayout->addLayout(TimeSubgrid);

    TimeSlider = new QSlider(Qt::Horizontal);
	TimeSlider->setMinimum(0);
	TimeSlider->setMaximum(Calculator->TDCICoefficients->get_times().size() - 1);
	TimeSlider->setSingleStep(1);
	TimeSlider->setSliderPosition(0);
    connect(TimeSlider, &QSlider::valueChanged, this, &VisualizationTDPlotObjectDocker::TimeSliderChanged);
    TimeBoxLayout->addWidget(TimeSlider);
    PlotTimeStepButton = new QPushButton("Plot selected Frame");
    TimeBoxLayout->addWidget(PlotTimeStepButton);
    connect(PlotTimeStepButton, &QPushButton::pressed, this, &VisualizationTDPlotObjectDocker::plot);

    QGridLayout* FrameSubgrid = new QGridLayout;
    FrameSubgrid->addWidget(new QLabel("Maximum Framerate"), 0, 0);
    FrameSubgrid->addWidget(new QLabel("Skipped Frames"), 0, 1);
    FrameRateLineEdit = new QLineEdit();
    SkippedFramesLineEdit = new QLineEdit();
    connect(FrameRateLineEdit, &QLineEdit::textChanged, this, &VisualizationTDPlotObjectDocker::FrameSettingsChanged);
    connect(SkippedFramesLineEdit, &QLineEdit::textChanged, this, &VisualizationTDPlotObjectDocker::FrameSettingsChanged);
    FrameRateLineEdit->setText(QString::number(FrameRate));
    SkippedFramesLineEdit->setText(QString::number(SkippedFrames));
    FrameSubgrid->addWidget(FrameRateLineEdit, 1, 0);
    FrameSubgrid->addWidget(SkippedFramesLineEdit, 1, 1);
    TimeBoxLayout->addLayout(FrameSubgrid);

    SelectTimeLabel = new QComboBox();
    SelectTimeLabel->addItem(tr("Show Time in au"));
    SelectTimeLabel->addItem(tr("Show Time in fs"));
    TimeBoxLayout->addWidget(SelectTimeLabel);

    PlayButton = new QPushButton("Play");
    connect(PlayButton, &QPushButton::pressed, this, &VisualizationTDPlotObjectDocker::SwitchPlaying);
    TimeBoxLayout->addWidget(PlayButton);
    TimeBox->setLayout(TimeBoxLayout);
    MainLayout->addWidget(TimeBox);

    QGroupBox* ObjectBox = new QGroupBox("Select Plot Object", this);
    QVBoxLayout* ObjectBoxLayout = new QVBoxLayout;

    SelectPlotObject = new QComboBox();
    ObjectBoxLayout->addWidget(SelectPlotObject);


    SelectPlotObject->addItem(tr("Density"));
    PlotDensityWidget = new VisualizationTDPlotDensityWidget();
    ObjectBoxLayout->addWidget(PlotDensityWidget);

    SelectPlotObject->addItem(tr("Density Difference"));
    PlotDensityDifferenceWidget = new VisualizationTDPlotDensityDifferenceWidget(Calculator->ciresults);
    ObjectBoxLayout->addWidget(PlotDensityDifferenceWidget);

    SelectPlotObject->addItem(tr("Density Gradient"));
    PlotDensityGradientWidget = new VisualizationTDPlotDensityGradientWidget();
    ObjectBoxLayout->addWidget(PlotDensityGradientWidget);

    SelectPlotObject->addItem(tr("NTO Density"));
    PlotNTODensityWidget = new VisualizationTDPlotNTODensityWidget();
    ObjectBoxLayout->addWidget(PlotNTODensityWidget);


    connect(SelectPlotObject, &QComboBox::currentTextChanged, this, &VisualizationTDPlotObjectDocker::PlotObjectChanged);
    PlotObjectChanged();


    ObjectBox->setLayout(ObjectBoxLayout);
    MainLayout->addWidget(ObjectBox);
    DockerWidget->setLayout(MainLayout);
    this->setWidget(DockerWidget);
}

VisualizationTDPlotObjectDocker::~VisualizationTDPlotObjectDocker()
{

}

void VisualizationTDPlotObjectDocker::InitialStateChanged()
{
    InitialStateIdx = SelectInitialState->currentIndex();
}

void VisualizationTDPlotObjectDocker::TimestepChanged()
{
    Timestep = TimestepLineEdit->text().toInt();
    if(Timestep > Calculator->TDCICoefficients->get_times().size() - 1)
    {
        Timestep = Calculator->TDCICoefficients->get_times().size() - 1;
        TimestepLineEdit->setText(QString::number(Timestep));
    }
    TimeSlider->blockSignals(true);
    TimeSlider->setSliderPosition(Timestep);
    Time = Calculator->TDCICoefficients->get_times()[Timestep];
    TimeLineEdit->setText(QString::number(Time));
    TimeSlider->blockSignals(false);
}

void VisualizationTDPlotObjectDocker::TimeSliderChanged()
{
    Timestep = TimeSlider->sliderPosition();
    TimestepLineEdit->blockSignals(true);
    TimestepLineEdit->setText(QString::number(Timestep));
    Time = Calculator->TDCICoefficients->get_times()[Timestep];
    TimeLineEdit->setText(QString::number(Time));
    TimestepLineEdit->blockSignals(false);
}

void VisualizationTDPlotObjectDocker::ChangeTimeStep(int newTimestep)
{
    Timestep = newTimestep;
    TimeSlider->blockSignals(true);
    TimestepLineEdit->blockSignals(true);
    TimeSlider->setSliderPosition(Timestep);
    TimestepLineEdit->setText(QString::number(Timestep));
    Time = Calculator->TDCICoefficients->get_times()[Timestep];
    TimeLineEdit->setText(QString::number(Time));
    TimeSlider->blockSignals(false);
    TimestepLineEdit->blockSignals(false);
}

void VisualizationTDPlotObjectDocker::FrameSettingsChanged()
{
    FrameRate = FrameRateLineEdit->text().toDouble();
    SkippedFrames = SkippedFramesLineEdit->text().toInt();
}

void VisualizationTDPlotObjectDocker::PlotObjectChanged()
{
    PlotDensityWidget->setVisible(false); 
    PlotDensityDifferenceWidget->setVisible(false); 
    PlotDensityGradientWidget->setVisible(false); 
    PlotNTODensityWidget->setVisible(false); 


    //Enable Selected
    if(SelectPlotObject->currentText() == "Density"){PlotDensityWidget->setVisible(true);}
    else if(SelectPlotObject->currentText() == "Density Difference"){PlotDensityDifferenceWidget->setVisible(true);}
    else if(SelectPlotObject->currentText() == "Density Gradient"){PlotDensityGradientWidget->setVisible(true);}
    else if(SelectPlotObject->currentText() == "NTO Density"){PlotNTODensityWidget->setVisible(true);}
}

void VisualizationTDPlotObjectDocker::plot()
{
    if(SelectPlotObject->count() > 0)
    {
        if(SelectTimeLabel->currentIndex() == 0){
            std::string time = std::to_string(Calculator->TDCICoefficients->get_times()[Timestep]);
            TDCI_Mainwindow->RenderWidget->Label = time.substr(0, time.size()-3) + " au";
        }
        else if (SelectTimeLabel->currentIndex() == 1){ 
            std::string time = std::to_string((double)Calculator->TDCICoefficients->get_times()[Timestep] * 0.024188);
            TDCI_Mainwindow->RenderWidget->Label = time.substr(0, time.size()-3) + " fs";
        }

        if(SelectPlotObject->currentText() == "Density"){Calculator->PlotDensity(Timestep);}
        else if(SelectPlotObject->currentText() == "Density Difference"){Calculator->PlotDensityDifference(Timestep, PlotDensityDifferenceWidget->ReferenceCIState);}
        else if(SelectPlotObject->currentText() == "Density Gradient"){Calculator->PlotDensityGradient(Timestep);}
        else if(SelectPlotObject->currentText() == "NTO Density"){Calculator->PlotNTODensity(Timestep, PlotNTODensityWidget->SelectedDensity);}
    }
}

void VisualizationTDPlotObjectDocker::SwitchPlaying()
{
    if(IsPlaying)
    {
        //Pause Video
        vtkRenderWindowInteractor* iren = static_cast<vtkRenderWindowInteractor*>(TDCI_Mainwindow->RenderWidget->screen->renderWindow()->GetInteractor());
		iren->DestroyTimer();
        PlayButton->setText("Play");
        IsPlaying = false;
    }
    else
    {
        //Start Video
        PlayButton->setText("Pause");
        StartPlay();
        IsPlaying = true;
    }
}

void VisualizationTDPlotObjectDocker::StartPlay()
{
	auto renderWindowInteractor = TDCI_Mainwindow->RenderWidget->ren->GetRenderWindow()->GetInteractor();
	auto t0 = std::chrono::steady_clock::now().time_since_epoch();
	Propagate();
	auto t1 = std::chrono::steady_clock::now().time_since_epoch();
	auto tt = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
	VideoFrameRendering = false;
	renderWindowInteractor->Initialize();
	int dt = 1000 / FrameRate;
	renderWindowInteractor->CreateRepeatingTimer(int(tt)+dt);
	//vtkSmartPointer<vtkCallbackCommand> propagateCallback = vtkSmartPointer<vtkCallbackCommand>::New();
	if (VideoFirstPlay)
	{
		renderWindowInteractor->AddObserver(vtkCommand::TimerEvent, this, &VisualizationTDPlotObjectDocker::Propagate);
		VideoFirstPlay = false;
	}
}

void VisualizationTDPlotObjectDocker::Propagate()
{
    if (!VideoFrameRendering)
	{
        int EndStep = Calculator->TDCICoefficients->get_times().size() - 1;
		VideoFrameRendering = true;
		if (Timestep + 1 + SkippedFrames <= EndStep)
		{
            ChangeTimeStep(Timestep + 1 + SkippedFrames);
            //Plot
            plot();
        }
		else
		{
			vtkRenderWindowInteractor* iren = static_cast<vtkRenderWindowInteractor*>(TDCI_Mainwindow->RenderWidget->screen->renderWindow()->GetInteractor());
			iren->DestroyTimer();
            ChangeTimeStep(EndStep);
			SwitchPlaying();
		}
		VideoFrameRendering = false;
    }
}