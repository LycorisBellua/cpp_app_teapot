#ifndef BACKGROUNDCOLORCOOKIE_HPP
#define BACKGROUNDCOLORCOOKIE_HPP

#include <string>

class BackgroundColorCookie
{
	public:
		static bool isValidName(const std::string& str);
		static bool isValidValue(const std::string& str);

	private:
		BackgroundColorCookie();
};

#endif
