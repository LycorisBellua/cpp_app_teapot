#include "HexColorCode.hpp"
#include "Helper.hpp"

bool HexColorCode::isValid(const std::string& str)
{
	if (str.length() != 7 || str[0] != '#')
		return false;
	for (size_t i = 1; i < 7; ++i)
	{
		if (std::islower(str[i]) || !Helper::isHexChar(str[i]))
			return false;
	}
	return true;
}
