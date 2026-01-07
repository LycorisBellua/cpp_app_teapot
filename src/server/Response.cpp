#include "Response.hpp"
#include "Request.hpp"
#include "Helper.hpp"
#include <sstream>

/* Public (static) ---------------------------------------------------------- */

std::string Response::getCRLF()
{
	return "\r\n";
}

std::string Response::getStartLine(int status)
{
	std::string str = getVersion();
	str += " ";
	str += Helper::nbrToString(status);
	str += " ";
	str += Request::getStatusMsg(status);
	str += getCRLF();
	return str;
}

std::string Response::getContentLengthLine(size_t length)
{
	return "Content-Length: " + Helper::nbrToString(length) + getCRLF();
}

std::string Response::getContentTypeLine(const std::string& type)
{
	return "Content-Type: " + type + getCRLF();
}

std::string Response::getConnectionCloseLine()
{
	return "Connection: close" + getCRLF();
}

/* Private (static) --------------------------------------------------------- */

std::string Response::getVersion()
{
	return "HTTP/1.1";
}
