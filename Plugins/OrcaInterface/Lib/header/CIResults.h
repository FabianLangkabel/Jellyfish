#pragma once
#include "configurations.h"
#include <Eigen/Dense>
#include <unsupported/Eigen/CXX11/Tensor>

namespace QC
{

	class CIResults
	{
        public:
        	std::vector<std::string> configuration_strings;
		    Eigen::MatrixXd ci_matrix;
		    Eigen::VectorXd ci_values;
    };
}