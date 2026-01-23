#ifndef HEX_COLOR_CODE_HPP
#define HEX_COLOR_CODE_HPP

#include <string>

class HexColorCode
{
	public:
		static bool isValid(const std::string& str);

	private:
		HexColorCode();
};

#endif
