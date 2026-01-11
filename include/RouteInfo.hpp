#pragma once

#include <map>
#include <string>

#include "../include/ServerData.hpp"

struct RouteResponse {
  int error_code;
  std::string error_body;
  std::string full_path;
  std::string query;
  std::string mime_type;
  size_t client_body_max;
  const std::map<int, std::string> error_pages;
  const LocationData location;
  RouteResponse();
  RouteResponse(const LocationData&, const std::map<int, std::string>&);
};

struct RouteRequest {
  const int port;
  const std::string host;
  const std::string uri;
  const std::string method;
  RouteRequest(int port, const std::string& host, const std::string& uri, const std::string& method);
};
