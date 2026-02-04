#pragma once

#include <string>
#include <map>
#include "CgiInfo.hpp"
#include "ServerData.hpp"
#include "LocationData.hpp"
#include "RequestData.hpp"

struct RouteInfo
{
	int error_code;
	std::string error_msg;
	std::string error_body;
	std::string full_path;
	std::string query;
	std::string mime_type;
	CgiInfo cgi;
	const ServerData& server;
	const LocationData& location;
	const std::map<std::string, std::string> mime_list;
	RequestData request;

	RouteInfo(const ServerData& srv, const LocationData& loc,
		const std::map<std::string, std::string>& mime,
		const RequestData& request);
};
