#pragma once

#include <QMainWindow>
#include <QtWidgets>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>

#include <iostream>
#include <fstream>
#include <complex>


#include "../../Lib/header/tdci_wf.h"
#include "../../Lib/header/laser.h"

QT_CHARTS_USE_NAMESPACE


class TDWF_Analysis_Window : public QMainWindow
{

public:
    explicit TDWF_Analysis_Window(QC::TDCI_WF* wavefunction, Eigen::VectorXd ci_energys, std::weak_ptr<std::vector<Eigen::MatrixXd>> _TransitionDipoleMoments, std::weak_ptr<QC::Laser> _laser);
	
private:
    QC::TDCI_WF* wavefunction;
    Eigen::VectorXd ci_energys;

    bool is_transition_dipole_moments_set = false;
    std::weak_ptr<std::vector<Eigen::MatrixXd>> _TransitionDipoleMoments;
    bool is_laser_set = false;
    std::weak_ptr<QC::Laser> _laser;


private:
    QChart* chart;

    QComboBox* select_initial_state_population;
    QLineEdit* states_for_population;
    void plot_population();
    void save_population();

    QComboBox* select_initial_state_norm;
    void plot_norm();
    void save_norm();

    QComboBox* select_initial_state_pes;
    void plot_pes();
    void save_pes();

    std::string mostpopularstates;
    bool mostpopularcalculated;
    void calc_mostpopularstates();

    QComboBox* select_initial_state_TDDipoleMoment;
    void plot_tddipolemoments();
    void save_tddipolemoments();

    void open_polarization_window();
    void calculate_polarization(QLineEdit* starttime_lineedit, QLineEdit* endtime_lineedit, QLineEdit* polarization_lineedit);
    int find_closest_timepoint(double time);
};