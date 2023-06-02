#include "../header/laser.h"

namespace QC
{
	// returns electric field strengths (x,y,z) of Laser at time t
	std::vector<double> Laser::get_field_strengths(double time) {
		std::vector<double> output;
		switch (m_type) {
		case LaserType::Trapezoid: {

			std::vector<double> Field_Strength_Vector;
			for (int i = 0; i < 3; i++) {
				Field_Strength_Vector.push_back(0);
			}
			//double ascent = 2 * pi / omega;
			double Envelope_Function;

			if (0 <= time && time < this->ascention_time) {
				Envelope_Function = time / this->ascention_time;
			}
			else if (this->ascention_time <= time && time < this->m_pulse_duration - this->ascention_time) {
				Envelope_Function = 1;
			}
			else if (this->m_pulse_duration - this->ascention_time <= time && time < this->m_pulse_duration) {
				Envelope_Function = (this->m_pulse_duration - time) / this->ascention_time;
			}
			else {
				Envelope_Function = 0;
			}

			output = calc_fieldstrengths(time, Envelope_Function);
			break;
		}
		case LaserType::Gaussian: {
			double pi = 3.14159265358979;

			std::vector<double> Field_Strength_Vector;
			for (int i = 0; i < 3; i++) {
				Field_Strength_Vector.push_back(0);
			}

			double t0 = pi * (this->cycle_number + 0.5) / this->m_frequency;
			double Envelope_Function;

			Envelope_Function = exp(-2 * log(2) * pow((time - t0) / this->FWHM, 2));
			this->m_pulse_duration = this->FWHM;
			output = calc_fieldstrengths(time, Envelope_Function);
			break;
		}
		case LaserType::CosSquared: {
			//std::cout << "Duration " << this->m_pulse_duration << std::endl;

			double pi = 3.14159265358979;

			std::vector<double> Field_Strength_Vector;
			for (int i = 0; i < 3; i++) {
				Field_Strength_Vector.push_back(0);
			}

			if (this->use_cycle_number) {
				cyclenumber_and_frequency_to_pulse_duration();
			}
			double Envelope_Function;

			if (time < this->m_pulse_duration) {
				Envelope_Function = pow(sin(pi / this->m_pulse_duration * time), 2);
			}
			else {
				Envelope_Function = 0;
			}

			output = calc_fieldstrengths(time, Envelope_Function);
			break;
		}
		case LaserType::File: {
			break;
		}
		};
		return output;
	}


	//calculates the field strengths value (x,y,z) of Laser at time t from amplitude and envelope
	std::vector<double> Laser::calc_fieldstrengths(double time, double Envelope_Function)
	{

		std::vector<double> Field_Strength_Vector = std::vector<double>({ 0,0,0 });

		if (this->polarization_index == 0)
		{
			double Oscillation_Function;
			switch (this->oscillation_function) {
			case 0:
				Oscillation_Function = sin(this->m_frequency * (time - 0.5 * this->m_pulse_duration));
				break;
			case 1:
				Oscillation_Function = -sin(this->m_frequency * (time - 0.5 * this->m_pulse_duration));
				break;
			case 2:
				Oscillation_Function = cos(this->m_frequency * (time - 0.5 * this->m_pulse_duration));
				break;
			case 3:
				Oscillation_Function = -cos(this->m_frequency * (time - 0.5 * this->m_pulse_duration));
				break;
			default:
				Oscillation_Function = 0;
			}

			for (int i = 0; i < 3; i++) {
				if (polarization_vector[i] != 0) {
					double scale = sqrt(polarization_vector[i] * polarization_vector[i]);
					Field_Strength_Vector[i] = this->m_amplitude * scale * Oscillation_Function * Envelope_Function;
				}
			}
		}
		else
		{
			double Oscillation_Function1;
			double Oscillation_Function2;
			if (polarization_index == 1)// r.h. circular z->(x,y) | phi_y = phi_x-90�
			{
				switch (this->oscillation_function) {
				case 0:
					Oscillation_Function1 = sin(this->m_frequency * (time - 0.5 * this->m_pulse_duration));
					Oscillation_Function2 = -cos(this->m_frequency * (time - 0.5 * this->m_pulse_duration));
					break;
				case 1:
					Oscillation_Function1 = -sin(this->m_frequency * (time - 0.5 * this->m_pulse_duration));
					Oscillation_Function2 = cos(this->m_frequency * (time - 0.5 * this->m_pulse_duration));
					break;
				case 2:
					Oscillation_Function1 = cos(this->m_frequency * (time - 0.5 * this->m_pulse_duration));
					Oscillation_Function2 = sin(this->m_frequency * (time - 0.5 * this->m_pulse_duration));
					break;
				case 3:
					Oscillation_Function1 = -cos(this->m_frequency * (time - 0.5 * this->m_pulse_duration));
					Oscillation_Function2 = -sin(this->m_frequency * (time - 0.5 * this->m_pulse_duration));
					break;
				}
			}
			else { // l.h. circular z->(x,y) | phi_y = phi_x +90�
				switch (this->oscillation_function) {
				case 0:
					Oscillation_Function1 = sin(this->m_frequency * (time - 0.5 * this->m_pulse_duration));
					Oscillation_Function2 = cos(this->m_frequency * (time - 0.5 * this->m_pulse_duration));
					break;
				case 1:
					Oscillation_Function1 = -sin(this->m_frequency * (time - 0.5 * this->m_pulse_duration));
					Oscillation_Function2 = -cos(this->m_frequency * (time - 0.5 * this->m_pulse_duration));
					break;
				case 2:
					Oscillation_Function1 = cos(this->m_frequency * (time - 0.5 * this->m_pulse_duration));
					Oscillation_Function2 = -sin(this->m_frequency * (time - 0.5 * this->m_pulse_duration));
					break;
				case 3:
					Oscillation_Function1 = -cos(this->m_frequency * (time - 0.5 * this->m_pulse_duration));
					Oscillation_Function2 = sin(this->m_frequency * (time - 0.5 * this->m_pulse_duration));
					break;
				}
			}
			//circular z->(x,y), y->(z,x), x->(y,z) -> phi, theta -> (phi,theta+90), (phi+90,theta+90)
			double incidentx = this->polarization_vector[0];
			double incidenty = this->polarization_vector[1];
			double incidentz = this->polarization_vector[2];


			if (incidentz * incidentz == 1.0) {
				Field_Strength_Vector[0] = incidentz * this->m_amplitude * Envelope_Function * (Oscillation_Function1);
				Field_Strength_Vector[1] = incidentz * this->m_amplitude * Envelope_Function * (Oscillation_Function2);
				Field_Strength_Vector[2] = 0;

			}
			else {
				double norm1 = sqrt(incidentx * incidentx + incidenty * incidenty);
				double norm2 = sqrt(((incidentx * incidentz) * (incidentx * incidentz)) + ((incidenty * incidentz) * (incidenty * incidentz)) + ((incidentx * incidentx + incidenty * incidenty) * (incidentx * incidentx + incidenty * incidenty)));

				Field_Strength_Vector[0] = -this->m_amplitude * Envelope_Function * (-incidenty / norm1 * Oscillation_Function1 - (incidentx * incidentz) / norm2 * Oscillation_Function2);
				Field_Strength_Vector[1] = -this->m_amplitude * Envelope_Function * (incidentx / norm1 * Oscillation_Function1 - (incidenty * incidentz) / norm2 * Oscillation_Function2);
				Field_Strength_Vector[2] = -this->m_amplitude * Envelope_Function * ((incidentx * incidentx + incidenty * incidenty) / norm2 * Oscillation_Function2);
			}
		}

		return Field_Strength_Vector;
	}


	double Laser::cyclenumber_and_frequency_to_pulse_duration()
	{
		double pi = 3.14159265358979;
		this->m_pulse_duration = 2 * pi * (double)this->cycle_number / this->m_frequency;
		return this->m_pulse_duration;
	}
}
