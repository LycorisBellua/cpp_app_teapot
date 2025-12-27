#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>

struct Request
{
	int status;
	std::string method;
	std::string uri;
	std::string version;
	std::string host;
	int port;
	std::string content_type;
	size_t content_length;
	bool chunked;
	bool expect_100;
	bool close_connection;
	std::string body;
};

#endif
