#pragma once

#include <ctime>
#include <string>
#include <vector>
#include "Request.hpp"
#include "RouteInfo.hpp"
#include "RequestData.hpp"
#include "ResponseData.hpp"

class Client
{
	public:
		RouteInfo* route_info;
		ResponseData* response_data;

		Client(const std::string& ip, int fd);
		~Client();

		std::time_t getLastActivity() const;
		bool isFullyParsed() const;
		bool isBufferEmpty() const;
		bool isCgiRunning() const;
		int getCgiFdInput() const;
		int getCgiFdOutput() const;
		bool shouldCloseConnection() const;
		std::string getMethod() const;
		std::string getDomain() const;
		int getPort() const;
		RequestData getRequestData() const;
		std::string getBackgroundColor() const;
		std::vector< std::pair<std::string, std::string> > getCookies() const;

		void updateLastActivity();
		void resetParsingData();
		bool parseRequest();
		bool setBackgroundColor(const std::string& str);

	private:
		Client();

		const std::string ip_;
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
		std::string hex_bg_color_;

		static size_t findEndOfLine(const std::string& str);

		bool readMoreRequestData();
		void parseHeader();
		void parseBody();
		bool parseChunkedBody();
};
