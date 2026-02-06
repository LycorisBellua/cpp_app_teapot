#include <iostream>
#include "Log.hpp"
#include "Helper.hpp"

namespace
{
	void writeLog(std::ostream& stream, const std::string& msg_type,
		const std::string& msg);
}

namespace Log
{
	void info(const std::string& msg)
	{
		writeLog(std::cout, "INFO", msg);
	}

	void error(const std::string& msg)
	{
		writeLog(std::cerr, "ERROR", msg);
	}
}

namespace
{
	void writeLog(std::ostream& stream, const std::string& msg_type,
		const std::string& msg)
	{
		stream << Helper::getDateISO(std::time(0)) << " " << msg_type << "\n"
			<< msg << "\n\n";
	}
}
