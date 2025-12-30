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
		bool isFullyParsed() const;

		void updateLastActivity();
		void resetParsingData();
		bool parseRequest();

	private:
		int fd_;
		std::time_t last_activity_;
		bool start_line_found_;
		bool end_line_found_;
		bool body_end_found_;
		std::string req_buffer_;
		Request req_;

		static size_t findEndOfLine(const std::string& str);
		static std::string extractLine(std::string& str, size_t end);
		static std::vector<std::string> splitAtFirstColon(const std::string&
			str);

		bool readMoreRequestData();
		void parseHeader();
		void parseBody();
};

#endif
