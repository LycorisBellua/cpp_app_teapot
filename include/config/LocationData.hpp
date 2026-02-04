#pragma once

#include <string>
#include <vector>
#include <map>

struct LocationData
{
	std::string path;
	std::vector<std::string> allowed_methods;
	std::string root;
	std::string index;
	bool autoindex;
	std::string upload_path;
	std::map<std::string, std::string> cgi;
	std::pair<int, std::string> redirect;

	LocationData();
	LocationData(const LocationData& src);
	~LocationData();
	LocationData& operator=(const LocationData& src);
};
