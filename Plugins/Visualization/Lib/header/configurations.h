#pragma once
#include <vector>
#include <string>

namespace QC
{
	class Configuration
	{
	public:
		std::vector<bool> alphastring;
		std::vector<bool> betastring;
		std::string print_configuration_string()
		{
			std::string resultstring = "";
			for (int i = 0; i < alphastring.size(); i++)
			{
				resultstring += std::to_string(alphastring[i]);
				resultstring += std::to_string(betastring[i]);
			}
			return resultstring;
		}
	};
}