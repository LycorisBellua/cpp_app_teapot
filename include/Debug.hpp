#pragma once

#include <cstdlib>
#include <iostream>  // IWYU pragma: keep

#include "Colors.hpp"  // IWYU pragma: keep
#include "Config.hpp"

typedef std::map<std::string, std::string>::const_iterator mime_it;
typedef std::vector<ServerData>::const_iterator serv_it;
typedef std::map<int, std::string>::const_iterator err_it;
typedef std::vector<LocationData>::const_iterator loc_it;
typedef std::vector<std::string>::const_iterator met_it;

void debugPrintConfig(const Config& conf);
void debugPrintConfig(const ConfigData& conf);
void debugPrintLn(std::string msg);
