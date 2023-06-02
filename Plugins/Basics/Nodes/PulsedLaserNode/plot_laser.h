#pragma once

#include <QMainWindow>
#include <QtWidgets>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

#include "../../Lib/header/laser.h"

QT_CHARTS_USE_NAMESPACE


class Laser_Analysis_Window : public QMainWindow
{
public:
    explicit Laser_Analysis_Window(std::shared_ptr<QC::Laser> laser);


private:
    std::shared_ptr<QC::Laser> laser;
    std::string direction;

private:
    QLineEdit* time_min;
    QLineEdit* time_max;
    QChart* chart;
    void plot();
};

