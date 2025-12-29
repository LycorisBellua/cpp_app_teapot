#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Request.hpp"
#include <string>
#include <vector>
#include <ctime>

class Client
{
	public:
		Client();
		Client(int fd);

		std::time_t getLastActivity() const;
		bool getIsParsed() const;

		void updateLastActivity();
		bool parseRequest();

	private:
		int fd_;
		std::time_t last_activity_;
		std::time_t req_start_;
		std::string req_buffer_;
		bool header_parsed_;
		bool body_parsed_;
		Request req_;

		static size_t findEndOfLine(const std::string& str);
		static std::string extractLine(std::string& str, size_t end);
		static std::vector<std::string> splitAtFirstColon(const std::string&
			str);

		bool parseHeader();
		bool parseBody();
		bool readMoreRequestData();
		bool isRequestTooSlow() const;
};

#endif
