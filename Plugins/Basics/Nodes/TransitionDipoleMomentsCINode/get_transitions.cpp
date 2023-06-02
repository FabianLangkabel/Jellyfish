#include "get_transitions.h"


Transition_Dipole_Moments_Window::Transition_Dipole_Moments_Window(std::vector<Eigen::MatrixXd> transition_dipole_moments) : QMainWindow()
{
	this->transition_dipole_moments = transition_dipole_moments;

	QVBoxLayout* layout_left = new QVBoxLayout;
	QVBoxLayout* layout_right = new QVBoxLayout;
	QHBoxLayout* layout_sum = new QHBoxLayout;
	layout_sum->addLayout(layout_left);
	layout_sum->addLayout(layout_right);

	layout_left->addWidget(new QLabel("Select State 1"));
	state1_list = new QListWidget;


	for (int i = 0; i < this->transition_dipole_moments[0].rows(); i++)
	{
		state1_list->addItem("State " + QString::number(i));
	}
	state1_list->setCurrentRow(0);
	layout_left->addWidget(state1_list);
	connect(state1_list, &QListWidget::itemSelectionChanged, this, &Transition_Dipole_Moments_Window::update_table);

	layout_right->addWidget(new QLabel("List of (Transition) Dipole Moments"));
	transition_dipole_table = new QTableWidget;
	transition_dipole_table->setRowCount(this->transition_dipole_moments[0].rows());
	transition_dipole_table->setColumnCount(4);
	transition_dipole_table->setHorizontalHeaderItem(0, new QTableWidgetItem("Transitiondipole x"));
	transition_dipole_table->setHorizontalHeaderItem(1, new QTableWidgetItem("Transitiondipole y"));
	transition_dipole_table->setHorizontalHeaderItem(2, new QTableWidgetItem("Transitiondipole z"));
	transition_dipole_table->setHorizontalHeaderItem(3, new QTableWidgetItem("Transitiondipole total"));
	for (int i = 0; i < this->transition_dipole_moments[0].rows(); i++)
	{
		transition_dipole_table->setVerticalHeaderItem(i, new QTableWidgetItem("State " + QString::number(i)));
	}
	layout_right->addWidget(transition_dipole_table);
	update_table();

	QWidget* central_widget = new QWidget;
	central_widget->setLayout(layout_sum);
	this->setCentralWidget(central_widget);
}

void Transition_Dipole_Moments_Window::update_table()
{
	for (int i = 0; i < this->transition_dipole_moments[0].rows(); i++)
	{
		double x = transition_dipole_moments[0](state1_list->currentRow(), i);
		double y = transition_dipole_moments[1](state1_list->currentRow(), i);
		double z = transition_dipole_moments[2](state1_list->currentRow(), i);
		double sum = sqrt(x*x+y*y+z*z);

		transition_dipole_table->setItem(i, 0, new QTableWidgetItem(QString::number(x)));
		transition_dipole_table->setItem(i, 1, new QTableWidgetItem(QString::number(y)));
		transition_dipole_table->setItem(i, 2, new QTableWidgetItem(QString::number(z)));
		transition_dipole_table->setItem(i, 3, new QTableWidgetItem(QString::number(sum)));
	}
}