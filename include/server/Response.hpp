#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>

class Response
{
	public:
		static std::string getCRLF();
		static std::string getStartLine(int status);
		static std::string getDateLine();
		static std::string getContentLengthLine(size_t length);
		static std::string getContentTypeLine(const std::string& type);
		static std::string getConnectionCloseLine();

	private:
		Response();

		static std::string getVersion();
		static std::string getCurrentDateGMT();
};

#endif
