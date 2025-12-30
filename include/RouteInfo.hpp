#pragma once

#include <map>
#include <string>

#include "../include/ServerData.hpp"

struct RouteResponse {
  int errcode;
  std::string full_path;
  std::string mime_type;
  size_t client_body_max;
  const std::map<int, std::string>* error_pages;
  const LocationData* location;
  RouteResponse();
};

struct RouteRequest {
  const int port;
  const std::string host;
  const std::string uri;
  const std::string method;
  RouteRequest();
  RouteRequest(int port, const std::string& host, const std::string& uri, const std::string& method);
};
