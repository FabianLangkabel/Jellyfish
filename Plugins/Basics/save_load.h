#pragma once

#include <string>
#include <vector>
#include <zip.h>
#include <zipconf.h>
#include <Eigen/Dense>
#include <unsupported/Eigen/CXX11/Tensor>
#include <sstream>
#include <iostream>

#include "Lib/header/pointcharge.h"
#include "Lib/header/gaussian_basisset.h"
#include "Lib/header/potential_gaussian.h"
#include "Lib/header/tdci_wf.h"
//#include "Lib/header_quantum_computing/basics.h"

class save_load
{
public:
	void static save(std::string file, std::string id, std::string name, int data);
	void static save(std::string file, std::string id, std::string name, double data);
	void static save(std::string file, std::string id, std::string name, std::vector<QC::Pointcharge> data);
	void static save(std::string file, std::string id, std::string name, QC::Gaussian_Basisset data);
	void static save(std::string file, std::string id, std::string name, Eigen::MatrixXd data);
	void static save(std::string file, std::string id, std::string name, Eigen::Tensor<double, 4> data);
	void static save(std::string file, std::string id, std::string name, Eigen::VectorXd data);
	void static save(std::string file, std::string id, std::string name, Eigen::MatrixXcd data);
	void static save(std::string file, std::string id, std::string name, std::vector<std::string> data);
	void static save(std::string file, std::string id, std::string name, std::vector<QC::Potential_Gaussian> data);
	void static save(std::string file, std::string id, std::string name, QC::TDCI_WF data);
	/*
	void static save(std::string id, std::string name, QC::Pauli_Operator data);
	void static save(std::string id, std::string name, QC::QC_Statevector data);
	void static save(std::string id, std::string name, QC::QC_TD_Statevectors data);
	void static save(std::string id, std::string name, QC::QC_TD_Norm data);
	*/

	void static load(std::string file, std::string id, std::string name, int *object);
	void static load(std::string file, std::string id, std::string name, double * object);
	void static load(std::string file, std::string id, std::string name, std::vector<QC::Pointcharge> * object);
	void static load(std::string file, std::string id, std::string name, QC::Gaussian_Basisset* object);
	void static load(std::string file, std::string id, std::string name, Eigen::MatrixXd* object);
	void static load(std::string file, std::string id, std::string name, Eigen::Tensor<double, 4>* object);
	void static load(std::string file, std::string id, std::string name, Eigen::VectorXd* object);
	void static load(std::string file, std::string id, std::string name, Eigen::MatrixXcd* object);
	void static load(std::string file, std::string id, std::string name, std::vector<std::string>* object);
	void static load(std::string file, std::string id, std::string name, std::vector<QC::Potential_Gaussian> *object);
	void static load(std::string file, std::string id, std::string name, QC::TDCI_WF* object);
	/*
	void static load(std::string id, std::string name, QC::Pauli_Operator* object);
	void static load(std::string id, std::string name, QC::QC_Statevector* object);
	void static load(std::string id, std::string name, QC::QC_TD_Statevectors* object);
	void static load(std::string id, std::string name, QC::QC_TD_Norm* object);
	*/

private:
	std::string static read_single_file(std::string file, std::string id, std::string name);
	void static write_single_file(std::string file, std::string id, std::string name, std::string content);
	std::vector<std::string> static split_string_in_strings(std::string string, char delimiter);
};
