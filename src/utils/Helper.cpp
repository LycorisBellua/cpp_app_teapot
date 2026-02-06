#include "Helper.hpp"

namespace Helper
{
	std::vector<std::string> splitAtWhitespace(const std::string& str)
	{
		std::vector<std::string> tokens;
		std::istringstream iss(str);
		std::string token;
		while (iss >> token)
			tokens.push_back(token);
		return tokens;
	}

	std::vector<std::string> splitAtChar(const std::string& str, char delimiter,
		bool trim)
	{
		std::vector<std::string> tokens;
		std::istringstream iss(str);
		std::string token;
		while (std::getline(iss, token, delimiter))
		{
			if (trim)
				token = Helper::trimWhitespaces(token);
			tokens.push_back(token);
		}
		if (!str.empty() && str[str.size() - 1] == delimiter)
			tokens.push_back("");
		return tokens;
	}

	std::vector<std::string> splitAtFirstChar(const std::string& str,
		char delimiter, bool trim)
	{
		std::vector<std::string> tokens;
		size_t del = str.find(delimiter);
		if (del == std::string::npos)
		{
			std::string token = !trim ? str : Helper::trimWhitespaces(str);
			if (token.length())
				tokens.push_back(token);
		}
		else
		{
			std::string token1 = str.substr(0, del);
			std::string token2 = str.substr(del + 1);
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

	std::string trimWhitespaces(const std::string& str)
	{
		size_t first = str.find_first_not_of(" \t\n\v\f\r");
		if (first == std::string::npos)
			return std::string();
		size_t last = str.find_last_not_of(" \t\n\v\f\r");
		return str.substr(first, last - first + 1);
	}

	std::string extractLine(std::string& str, size_t length, bool remove_crlf)
	{
		std::string line = str.substr(0, length);
		str.erase(0, length);
		if (remove_crlf)
		{
			if (str[0] == '\r' && str[1] == '\n')
				str.erase(0, 2);
			else if (str[0] == '\n')
				str.erase(0, 1);
		}
		return line;
	}

	bool insensitiveCmp(const std::string& str1, const std::string& str2)
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

	std::string touppercase(const std::string& str)
	{
		std::string res = str;
		for (size_t i = 0; i < res.length(); ++i)
			res[i] = std::toupper(static_cast<unsigned char>(res[i]));
		return res;
	}

	bool isPrintableAscii(const std::string& str)
	{
		for (size_t i = 0; i < str.length(); ++i)
		{
			unsigned char c = static_cast<unsigned char>(str[i]);
			if (c > 127 || !std::isprint(c))
				return false;
		}
		return true;
	}

	int rngMinmax(int *seed, int min, int max)
	{
		unsigned int random;

		if (!*seed)
			*seed = 1;
		random = *seed;
		random ^= random << 13;
		random ^= random >> 17;
		random ^= random << 5;
		*seed = random;
		return random % (max - min + 1) + min;
	}

	/* RFC format and GMT */
	std::string getDateRFC(const std::time_t& time)
	{
		std::tm *tm = std::gmtime(&time);
		if (!tm)
			return "";
		char buffer[64] = {0};
		std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", tm);
		return buffer;
	}

	/* ISO format and local */
	std::string getDateISO(const std::time_t& time)
	{
		std::tm* tm = std::localtime(&time);
		if (!tm)
			return "";
		char buffer[64] = {0};
		std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm);
		return buffer;
	}
}
