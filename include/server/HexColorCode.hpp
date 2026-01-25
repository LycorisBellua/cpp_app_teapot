#ifndef HEX_COLOR_CODE_HPP
#define HEX_COLOR_CODE_HPP

#include <string>

class HexColorCode
{
	public:
		static bool isValid(const std::string& str);
		static std::string generate();

	private:
		HexColorCode();

		static bool isHexChar(char c);
};

#endif
