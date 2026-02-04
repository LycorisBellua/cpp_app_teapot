#pragma once

#include <string>
#include <vector>
#include <map>
#include "LocationData.hpp"

struct ServerData
{
	int port;
	std::string host;
	std::string name;
	size_t client_body_max;
	std::map<int, std::string> errors;
	std::vector<LocationData> locations;

	ServerData();
	ServerData(const ServerData& src);
	~ServerData();
	ServerData& operator=(const ServerData& src);
};
