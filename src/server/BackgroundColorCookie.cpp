#include "BackgroundColorCookie.hpp"
#include "Helper.hpp"

bool BackgroundColorCookie::isValidName(const std::string& str)
{
	return str == "background-color";
}

bool BackgroundColorCookie::isValidValue(const std::string& str)
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
