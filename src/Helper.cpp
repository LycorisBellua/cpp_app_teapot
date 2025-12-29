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
	std::istringstream iss(str);
	std::string token;
	if (std::getline(iss, token, ':'))
	{
		if (trim)
			token = Helper::trimWhitespaces(token);
		tokens.push_back(token);
	}
	if (iss >> token)
	{
		if (trim)
			token = Helper::trimWhitespaces(token);
		tokens.push_back(token);
	}
	return tokens;
}

std::string Helper::trimWhitespaces(const std::string& str)
{
    size_t first = str.find_first_not_of(" \t\n\v\f\r");
    if (first == std::string::npos)
        return str;
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
