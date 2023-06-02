#pragma once

#include <QMainWindow>
#include <QtWidgets>
#include <Eigen/Dense>


class Transition_Dipole_Moments_Window : public QMainWindow
{
public:
    explicit Transition_Dipole_Moments_Window(std::vector<Eigen::MatrixXd> transition_dipole_moments);
	
private:
    std::vector<Eigen::MatrixXd> transition_dipole_moments;

private:
    QTableWidget* transition_dipole_table;
    QListWidget* state1_list;
    void update_table();
};

