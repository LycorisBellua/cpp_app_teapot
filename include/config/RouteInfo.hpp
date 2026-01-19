#pragma once

#include <limits>
#include <string>

#include "ServerData.hpp"
#include "HttpResponse.hpp"

// TODO: Make server and location const references
struct RouteInfo {
  int error_code;
  std::string error_msg;
  std::string error_body;
  std::string full_path;
  std::string query;
  std::string mime_type;
  ServerData server;
  LocationData location;
  const std::map<std::string, std::string> mime_list;
  const RouteRequest& request;

  RouteInfo(const std::map<std::string, std::string>& mime, const RouteRequest&);

  RouteInfo(const ServerData&, const LocationData&, const std::map<std::string, std::string>& mime, const RouteRequest&);
};