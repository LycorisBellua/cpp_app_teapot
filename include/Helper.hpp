#ifndef HELPER_HPP
#define HELPER_HPP

#include <string>
#include <vector>
#include <sstream>

class Helper
{
	public:
		template<typename T>
		static bool stringToUnsignedNbr(const std::string& str, T& nbr);
		static std::vector<std::string> splitAtWhitespace(const std::string&
			str);
		static std::vector<std::string> splitAtFirstColon
			(const std::string& str, bool trim);
		static std::string trimWhitespaces(const std::string& str);
		static bool insensitiveCmp(const std::string& str1, const std::string&
			str2);
		static std::string touppercase(const std::string& str);
	
	private:
		Helper();
};

template<typename T>
bool Helper::stringToUnsignedNbr(const std::string& str, T& nbr)
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

#endif
