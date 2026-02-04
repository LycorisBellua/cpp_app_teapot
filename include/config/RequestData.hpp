#pragma once

#include <string>

struct RequestData
{
	int client_fd;
	int error_code;
	int port;
	std::string host;
	std::string uri;
	std::string client_ip;
	std::string protocol;
	std::string method;
	std::string content_type;
	std::string body;

	RequestData(int client_fd, int error_code, int port, const std::string& host, const std::string& uri, const std::string& client_ip, const std::string& protocol, const std::string& method, const std::string& content_type,
			const std::string& body);
};
