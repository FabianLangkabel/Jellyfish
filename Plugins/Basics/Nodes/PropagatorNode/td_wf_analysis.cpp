#include "td_wf_analysis.h"


TDWF_Analysis_Window::TDWF_Analysis_Window(QC::TDCI_WF* wavefunction, Eigen::VectorXd ci_energys, std::weak_ptr<std::vector<Eigen::MatrixXd>> _TransitionDipoleMoments, std::weak_ptr<QC::Laser> _laser) : QMainWindow()
{
	this->wavefunction = wavefunction;
	this->ci_energys = ci_energys;

	if (!_TransitionDipoleMoments.expired())
	{
		this->is_transition_dipole_moments_set = true;
		this->_TransitionDipoleMoments = _TransitionDipoleMoments;
	}
	if (!_laser.expired())
	{
		this->is_laser_set = true;
		this->_laser = _laser;
	}

	//Rechter Docker
	QDockWidget *right_docker = new QDockWidget(tr("Analysis Tools"), this);
	right_docker->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	QTabWidget* tab_widget = new QTabWidget;
	right_docker->setWidget(tab_widget);
	right_docker->setMinimumWidth(250);
	addDockWidget(Qt::RightDockWidgetArea, right_docker);

	//Tab 1
	{
		QScrollArea* scrollArea_population = new QScrollArea;
		scrollArea_population->setFrameShape(QFrame::NoFrame);
		scrollArea_population->setWidgetResizable(true);
		QWidget* content_population = new QWidget;
		QVBoxLayout* layout_population = new QVBoxLayout;
		layout_population->setAlignment(Qt::AlignTop);
		content_population->setLayout(layout_population);
		scrollArea_population->setWidget(content_population);
		tab_widget->addTab(scrollArea_population, "Population");

		layout_population->addWidget(new QLabel("Select Initial State of Propagation"));
		select_initial_state_population = new QComboBox;
		for (int i = 0; i < this->wavefunction->get_states().size(); i++)
		{
			select_initial_state_population->addItem("State: " + QString::number(this->wavefunction->get_states()[i]));
		}
		layout_population->addWidget(select_initial_state_population);

		auto line = new QFrame;
		line->setFrameShape(QFrame::HLine);
		line->setFrameShadow(QFrame::Sunken);
		layout_population->addWidget(line);

		layout_population->addWidget(new QLabel("Select states for which the population should be displayed"));
		states_for_population = new QLineEdit;
		layout_population->addWidget(states_for_population);

		QPushButton* auto_button = new QPushButton("Auto");
		connect(auto_button, &QPushButton::clicked, this, &TDWF_Analysis_Window::calc_mostpopularstates);
		layout_population->addWidget(auto_button);
		mostpopularcalculated = false;

		auto line2 = new QFrame;
		line2->setFrameShape(QFrame::HLine);
		line2->setFrameShadow(QFrame::Sunken);
		layout_population->addWidget(line2);

		QPushButton* plot_button = new QPushButton("Plot");
		connect(plot_button, &QPushButton::clicked, this, &TDWF_Analysis_Window::plot_population);
		layout_population->addWidget(plot_button);

		QPushButton* save_button = new QPushButton("Save to File");
		connect(save_button, &QPushButton::clicked, this, &TDWF_Analysis_Window::save_population);
		layout_population->addWidget(save_button);
	}

	//Tab 2
	{
		QScrollArea* scrollArea_norm = new QScrollArea;
		scrollArea_norm->setFrameShape(QFrame::NoFrame);
		scrollArea_norm->setWidgetResizable(true);
		QWidget* content_norm = new QWidget;
		QVBoxLayout* layout_norm = new QVBoxLayout;
		layout_norm->setAlignment(Qt::AlignTop);
		content_norm->setLayout(layout_norm);
		scrollArea_norm->setWidget(content_norm);
		tab_widget->addTab(scrollArea_norm, "Norm");

		layout_norm->addWidget(new QLabel("Select Initial State of Propagation"));
		select_initial_state_norm = new QComboBox;
		for (int i = 0; i < this->wavefunction->get_states().size(); i++)
		{
			select_initial_state_norm->addItem("State: " + QString::number(this->wavefunction->get_states()[i]));
		}
		layout_norm->addWidget(select_initial_state_norm);

		QPushButton* plot_button = new QPushButton("Plot");
		connect(plot_button, &QPushButton::clicked, this, &TDWF_Analysis_Window::plot_norm);
		layout_norm->addWidget(plot_button);

		QPushButton* save_button = new QPushButton("Save to File");
		connect(save_button, &QPushButton::clicked, this, &TDWF_Analysis_Window::save_norm);
		layout_norm->addWidget(save_button);
	}

	//Tab 3 (Photo Electron Spectrum)
	{
		QScrollArea* scrollArea_pes = new QScrollArea;
		scrollArea_pes->setFrameShape(QFrame::NoFrame);
		scrollArea_pes->setWidgetResizable(true);
		QWidget* content_pes = new QWidget;
		QVBoxLayout* layout_pes = new QVBoxLayout;
		layout_pes->setAlignment(Qt::AlignTop);
		content_pes->setLayout(layout_pes);
		scrollArea_pes->setWidget(content_pes);
		tab_widget->addTab(scrollArea_pes, "Photo Electron Spectrum");

		layout_pes->addWidget(new QLabel("Select Initial State of Propagation"));
		select_initial_state_pes = new QComboBox;
		for (int i = 0; i < this->wavefunction->get_states().size(); i++)
		{
			select_initial_state_pes->addItem("State: " + QString::number(this->wavefunction->get_states()[i]));
		}
		layout_pes->addWidget(select_initial_state_pes);

		QPushButton* plot_button = new QPushButton("Plot");
		connect(plot_button, &QPushButton::clicked, this, &TDWF_Analysis_Window::plot_pes);
		layout_pes->addWidget(plot_button);

		QPushButton* save_button = new QPushButton("Save to File");
		connect(save_button, &QPushButton::clicked, this, &TDWF_Analysis_Window::save_pes);
		layout_pes->addWidget(save_button);
	}

	//Tab 4 (Time-dependent Dipole-Moment)
	if(is_transition_dipole_moments_set)
	{
		QScrollArea* scrollArea_TDDipoleMoment = new QScrollArea;
		scrollArea_TDDipoleMoment->setFrameShape(QFrame::NoFrame);
		scrollArea_TDDipoleMoment->setWidgetResizable(true);
		QWidget* content_TDDipoleMoment = new QWidget;
		QVBoxLayout* layout_TDDipoleMoment = new QVBoxLayout;
		layout_TDDipoleMoment->setAlignment(Qt::AlignTop);
		content_TDDipoleMoment->setLayout(layout_TDDipoleMoment);
		scrollArea_TDDipoleMoment->setWidget(content_TDDipoleMoment);
		tab_widget->addTab(scrollArea_TDDipoleMoment, "Time-Dependent Dipole Moment");

		layout_TDDipoleMoment->addWidget(new QLabel("Select Initial State of Propagation"));
		select_initial_state_TDDipoleMoment = new QComboBox;
		for (int i = 0; i < this->wavefunction->get_states().size(); i++)
		{
			select_initial_state_TDDipoleMoment->addItem("State: " + QString::number(this->wavefunction->get_states()[i]));
		}
		layout_TDDipoleMoment->addWidget(select_initial_state_TDDipoleMoment);

		QPushButton* plot_button = new QPushButton("Calculate TD Dipole Moment");
		connect(plot_button, &QPushButton::clicked, this, &TDWF_Analysis_Window::plot_tddipolemoments);
		layout_TDDipoleMoment->addWidget(plot_button);

		QPushButton* save_button = new QPushButton("Save to File");
		connect(save_button, &QPushButton::clicked, this, &TDWF_Analysis_Window::save_tddipolemoments);
		layout_TDDipoleMoment->addWidget(save_button);

		if (is_laser_set)
		{
			QPushButton* polarization_button = new QPushButton("Calculate Polarization");
			connect(polarization_button, &QPushButton::clicked, this, &TDWF_Analysis_Window::open_polarization_window);
			layout_TDDipoleMoment->addWidget(polarization_button);
		}
	}

	//Central
	chart = new QChart();
	chart->legend()->hide();
	chart->setTitle("Plot Window");

	QChartView* chartView = new QChartView(chart);
	chartView->setRenderHint(QPainter::Antialiasing);

	this->setCentralWidget(chartView);
	
	
}

void TDWF_Analysis_Window::plot_population()
{
	chart->setTitle("Plot Population");
	chart->removeAllSeries();
	int initial_state = this->select_initial_state_population->currentIndex();
	std::vector<double> times = this->wavefunction->get_times();
	std::vector<Eigen::MatrixXcd> ci_vectors = this->wavefunction->get_ci_vectors();

	//QMessageBox msgBox;
	//msgBox.setText(QString::number(initial_state));
	//msgBox.exec();

	QStringList series_list = this->states_for_population->text().split(QRegExp("\\s+"));
	for (int i = 0; i < series_list.size(); i++)
	{
		QSplineSeries* series = new QSplineSeries();
		series->setName("State " + series_list[i]);
		//QLineSeries * series = new QLineSeries();
		for (int timestep = 0; timestep < times.size(); timestep++)
		{
			double pop = ci_vectors[timestep](series_list[i].toInt(), initial_state).real() * ci_vectors[timestep](series_list[i].toInt(), initial_state).real();
			pop += ci_vectors[timestep](series_list[i].toInt(), initial_state).imag() * ci_vectors[timestep](series_list[i].toInt(), initial_state).imag();
			series->append(times[timestep], pop);
		}
		chart->addSeries(series);
	}
	chart->createDefaultAxes();
	chart->legend()->setVisible(true);
}

void TDWF_Analysis_Window::save_population()
{
	QString FileName = QFileDialog::getSaveFileName(nullptr, tr("Save Data File"), QDir::homePath(), tr("txt File (*.txt)"));

	if (!FileName.isEmpty())
	{

		int initial_state = this->select_initial_state_population->currentIndex();
		std::vector<double> times = this->wavefunction->get_times();
		std::vector<Eigen::MatrixXcd> ci_vectors = this->wavefunction->get_ci_vectors();

		QStringList series_list = this->states_for_population->text().split(QRegExp("\\s+"));
		Eigen::MatrixXd all_data(times.size(), series_list.size() + 1);


		//Fill in Time
		for (int timestep = 0; timestep < times.size(); timestep++)
		{
			all_data(timestep, 0) = times[timestep];
		}

		//Fill in Pops
		for (int i = 0; i < series_list.size(); i++)
		{
			for (int timestep = 0; timestep < times.size(); timestep++)
			{
				double pop = ci_vectors[timestep](series_list[i].toInt(), initial_state).real() * ci_vectors[timestep](series_list[i].toInt(), initial_state).real();
				pop += ci_vectors[timestep](series_list[i].toInt(), initial_state).imag() * ci_vectors[timestep](series_list[i].toInt(), initial_state).imag();
				all_data(timestep, i + 1) = pop;
			}
		}

		std::ofstream outfile;
		outfile.open(FileName.toStdString());
		outfile << "Time ";
		for (int i = 0; i < series_list.size(); i++)
		{
			outfile << "State:" << series_list[i].toInt() << " ";
		}
		outfile << std::endl;
		outfile << all_data;
		outfile.close();
	}
}

void TDWF_Analysis_Window::calc_mostpopularstates() {
	if (mostpopularcalculated) {
		states_for_population->setText(QString::fromStdString(mostpopularstates));
	}
	else {
		mostpopularstates = "";
		const double relevance = 0.05;
		int initial_state = this->select_initial_state_population->currentIndex();
		std::vector<Eigen::MatrixXcd> ci_vectors = this->wavefunction->get_ci_vectors();
		std::vector<double> times = this->wavefunction->get_times();
		for (int timestep = 0; timestep < times.size(); timestep++)
		{
			double sum = 0;
			for (int i = 0; i < ci_vectors[0].size(); i++) {
				if (sum <= 1-relevance) {
					double pop = ci_vectors[timestep](i, initial_state).real() * ci_vectors[timestep](i, initial_state).real();
					pop += ci_vectors[timestep](i, initial_state).imag() * ci_vectors[timestep](i, initial_state).imag();
					sum += pop;
					if (pop >= relevance) {
						if (mostpopularstates.find(std::to_string(i)) == std::string::npos)
						{
							mostpopularstates.append(" ");
							mostpopularstates.append(std::to_string(i));
						}
					}
				}
			}
		}
		mostpopularcalculated = true;
		states_for_population->setText(QString::fromStdString(mostpopularstates));
	}
}

void TDWF_Analysis_Window::plot_norm()
{
	chart->setTitle("Plot Norm");
	chart->removeAllSeries();
	int initial_state = this->select_initial_state_norm->currentIndex();
	std::vector<double> times = this->wavefunction->get_times();
	std::vector<Eigen::MatrixXcd> ci_vectors = this->wavefunction->get_ci_vectors();
	QSplineSeries* series = new QSplineSeries();

	for (int timestep = 0; timestep < times.size(); timestep++)
	{
		double norm = 0;
		for (int state = 0; state < ci_vectors[0].rows(); state++)
		{
			norm += ci_vectors[timestep](state, initial_state).real() * ci_vectors[timestep](state, initial_state).real() + ci_vectors[timestep](state, initial_state).imag() * ci_vectors[timestep](state, initial_state).imag();
		}
		series->append(times[timestep], norm);
	}
	chart->addSeries(series);
	chart->createDefaultAxes();
	chart->legend()->setVisible(false);
}

void TDWF_Analysis_Window::save_norm() 
{
	QString FileName = QFileDialog::getSaveFileName(nullptr, tr("Save Data File"), QDir::homePath(), tr("txt File (*.txt)"));

	if (!FileName.isEmpty())
	{
		int initial_state = this->select_initial_state_norm->currentIndex();
		std::vector<double> times = this->wavefunction->get_times();
		std::vector<Eigen::MatrixXcd> ci_vectors = this->wavefunction->get_ci_vectors();

		Eigen::MatrixXd all_data(times.size(), 2);
		for (int timestep = 0; timestep < times.size(); timestep++)
		{
			double norm = 0;
			for (int state = 0; state < ci_vectors[0].rows(); state++)
			{
				norm += ci_vectors[timestep](state, initial_state).real() * ci_vectors[timestep](state, initial_state).real() + ci_vectors[timestep](state, initial_state).imag() * ci_vectors[timestep](state, initial_state).imag();
			}
			all_data(timestep, 0) = times[timestep];
			all_data(timestep, 1) = norm;
		}

		std::ofstream outfile;
		outfile.open(FileName.toStdString());
		outfile << "Time Norm";
		outfile << std::endl;
		outfile << all_data;
		outfile.close();
	}
}

void TDWF_Analysis_Window::plot_pes()
{
	chart->setTitle("Plot Photo Electon Spectrum");
	chart->removeAllSeries();
	int initial_state = this->select_initial_state_pes->currentIndex();
	std::vector<double> times = this->wavefunction->get_times();
	std::vector<Eigen::MatrixXcd> ci_vectors = this->wavefunction->get_ci_vectors();
	QSplineSeries* series = new QSplineSeries();

	int timestep = times.size() - 1;
	for (int energy_index = 0; energy_index < ci_energys.size(); energy_index++)
	{
		double abs = ci_vectors[timestep](energy_index, initial_state).real()* ci_vectors[timestep](energy_index, initial_state).real() + ci_vectors[timestep](energy_index, initial_state).imag() * ci_vectors[timestep](energy_index, initial_state).imag();
		series->append(this->ci_energys[energy_index], abs);
	}

	chart->addSeries(series);
	chart->createDefaultAxes();
	chart->legend()->setVisible(false);
}

void TDWF_Analysis_Window::save_pes()
{
	QString FileName = QFileDialog::getSaveFileName(nullptr, tr("Save Data File"), QDir::homePath(), tr("txt File (*.txt)"));

	if (!FileName.isEmpty())
	{
		int initial_state = this->select_initial_state_pes->currentIndex();
		std::vector<double> times = this->wavefunction->get_times();
		std::vector<Eigen::MatrixXcd> ci_vectors = this->wavefunction->get_ci_vectors();
		Eigen::MatrixXd all_data(ci_energys.size(), 2);

		int timestep = times.size() - 1;
		for (int energy_index = 0; energy_index < ci_energys.size(); energy_index++)
		{
			double abs = ci_vectors[timestep](energy_index, initial_state).real() * ci_vectors[timestep](energy_index, initial_state).real() + ci_vectors[timestep](energy_index, initial_state).imag() * ci_vectors[timestep](energy_index, initial_state).imag();
			all_data(energy_index, 0) = this->ci_energys[energy_index];
			all_data(energy_index, 1) = abs;
		}

		std::ofstream outfile;
		outfile.open(FileName.toStdString());
		outfile << "Energy I";
		outfile << std::endl;
		outfile << all_data;
		outfile.close();
	}
}

void TDWF_Analysis_Window::plot_tddipolemoments()
{
	chart->setTitle("Time-Dependent Dipole Moment");
	chart->removeAllSeries();
	int initial_state = this->select_initial_state_pes->currentIndex();
	std::vector<double> times = this->wavefunction->get_times();
	
	QSplineSeries* series_x = new QSplineSeries();
	QSplineSeries* series_y = new QSplineSeries();
	QSplineSeries* series_z = new QSplineSeries();
	QSplineSeries* series_total = new QSplineSeries();
	series_x->setName("X");
	series_y->setName("Y");
	series_z->setName("Z");
	series_total->setName("Total");

	std::vector<Eigen::MatrixXcd> ci_vectors = this->wavefunction->get_ci_vectors();
	std::vector<Eigen::MatrixXd> transition_dipole_moments = *(this->_TransitionDipoleMoments.lock().get());

	for (int timestep = 0; timestep < times.size(); timestep++)
	{
		std::complex<double> x(0, 0);
		std::complex<double> y(0, 0);
		std::complex<double> z(0, 0);
		for (int p = 0; p < ci_vectors[0].rows(); p++)
		{
			if(abs(ci_vectors[timestep](p, initial_state)) > 0.000001)
			{
				for (int q = 0; q < ci_vectors[0].rows(); q++)
				{
					if(abs(ci_vectors[timestep](q, initial_state)) > 0.000001)
					{
						std::complex<double> prefac = ci_vectors[timestep](p, initial_state) * std::conj(ci_vectors[timestep](q, initial_state));
						x += prefac * transition_dipole_moments[0](p, q);
						y += prefac * transition_dipole_moments[1](p, q);
						z += prefac * transition_dipole_moments[2](p, q);
					}
				}
			}
		}

		//std::cout << x << " " << y << " " << z << std::endl;
		double x_abs = x.real() * x.real();
		double y_abs = y.real() * y.real();
		double z_abs = z.real() * z.real();

		double total = sqrt(x_abs + y_abs + z_abs);

		series_x->append(times[timestep], x.real());
		series_y->append(times[timestep], y.real());
		series_z->append(times[timestep], z.real());
		series_total->append(times[timestep], total);
	}

	chart->addSeries(series_x);
	chart->addSeries(series_y);
	chart->addSeries(series_z);
	//chart->addSeries(series_total);
	chart->createDefaultAxes();
	chart->legend()->setVisible(true);
}

void TDWF_Analysis_Window::save_tddipolemoments()
{
	QString FileName = QFileDialog::getSaveFileName(nullptr, tr("Save Data File"), QDir::homePath(), tr("txt File (*.txt)"));

	if (!FileName.isEmpty())
	{
		int initial_state = this->select_initial_state_pes->currentIndex();
		std::vector<double> times = this->wavefunction->get_times();

		Eigen::MatrixXd all_data(times.size(), 5);

		std::vector<Eigen::MatrixXcd> ci_vectors = this->wavefunction->get_ci_vectors();
		std::vector<Eigen::MatrixXd> transition_dipole_moments = *(this->_TransitionDipoleMoments.lock().get());

		for (int timestep = 0; timestep < times.size(); timestep++)
		{
			std::complex<double> x(0, 0);
			std::complex<double> y(0, 0);
			std::complex<double> z(0, 0);
			for (int p = 0; p < ci_vectors[0].rows(); p++)
			{
				if(abs(ci_vectors[timestep](p, initial_state)) > 0.000001)
				{
					for (int q = 0; q < ci_vectors[0].rows(); q++)
					{
						if(abs(ci_vectors[timestep](q, initial_state)) > 0.000001)
						{
							std::complex<double> prefac = ci_vectors[timestep](p, initial_state) * std::conj(ci_vectors[timestep](q, initial_state));
							x += prefac * transition_dipole_moments[0](p, q);
							y += prefac * transition_dipole_moments[1](p, q);
							z += prefac * transition_dipole_moments[2](p, q);
						}
					}
				}
			}

			//std::cout << x << " " << y << " " << z << std::endl;
			double x_abs = x.real() * x.real();
			double y_abs = y.real() * y.real();
			double z_abs = z.real() * z.real();

			double total = sqrt(x_abs + y_abs + z_abs);

			all_data(timestep, 0) = times[timestep];
			all_data(timestep, 1) = x.real();
			all_data(timestep, 2) = y.real();
			all_data(timestep, 3) = z.real();
			all_data(timestep, 4) = total;
		}

		std::ofstream outfile;
		outfile.open(FileName.toStdString());
		outfile << "Time x y z abs";
		outfile << std::endl;
		outfile << all_data;
		outfile.close();
	}
}

void TDWF_Analysis_Window::open_polarization_window()
{
	std::vector<double> times = this->wavefunction->get_times();


	QWidget* polarization_dialog = new QWidget;
	QVBoxLayout* layout = new QVBoxLayout(polarization_dialog);


	QLineEdit* starttime = new QLineEdit;
	starttime->setValidator(new QDoubleValidator(0, times[times.size() - 1], 2, this));
	starttime->setText(0);
	QLineEdit* endtime = new QLineEdit;
	endtime->setValidator(new QDoubleValidator(0, times[times.size() - 1], 2, this));
	endtime->setText(0);
	//connect(endtime, &QLineEdit::textChanged, this, &TDWF_Analysis_Window::calculate_polarization);
	QLineEdit* polarization = new QLineEdit;
	polarization->setReadOnly(true);
	polarization->setText("inf");

	//connect(starttime, &QLineEdit::textChanged, this, [=]() {this->calculate_polarization(starttime, endtime, polarization); });
	//connect(endtime, &QLineEdit::textChanged, this, [=]() {this->calculate_polarization(starttime, endtime, polarization); });

	QFormLayout* inputLayout = new QFormLayout;
	inputLayout->addRow(tr("&Start-Time:"), starttime);
	inputLayout->addRow(tr("&End-Time:"), endtime);
	inputLayout->addRow(tr("&Polarization:"), polarization);
	QWidget* inputWidget = new QWidget;
	inputWidget->setLayout(inputLayout);
	layout->addWidget(inputWidget);

	QPushButton* calculate_button = new QPushButton("Calculate Polarization");
	connect(calculate_button, &QPushButton::clicked, this, [=]() {this->calculate_polarization(starttime, endtime, polarization); });
	layout->addWidget(calculate_button);

	polarization_dialog->show();
}

void TDWF_Analysis_Window::calculate_polarization(QLineEdit* starttime_lineedit, QLineEdit* endtime_lineedit, QLineEdit* polarization_lineedit)
{
	//Fix Times
	double starttime = starttime_lineedit->text().toDouble();
	double endtime = endtime_lineedit->text().toDouble();

	std::vector<double> times = this->wavefunction->get_times();
	int starttime_idx = find_closest_timepoint(starttime);
	int endtime_idx = find_closest_timepoint(endtime);
	starttime = times[starttime_idx];
	endtime = times[endtime_idx];

	starttime_lineedit->setText(QString::number(starttime));
	endtime_lineedit->setText(QString::number(endtime));

	//Get Delta_E
	QC::Laser* laser = this->_laser.lock().get();

	std::vector<double> field_strenght1 = laser->get_field_strengths(starttime);
	std::vector<double> field_strenght2 = laser->get_field_strengths(endtime);
	double delta_E = sqrt(field_strenght2[0] * field_strenght2[0] + field_strenght2[1] * field_strenght2[1] + field_strenght2[2] * field_strenght2[2]) - sqrt(field_strenght1[0] * field_strenght1[0] + field_strenght1[1] * field_strenght1[1] + field_strenght1[2] * field_strenght1[2]);
	
	//Get Delta mu
	int initial_state = this->select_initial_state_pes->currentIndex();
	std::vector<Eigen::MatrixXcd> ci_vectors = this->wavefunction->get_ci_vectors();
	std::vector<Eigen::MatrixXd> transition_dipole_moments = *(this->_TransitionDipoleMoments.lock().get());

	double mu_entime, mu_starttime;
	{
		std::complex<double> x(0, 0);
		std::complex<double> y(0, 0);
		std::complex<double> z(0, 0);
		std::complex<double> total(0, 0);
		for (int p = 0; p < ci_vectors[0].rows(); p++)
		{
			for (int q = 0; q < ci_vectors[0].rows(); q++)
			{
				std::complex<double> prefac = ci_vectors[endtime_idx](p, initial_state) * ci_vectors[endtime_idx](q, initial_state);
				x += prefac * transition_dipole_moments[0](p, q);
				y += prefac * transition_dipole_moments[1](p, q);
				z += prefac * transition_dipole_moments[2](p, q);
			}
		}
		double x_abs = x.real() * x.real() + x.imag() * x.imag();
		double y_abs = y.real() * y.real() + y.imag() * y.imag();
		double z_abs = z.real() * z.real() + z.imag() * z.imag();

		mu_entime = sqrt(x_abs + y_abs + z_abs);
	}
	{
		std::complex<double> x(0, 0);
		std::complex<double> y(0, 0);
		std::complex<double> z(0, 0);
		std::complex<double> total(0, 0);
		for (int p = 0; p < ci_vectors[0].rows(); p++)
		{
			for (int q = 0; q < ci_vectors[0].rows(); q++)
			{
				std::complex<double> prefac = ci_vectors[starttime_idx](p, initial_state) * ci_vectors[starttime_idx](q, initial_state);
				x += prefac * transition_dipole_moments[0](p, q);
				y += prefac * transition_dipole_moments[1](p, q);
				z += prefac * transition_dipole_moments[2](p, q);
			}
		}
		double x_abs = x.real() * x.real() + x.imag() * x.imag();
		double y_abs = y.real() * y.real() + y.imag() * y.imag();
		double z_abs = z.real() * z.real() + z.imag() * z.imag();

		mu_starttime = sqrt(x_abs + y_abs + z_abs);
	}
	double delta_mu = mu_entime - mu_starttime;

	//Finalize
	if (delta_E == 0)
	{
		polarization_lineedit->setText("inf");
	}
	else
	{
		double polarization = delta_mu / delta_E;
		polarization_lineedit->setText(QString::number(polarization));
	}

}

int TDWF_Analysis_Window::find_closest_timepoint(double time)
{
	std::vector<double> times = this->wavefunction->get_times();
	
	if (time <= times[0]) { return 0; }
	else if (time >= times[times.size() - 1]) { return times.size() - 1; }
	else
	{
		for(int i = 0; i < times.size() - 1; i++)
		{
			if (time >= times[i] && time <= times[i + 1])
			{
				if(abs(time - times[i]) < abs(time - times[i + 1])){ return i;}
				else {return i + 1;}
			}
		}
	}
	return 0;
}