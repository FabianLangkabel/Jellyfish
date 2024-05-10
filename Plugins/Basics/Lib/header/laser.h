#pragma once
#define NOMINMAX
#include <map>
#include <iostream>
#include <string>
#include <algorithm>
#include <Eigen/Dense>
#include <unsupported/Eigen/CXX11/Tensor>


namespace QC
{
	class Laser {
	public:
		enum class LaserType {
			CosSquared,
			Gaussian,
			Trapezoid,
			File
		};
		enum OscillationFunction {
			Sin = 0,
			NSin,
			Cos,
			NCos
		};

		Laser() = default;
		~Laser() = default;
		//~Laser() { if (m_type == LaserType::File) laser_file.~string(); };


		// polarization
		std::vector<double> polarization_vector = std::vector<double>({ 1,0,0 });
		int polarization_index = 0;

		// set general variables
		void set_type(LaserType type) { m_type = type; }
		void set_pulse_duration(double duration) { m_pulse_duration = duration;}
		void set_amplitude(double amplitude) { m_amplitude = amplitude; }
		void set_frequency(double frequency) { m_frequency = frequency; }
		double get_frequency() { return m_frequency; }
		void set_oscillation_function(int osc_function) { oscillation_function = (OscillationFunction) osc_function; }
		void set_polarization(std::vector<double> polarization_vector, int polarization_index) {
			this->polarization_vector = std::move(polarization_vector);
			this->polarization_index = polarization_index;
		}


		// set specific variables
		void set_ascention_time(double ascend) { if (m_type == LaserType::Trapezoid) { ascention_time = ascend; } }
		void set_cycle_number(int cycnumber) {
			if (m_type == LaserType::Gaussian || m_type == LaserType::CosSquared) { cycle_number = cycnumber; }
		}
		void set_use_cycle_number(bool usenumber) {
			if (m_type == LaserType::Gaussian || m_type == LaserType::CosSquared) { use_cycle_number = usenumber; }
		}
		void set_fwhm(double fwhm) {
			if (m_type == LaserType::Gaussian) { FWHM = fwhm; }
		}
		void set_laserfile(std::string Laserfile) { laser_file = Laserfile; }
		bool use_file() {
			if (m_type == LaserType::File) { return true; }
			else {return false;	}
		}

		// File
		std::string get_laserfile() {
			return laser_file;
		}

		// output(t)
		std::vector<double> get_field_strengths(double time);

		// LaserType type() const { return m_type; }
		LaserType type() { return m_type; }

	private:
		LaserType m_type;
		double m_pulse_duration;
		double m_amplitude;
		double m_frequency;
		OscillationFunction oscillation_function {OscillationFunction::Sin};  //0 = sin, 1 = -sin, 2 = cos, 3 = -cos
		
		union {
			double ascention_time;
			struct {
				int cycle_number;
				union {
					bool use_cycle_number;
					double FWHM;
				};
			};
		};
		
		/*
		double ascention_time;
		int cycle_number;
		bool use_cycle_number;
		double FWHM;
		*/
		std::string laser_file;

		// calculations
		std::vector<double> calc_fieldstrengths(double time, double Envelope_Function);
		double cyclenumber_and_frequency_to_pulse_duration();
	};
}



/*
namespace QC
{
	class Laser
	{
	public:
		//variables
		double Pulse_Duration;
		double Amplitude;
		double Frequency;
		int Oscillation_Function;  //0 = sin, 1 = -sin, 2 = cos, 3 = -cos
		
		std::vector<double> polarization_vector = std::vector<double>({ 1,0,0 });
		int polarization_index = 0;

		//functions
		virtual std::vector<double> get_field_strengths(double time)
		{
			std::vector<double> Field_Strength_Vector;
			for (int i = 0; i < 3; i++) {
				Field_Strength_Vector.push_back(0);
			}
			return Field_Strength_Vector;
		}
		virtual std::string get_laserfile() { return ""; }
		virtual bool use_file() { return false; }
		std::vector<double> calcFieldstrengths(double time, double Envelope_Function);

		void set_oscillation_function(int Oscillation_function) { this->Oscillation_Function = Oscillation_function; }
		void set_amplitude(double Amplitude) { this->Amplitude = Amplitude; }
		void set_frequency(double Frequency) { this->Frequency = Frequency; }
		void set_pulse_duration(double Pulse_Duration) { this->Pulse_Duration = Pulse_Duration; }
		void set_polarization(std::vector<double> Polarization_vector, int Polarization_index) {
			this->polarization_vector = Polarization_vector;
			this->polarization_index = Polarization_index;
		}
	};

	class Laserfile : public Laser
	{
	private:
		std::string Laserfile;

	public:
		bool use_file() override;
		void set_laserfile(std::string Laserfile) { this->Laserfile = Laserfile; }

		std::string get_laserfile() override;

	};

	class Laserpulse_Trapezoid : public Laser
	{
	private:
		double ascention_time;
		double trapezoid(double time);
	
	public:
		void set_ascend(double ascention_time) { this->ascention_time = ascention_time; }
		std::vector<double> get_field_strengths(double time) override;
	};

	class Laserpulse_Gaussian : public Laser
	{
	private:
		double FWHM;
		int Cycle_Number;

	public:
		void set_fwhm(double FWHM) { this->FWHM = FWHM; }
		void set_cycle_number(int Cycle_Number) { this->Cycle_Number = Cycle_Number; }
		std::vector<double> get_field_strengths(double time) override;

	};

	class Laserpulse_Cosine_Squared : public Laser
	{
	private:
		int Cycle_Number;
		bool use_cyclenumber = false;

	public:
		void set_cycle_number(int cycle_number) {this->Cycle_Number = cycle_number;}
		void set_use_cycle_number(bool use_cycle_number) { this->use_cyclenumber = use_cycle_number; }
		double cyclenumber_and_frequency_to_pulse_duration();
		std::vector<double> get_field_strengths(double time) override;
	};
}
*/