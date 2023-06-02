#pragma once
#include <QtWidgets>

#include <string>
#include <fstream>
#include "../../../../JellyfishCore/include/NodeSystem/node.hpp"
#include "../../../../JellyfishCore/include/json.hpp"

#include "../../Lib/header/laser.h"
#include "../../save_load.h"

#include "plot_laser.h"

using json = nlohmann::json;


class PulsedLaserNode : public Node, public QObject
{
public:
    PulsedLaserNode(std::string InpNodeTypeID);
    ~PulsedLaserNode();
    void LoadData(json Data, std::string file) override;
    json SaveData(std::string file) override;
    void calculate() override;
    void NodeInspector(QWidget* Inspector) override;
    bool InPortAllowMultipleConnections(int Port) override;
    unsigned int Ports(NodePortType PortType) override;
    bool IsCustomPortCaption(NodePortType PortType, int Port) override;
    std::string CustomPortCaption(NodePortType PortType, int Port) override;
    std::string DataTypeName(NodePortType PortType, int Port) override;
    std::any getOutData(int Port) override;
    void setInData(int Port, std::any data) override;

private:
  std::shared_ptr<QC::Laser> _laserdata;
  QC::Laser Laserpulse;

private:
	std::vector<QWidget*> cos_sq_exclusive_widgets, gauss_exclusive_widgets, trapezoid_exclusive_widgets;
	void update_exclusives_in_outer_function();

	QComboBox* inner_function;
	QComboBox* outer_function;
	QGridLayout* polarization;
	QRadioButton* linearpolarizationButton;
	QRadioButton* righthandpolarizationButton;
	QRadioButton* lefthandpolarizationButton;
	int polarization_index = 0;
	QLineEdit* xPolarizationBox;
	QLineEdit* yPolarizationBox;
	QLineEdit* zPolarizationBox;
	double xPolarization = 1;
	double yPolarization = 0;
	double zPolarization = 0;
	std::vector<double> polarization_vector = std::vector<double>({1,0,0});
	
	int inner_function_index = 0;
	int outer_function_index = 0;
	void select_inner_function();
	void select_outer_function();
	void select_polarization();
	//void norm_polarization();

	QLineEdit* amplitude_lineedit;
	QLineEdit* frequency_lineedit;
	double amplitude = 0;
	double frequency = 0;
	void select_amplitude();
	void select_frequency();

	//Only for cos^2 Laser
	QLineEdit* pulse_duration_lineedit;
	double pulse_duration = 0;
	void select_pulse_duration();
	QCheckBox* cos_sqr_use_cycle_number_box;
	bool cos_sqr_use_cycle_number = false;
	void select_cos_sqr_use_cycle_number();
	QLineEdit* cos_sqr_cycle_number_lineedit;
	int cos_sqr_cycle_number = 1;
	void select_cos_sqr_cycle_number();

	void update_cos_sqr_pulse_duration();

	//Only for Gauss Laser
	QLineEdit* FWHM_lineedit;
	double FWHM = 0;
	void select_FWHM();
	QLineEdit* cycle_number_lineedit;
	int cycle_number = 0;
	void select_cycle_number();
	

	//Only for Trapezoid Laser
	QLineEdit* ascend_lineedit;
	double ascend = 0;
	void select_ascend();

	void start_analyse_laser();
	void save_laser();
};