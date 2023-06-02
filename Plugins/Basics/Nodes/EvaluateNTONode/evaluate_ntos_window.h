#pragma once
//#include <thread>
#include <math.h> 
#include <iostream>
#include <fstream>
#include <complex>

#include <QMainWindow>
#include <QWidget>
#include <QtWidgets>
//#include <QApplication>
//#include <QMessageBox>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>

#include "../../Lib/header/tdci_wf.h"
#include "../../Lib/header/gaussian_basisset.h"
#include "../../Lib/header/integrator_libcint_interface_1e.h"
#include "../../Lib/header/gaussian_basisset.h"
#include "../../Lib/header/transformation_integrals_basisfunction_to_hforbitals.h"
#include "../../Lib/header/CIResults.h"
//#include "../../ThirdParty/eigen/Eigen/Dense"


QT_CHARTS_USE_NAMESPACE

class EvaluateTIDWidget : public QWidget
{
public:
    EvaluateTIDWidget(QWidget* parent = nullptr)
    {
        QVBoxLayout* layout_left = new QVBoxLayout;
        QVBoxLayout* layout_right = new QVBoxLayout;
        QHBoxLayout* layout_sum = new QHBoxLayout;

        layout_sum->addLayout(layout_left, 33);
        layout_sum->addLayout(layout_right, 66);
        this->setLayout(layout_sum);

        layout_left->addWidget(new QLabel("Select Excited States"));
        this->excited_states_list = new QListWidget;
        layout_left->addWidget(excited_states_list);
        calculate_button = new QPushButton("Calculate");
        layout_left->addWidget(calculate_button);

        layout_right->addWidget(new QLabel("Results"));
        result_table = new QTableWidget;
        layout_right->addWidget(result_table);
        result_table->setColumnCount(4);
        result_table->setRowCount(6);
        result_table->setHorizontalHeaderItem(0, new QTableWidgetItem("x"));
        result_table->setHorizontalHeaderItem(1, new QTableWidgetItem("y"));
        result_table->setHorizontalHeaderItem(2, new QTableWidgetItem("z"));
        result_table->setHorizontalHeaderItem(3, new QTableWidgetItem("total"));

        result_table->setVerticalHeaderItem(0, new QTableWidgetItem("Hole-Position"));
        result_table->setVerticalHeaderItem(1, new QTableWidgetItem("Particle-Position"));
        result_table->setVerticalHeaderItem(2, new QTableWidgetItem("average hole particle distance"));
        result_table->setVerticalHeaderItem(3, new QTableWidgetItem("hole size"));
        result_table->setVerticalHeaderItem(4, new QTableWidgetItem("particle size"));
        result_table->setVerticalHeaderItem(5, new QTableWidgetItem("exciton size"));

    }
    ~EvaluateTIDWidget() {}
    QListWidget* excited_states_list;
    QPushButton* calculate_button;
    QTableWidget* result_table;

};

class EvaluateTDWidget : public QWidget
{
public:
    EvaluateTDWidget(QWidget* parent = nullptr)
    {
        QVBoxLayout* layout_left = new QVBoxLayout;
        QVBoxLayout* layout_right = new QVBoxLayout;
        QHBoxLayout* layout_sum = new QHBoxLayout;

        layout_sum->addLayout(layout_left, 33);
        layout_sum->addLayout(layout_right, 66);
        this->setLayout(layout_sum);

        chart = new QChart;
        chart->setTitle("Plot Window");
        QChartView* chartView = new QChartView(chart);
        chartView->setRenderHint(QPainter::Antialiasing);
        layout_right->addWidget(chartView);

        plot_selector = new QListWidget;
        plot_selector->addItem("Hole-Position");
        plot_selector->addItem("Particle-Position");
        plot_selector->addItem("average hole particle distance");
        plot_selector->addItem("hole size");
        plot_selector->addItem("particle size");
        plot_selector->addItem("exciton size");
        plot_selector->setCurrentRow(0);
        layout_left->addWidget(plot_selector);
        MultiplyWithNorm_box = new QCheckBox("Multiply with norm");
        layout_left->addWidget(MultiplyWithNorm_box);
        calculate_button = new QPushButton("Calculate");
        save_button = new QPushButton("Save all to file");
        layout_left->addWidget(calculate_button);
        layout_left->addWidget(save_button);
        layout_left->setAlignment(Qt::AlignTop);


    }
    ~EvaluateTDWidget() {}
    QPushButton* calculate_button;
    QPushButton* save_button;
    QChart* chart;
    QListWidget* plot_selector;
    QCheckBox* MultiplyWithNorm_box;

    bool intermediates_calculated = false;
    Eigen::MatrixXd S, Mx, My, Mz, Mxx, Mxy, Mxz, Myx, Myy, Myz, Mzx, Mzy, Mzz;
    std::vector<Eigen::MatrixXcd> TD_OnePTDMs;

    double norm_threshold = 0.001;

};


class Evaluate_NTOs_Window : public QMainWindow
{

public:
    explicit Evaluate_NTOs_Window(
        std::shared_ptr <QC::CIResults> ciresults,
        std::shared_ptr <Eigen::MatrixXd> hfcmatrix,
        std::shared_ptr <QC::Gaussian_Basisset> basissetdata,
        std::shared_ptr <QC::TDCI_WF> TDCICoefficients
    );



private:
    QC::CIResults ciresults;
    std::shared_ptr <Eigen::MatrixXd> hfcmatrix;
    std::shared_ptr <QC::Gaussian_Basisset> basissetdata;
    std::shared_ptr <QC::TDCI_WF> TDCICoefficients;

    EvaluateTIDWidget* TID_NTOs_Evaluation;
    EvaluateTDWidget* TD_NTOs_Evaluation;

public:
    void update_tid_results();
    void plot_td_results();
    void update_td_matrices();
    void update_td_OnePTDMs();
    void save_td_results();
};