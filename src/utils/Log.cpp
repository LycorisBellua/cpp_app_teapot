#include <ctime>
#include <iostream>
#include "Log.hpp"

namespace
{
	void writeLog(std::ostream& os, const std::string& msg_type,
		const std::string& msg);
}

namespace Log
{
	void error(const std::string& msg)
	{
		writeLog(std::cerr, "ERROR", msg);
	}

	void info(const std::string& msg)
	{
		writeLog(std::cout, "INFO", msg);
	}
}

namespace
{
	void writeLog(std::ostream& os, const std::string& msg_type,
		const std::string& msg)
	{
		const time_t now = std::time(0);
		struct tm* timeinfo = std::localtime(&now);
		char buffer[100];
		std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

		os << buffer << " " << msg_type << "\n" << msg << "\n\n";
	}
}
