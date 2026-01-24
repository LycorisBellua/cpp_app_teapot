#ifndef HELPER_HPP
#define HELPER_HPP

#include <string>
#include <vector>
#include <sstream>

class Helper
{
	public:
		template<typename T>
		static bool decToUnsignedNbr(const std::string& str, T& nbr);
		template<typename T>
		static bool hexToUnsignedNbr(const std::string& str, T& nbr);
		template<typename T>
		static std::string nbrToString(T nbr);

		static std::vector<std::string> splitAtWhitespace(const std::string&
			str);
		static std::vector<std::string> splitAtChar(const std::string& str,
			char delimiter, bool trim);
		static std::vector<std::string> splitAtFirstChar(const std::string& str,
			char delimiter, bool trim);
		static std::string trimWhitespaces(const std::string& str);
		static std::string extractLine(std::string& str, size_t length,
			bool remove_crlf);
		static bool insensitiveCmp(const std::string& str1, const std::string&
			str2);
		static std::string touppercase(const std::string& str);
		static bool isPrintableAscii(const std::string& str);
		static bool isHexChar(char c);
	
	private:
		Helper();
};

template<typename T>
bool Helper::decToUnsignedNbr(const std::string& str, T& nbr)
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
bool Helper::hexToUnsignedNbr(const std::string& str, T& nbr)
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
std::string Helper::nbrToString(T nbr)
{
	std::ostringstream oss;
	if (!(oss << nbr))
		return "";
	return oss.str();
}

#endif
