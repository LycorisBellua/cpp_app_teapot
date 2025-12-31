#include "Helper.hpp"

std::vector<std::string> Helper::splitAtWhitespace(const std::string& str)
{
	std::vector<std::string> tokens;
	std::istringstream iss(str);
	std::string token;
	while (iss >> token)
		tokens.push_back(token);
	return tokens;
}

std::vector<std::string> Helper::splitAtFirstColon(const std::string& str,
	bool trim)
{
	std::vector<std::string> tokens;
	size_t colon = str.find(':');
	if (colon == std::string::npos)
	{
		std::string token = !trim ? str : Helper::trimWhitespaces(str);
		if (token.length())
			tokens.push_back(token);
	}
	else
	{
		std::string token1 = str.substr(0, colon);
		std::string token2 = str.substr(colon + 1);
		if (trim)
		{
			token1 = Helper::trimWhitespaces(token1);
			token2 = Helper::trimWhitespaces(token2);
		}
		tokens.push_back(token1);
		tokens.push_back(token2);
	}
	return tokens;
}

std::string Helper::trimWhitespaces(const std::string& str)
{
    size_t first = str.find_first_not_of(" \t\n\v\f\r");
    if (first == std::string::npos)
        return std::string();
    size_t last = str.find_last_not_of(" \t\n\v\f\r");
    return str.substr(first, last - first + 1);
}

bool Helper::insensitiveCmp(const std::string& str1, const std::string& str2)
{
	if (str1.length() != str2.length())
		return false;
	size_t i = 0;
	while (str1[i])
	{
		if (std::tolower(static_cast<unsigned char>(str1[i]))
			!= std::tolower(static_cast<unsigned char>(str2[i])))
			return false;
		++i;
	}
	return true;
}

std::string Helper::touppercase(const std::string& str)
{
	std::string res = str;
	for (size_t i = 0; i < res.length(); ++i)
		res[i] = std::toupper(static_cast<unsigned char>(res[i]));
	return res;
}
