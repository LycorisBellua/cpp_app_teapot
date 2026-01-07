#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Request.hpp"
#include "Response.hpp"
#include <string>
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
		std::string composeResponse() const;

	private:
		int fd_;
		std::time_t last_activity_;
		bool start_line_found_;
		bool end_line_found_;
		bool body_end_found_;
		bool size_zero_found_;
		bool is_size_line_;
		size_t chunk_size_;
		std::string req_buffer_;
		Request req_;

		static size_t findEndOfLine(const std::string& str);

		bool readMoreRequestData();
		void parseHeader();
		void parseBody();
		bool parseChunkedBody();
};

#endif
