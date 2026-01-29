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

/*
	END RESULT:

	`<body>`
		-> `<body style="background-color: #RRGGBB;">`

	`<body style="lorem: ipsum">`
		-> `<body style="background-color: #RRGGBB;lorem: ipsum">`

	`<body attr="">`
		-> `<body style="background-color: #RRGGBB;" attr="">`
*/
void HexColorCode::embedBackgroundColor(const std::string& color,
	std::string& html)
{
	size_t body_tag = findBodyTag(html);
	if (body_tag == std::string::npos)
		return;
	size_t opening_quote = insertStyleAttributeIfMissing(html, body_tag);
	html.insert(opening_quote + 1, "background-color: " + color + ";");
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

size_t HexColorCode::findBodyTag(const std::string& html)
{
	if (!Helper::insensitiveCmp("<!DOCTYPE html>", html.substr(0, 15)))
		return std::string::npos;
	return html.find("<body");
}

size_t HexColorCode::insertStyleAttributeIfMissing(std::string& html,
	size_t body_tag)
{
	size_t attribute;
	if (html[body_tag + 5] == '>'
		|| (attribute = html.find("style=", body_tag + 5)) == std::string::npos)
	{
		html.insert(body_tag + 5, " style=\"\"");
		return body_tag + 12;
	}
	return attribute + 6;
}
