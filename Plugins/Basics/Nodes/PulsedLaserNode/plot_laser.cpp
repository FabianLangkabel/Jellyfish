#include "plot_laser.h"


Laser_Analysis_Window::Laser_Analysis_Window(std::shared_ptr<QC::Laser> laser) : QMainWindow()
{
	this->laser = laser;
	
	//Rechter Docker
	QDockWidget *right_docker = new QDockWidget(tr("Plot Laser"), this);
	right_docker->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	QScrollArea* scrollArea = new QScrollArea;
	scrollArea->setFrameShape(QFrame::NoFrame);
	scrollArea->setWidgetResizable(true);
	QWidget* content = new QWidget;
	QVBoxLayout* layout = new QVBoxLayout;
	layout->setAlignment(Qt::AlignTop);
	content->setLayout(layout);
	scrollArea->setWidget(content);
	right_docker->setWidget(scrollArea);
	right_docker->setMinimumWidth(250);
	addDockWidget(Qt::RightDockWidgetArea, right_docker);

	layout->addWidget(new QLabel("Time_min"));
	time_min = new QLineEdit;
	time_min->setValidator(new QDoubleValidator(0, 100000000, 5, this));
	layout->addWidget(time_min);

	auto line = new QFrame;
	line->setFrameShape(QFrame::HLine);
	line->setFrameShadow(QFrame::Sunken);
	layout->addWidget(line);

	layout->addWidget(new QLabel("Time_max"));
	time_max = new QLineEdit;
	time_max->setValidator(new QDoubleValidator(0, 100000000, 5, this));
	layout->addWidget(time_max);

	auto line2 = new QFrame;
	line2->setFrameShape(QFrame::HLine);
	line2->setFrameShadow(QFrame::Sunken);
	layout->addWidget(line2);

	QPushButton* plot_button = new QPushButton("Plot");
	connect(plot_button, &QPushButton::clicked, this, &Laser_Analysis_Window::plot);
	layout->addWidget(plot_button);

	
	//Central
	chart = new QChart();
	chart->legend()->hide();
	chart->setTitle("Laser Pulse");

	
	QChartView* chartView = new QChartView(chart);
	chartView->setRenderHint(QPainter::Antialiasing);

	this->setCentralWidget(chartView);
	
	
}

void Laser_Analysis_Window::plot()
{
	
	chart->removeAllSeries();
	int points = 5000;
	double delta = (this->time_max->text().toDouble() - this->time_min->text().toDouble()) / points;
	std::string direction = { "xyz" };

	for (int j = 0; j < 3; j++) {
		QLineSeries* series = new QLineSeries();
		
		for (int i = 0; i < points + 1; i++)
		{
				double time = this->time_min->text().toDouble() + i * delta;
				series->append(time, this->laser->get_field_strengths(time)[j]);
		}
		chart->addSeries(series);
		series->setName(QString(direction[j]));
	}
	chart->createDefaultAxes();
	chart->legend()->setVisible(true);
}

	