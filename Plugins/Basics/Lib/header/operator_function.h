#pragma once
#define NOMINMAX
#include <map>
#include <iostream>
#include <string>
#include <algorithm>
#include <Eigen/Dense>
#include <unsupported/Eigen/CXX11/Tensor>
#include "potential_gaussian.h"
#include <muParser.h>


namespace QC
{
	class Operator_Function
	{
	public:
		virtual double get_operator_function(double x, double y, double z) { return 0; }
		
	};

	class CAP1 : public Operator_Function
	{
	private:
		double Strength;
		double Radius_abs;
		int Order;

	public:
		void set_strength(double Strength) { this->Strength = Strength; }
		void set_radius_abs(double Radius_abs) { this->Radius_abs = Radius_abs; }
		void set_order(int Order) { this->Order = Order; }

		double get_operator_function(double x, double y, double z) override;

	};

	class CAP2 : public Operator_Function
	{
	private:
		double Potential_Max;
		double Radius_abs;
		double Curvate;

	public:
		void set_potential_max(double Potential_Max) { this->Potential_Max  = Potential_Max; }
		void set_radius_abs(double Radius_abs) { this->Radius_abs = Radius_abs; }
		void set_curvate(double Curvate) { this->Curvate = Curvate; }

		double get_operator_function(double x, double y, double z) override;

	};

	class Gaussian_Function : public Operator_Function
	{
	private:
		std::vector<QC::Potential_Gaussian> Gaussians;

	public:
		void add_potential_gaussian(Potential_Gaussian Gaussian)
		{
			Gaussians.push_back(Gaussian);
		}
		
		double get_operator_function(double x, double y, double z) override;
	};

	class Unit : public Operator_Function
	{

	public:
		double get_operator_function(double x, double y, double z) override;

	};

	class Parser_Function : public Operator_Function
	{
	private:
		std::string function_string;
	public:
		void set_function_string(std::string function_string) { this->function_string = function_string; }
		double get_operator_function(double x, double y, double z) override;
	};
}