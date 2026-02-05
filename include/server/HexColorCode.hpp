#pragma once

#include <string>

namespace HexColorCode
{
	bool isValid(const std::string& str);
	std::string generate();
	void embedBackgroundColor(const std::string& color, std::string& html);
}
