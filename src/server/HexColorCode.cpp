#include "HexColorCode.hpp"
#include "Helper.hpp"

/* Public (Static) ---------------------------------------------------------- */

bool HexColorCode::isValid(const std::string& str)
{
	if (str.length() != 7 || str[0] != '#')
		return false;
	for (size_t i = 1; i < 7; ++i)
	{
		if (std::islower(str[i]) || !HexColorCode::isHexChar(str[i]))
			return false;
	}
	return true;
}

std::string HexColorCode::generate()
{
	static int seed = 42;
	std::string str = "#RRGGBB";
	for (size_t i = 1; i < 7; ++i)
		str[i] = "0123456789ABCDEF"[Helper::rngMinmax(&seed, 0, 15)];
	return str;
}

/* Private (Static) --------------------------------------------------------- */

bool HexColorCode::isHexChar(char c)
{
	if (std::isdigit(c))
		return true;
	else if (!std::isalpha(c))
		return false;
	c = std::toupper(static_cast<unsigned char>(c));
	return c >= 'A' && c <= 'F';
}
