#pragma once

#include <map>
#include <string>

#include "../include/ServerData.hpp"

struct RouteInfo {
  int errcode;
  std::string full_path;
  std::string mime_type;
  size_t client_body_max;
  std::map<int, std::string>* error_pages;
  LocationData* location;
};
