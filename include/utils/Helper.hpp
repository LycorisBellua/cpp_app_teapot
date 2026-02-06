#pragma once

#include <ctime>
#include <string>
#include <sstream>
#include <vector>

namespace Helper
{
	template<typename T>
	bool decToUnsignedNbr(const std::string& str, T& nbr);

	template<typename T>
	bool hexToUnsignedNbr(const std::string& str, T& nbr);

	template<typename T>
	std::string nbrToString(T nbr);

	std::vector<std::string> splitAtWhitespace(const std::string& str);
	std::vector<std::string> splitAtChar(const std::string& str, char delimiter,
		bool trim);
	std::vector<std::string> splitAtFirstChar(const std::string& str,
		char delimiter, bool trim);
	std::string trimWhitespaces(const std::string& str);
	std::string extractLine(std::string& str, size_t length, bool remove_crlf);
	bool insensitiveCmp(const std::string& str1, const std::string& str2);
	std::string touppercase(const std::string& str);
	bool isPrintableAscii(const std::string& str);
	int rngMinmax(int *seed, int min, int max);
	std::string getDateRFC(const std::time_t& time);
	std::string getDateISO(const std::time_t& time);

	/* Template definitions ------------------------------------------------- */

	template<typename T>
	bool decToUnsignedNbr(const std::string& str, T& nbr)
	{
		nbr = 0;
		if (!std::isdigit(str[0]))
			return false;
		std::istringstream iss(str);
		char extra;
		if (!(iss >> nbr) || (iss >> extra))
		{
			nbr = 0;
			return false;
		}
		return true;
	}

	template<typename T>
	bool hexToUnsignedNbr(const std::string& str, T& nbr)
	{
		nbr = 0;
		if (!std::isalnum(str[0]))
			return false;
		std::stringstream ss;
		ss << std::hex << str;
		char extra;
		if (!(ss >> nbr) || (ss >> extra))
		{
			nbr = 0;
			return false;
		}
		return true;
	}

	template<typename T>
	std::string nbrToString(T nbr)
	{
		std::ostringstream oss;
		if (!(oss << nbr))
			return "";
		return oss.str();
	}
}
