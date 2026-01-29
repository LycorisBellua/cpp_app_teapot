#ifndef HEX_COLOR_CODE_HPP
#define HEX_COLOR_CODE_HPP

#include <string>

class HexColorCode
{
	public:
		static bool isValid(const std::string& str);
		static std::string generate();
		static void embedBackgroundColor(const std::string& color,
			std::string& html);

	private:
		HexColorCode();

		static bool isHexChar(char c);
		static size_t findBodyTag(const std::string& html);
		static size_t insertStyleAttributeIfMissing(std::string& html,
			size_t body_tag);
};

#endif
