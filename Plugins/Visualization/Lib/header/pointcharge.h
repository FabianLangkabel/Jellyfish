#pragma once
#include <vector>
#include <map>
#include <Eigen/Dense>
#include <unsupported/Eigen/CXX11/Tensor>

namespace QC
{
	class Pointcharge
	{
	private:
		int charge;
		std::vector<double> center;

	public:
		void set_center(double x, double y, double z)
		{
			this->center = std::vector<double>(3);
			this->center[0] = x;
			this->center[1] = y;
			this->center[2] = z;
		}
		void set_charge(int charge)
		{
			this->charge = charge;
		}

		double get_center_x() { return center[0]; }
		double get_center_y() { return center[1]; }
		double get_center_z() { return center[2]; }
		int get_charge() { return charge; }
		std::vector<double> get_center() { return center; }

		Eigen::Vector3d get_center_as_vec()
		{
			Eigen::Vector3d ret;
			ret(0) = center[0];
			ret(1) = center[1];
			ret(2) = center[2];
			return ret;
		}

		double get_mass()
		{
			std::map<int, double> charge_to_atommass;
			charge_to_atommass[1] = 1.00794;
			charge_to_atommass[2] = 4.002602;
			charge_to_atommass[3] = 6.941;
			charge_to_atommass[4] = 9.012182;
			charge_to_atommass[5] = 10.811;
			charge_to_atommass[6] = 12.0107;
			charge_to_atommass[7] = 14.0067;
			charge_to_atommass[8] = 15.9994;
			charge_to_atommass[9] = 18.9984032;
			charge_to_atommass[10] = 20.1797;
			charge_to_atommass[11] = 22.98976928;
			charge_to_atommass[12] = 24.3050;
			charge_to_atommass[13] = 26.9815386;
			charge_to_atommass[14] = 28.0855;
			charge_to_atommass[15] = 30.973762;
			charge_to_atommass[16] = 32.065;
			charge_to_atommass[17] = 35.453;
			charge_to_atommass[18] = 39.0983;
			charge_to_atommass[19] = 39.948;
			charge_to_atommass[20] = 40.078;
			charge_to_atommass[21] = 44.955912;
			charge_to_atommass[22] = 47.867;
			charge_to_atommass[23] = 50.9415;
			charge_to_atommass[24] = 51.9961;
			charge_to_atommass[25] = 54.938045;
			charge_to_atommass[26] = 55.845;
			charge_to_atommass[27] = 58.933195;
			charge_to_atommass[28] = 58.6934;
			charge_to_atommass[29] = 63.546;
			charge_to_atommass[30] = 65.409;
			charge_to_atommass[31] = 69.723;
			charge_to_atommass[32] = 72.64;
			charge_to_atommass[33] = 74.92160;
			charge_to_atommass[34] = 78.96;
			charge_to_atommass[35] = 79.904;
			charge_to_atommass[36] = 83.798;
			charge_to_atommass[37] = 85.4678;
			charge_to_atommass[38] = 87.62;
			charge_to_atommass[39] = 88.90585;
			charge_to_atommass[40] = 91.224;
			charge_to_atommass[41] = 92.90638;
			charge_to_atommass[42] = 95.94;
			charge_to_atommass[43] = 98;
			charge_to_atommass[44] = 101.07;
			charge_to_atommass[45] = 102.90550;
			charge_to_atommass[46] = 106.42;
			charge_to_atommass[47] = 107.8682;
			charge_to_atommass[48] = 112.411;
			charge_to_atommass[49] = 114.818;
			charge_to_atommass[50] = 118.710;
			charge_to_atommass[51] = 121.760;
			charge_to_atommass[52] = 127.60;
			charge_to_atommass[53] = 126.90447;
			charge_to_atommass[54] = 131.293;
			charge_to_atommass[55] = 132.9054519;
			charge_to_atommass[56] = 137.327;
			charge_to_atommass[57] = 138.90547;
			charge_to_atommass[58] = 140.116;
			charge_to_atommass[59] = 140.90765;
			charge_to_atommass[60] = 144.242;
			charge_to_atommass[61] = 145;
			charge_to_atommass[62] = 150.36;
			charge_to_atommass[63] = 151.964;
			charge_to_atommass[64] = 157.25;
			charge_to_atommass[65] = 158.92535;
			charge_to_atommass[66] = 162.500;
			charge_to_atommass[67] = 164.93032;
			charge_to_atommass[68] = 167.259;
			charge_to_atommass[69] = 168.93421;
			charge_to_atommass[70] = 173.04;
			charge_to_atommass[71] = 174.967;
			charge_to_atommass[72] = 178.49;
			charge_to_atommass[73] = 180.94788;
			charge_to_atommass[74] = 183.84;
			charge_to_atommass[75] = 186.207;
			charge_to_atommass[76] = 190.23;
			charge_to_atommass[77] = 192.217;
			charge_to_atommass[78] = 195.084;
			charge_to_atommass[79] = 196.966569;
			charge_to_atommass[80] = 200.59;
			charge_to_atommass[81] = 204.3833;
			charge_to_atommass[82] = 207.2;
			charge_to_atommass[83] = 208.98040;
			charge_to_atommass[84] = 210;
			charge_to_atommass[85] = 210;
			charge_to_atommass[86] = 220;
			charge_to_atommass[87] = 223;
			charge_to_atommass[88] = 226;
			charge_to_atommass[89] = 227;
			charge_to_atommass[90] = 232.03806;
			charge_to_atommass[91] = 231.03588;
			charge_to_atommass[92] = 238.02891;
			charge_to_atommass[93] = 237;
			charge_to_atommass[94] = 244;
			charge_to_atommass[95] = 243;
			charge_to_atommass[96] = 247;
			charge_to_atommass[97] = 247;
			charge_to_atommass[98] = 251;
			charge_to_atommass[99] = 252;
			charge_to_atommass[100] = 257;
			charge_to_atommass[101] = 258;
			charge_to_atommass[102] = 259;
			charge_to_atommass[103] = 262;
			charge_to_atommass[104] = 261;
			charge_to_atommass[105] = 262;
			charge_to_atommass[106] = 266;
			charge_to_atommass[107] = 264;
			charge_to_atommass[108] = 277;
			charge_to_atommass[109] = 268;
			charge_to_atommass[110] = 271;
			charge_to_atommass[111] = 272;
			charge_to_atommass[112] = 285;
			charge_to_atommass[113] = 284;
			charge_to_atommass[114] = 289;
			charge_to_atommass[115] = 288;
			charge_to_atommass[116] = 292;
			charge_to_atommass[117] = 292;
			charge_to_atommass[118] = 294;

			return charge_to_atommass[this->charge];
		}
	};
}