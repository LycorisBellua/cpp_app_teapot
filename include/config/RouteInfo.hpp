#pragma once

#include <limits>
#include <map>
#include <string>

#include "ServerData.hpp"

struct RouteRequest {
  int error_code; //TODO: If not 0, return error page
  int port;
  std::string host;
  std::string uri;
  std::string method;
  std::string content_type;
  std::string body;
  RouteRequest(int error_code, int port, const std::string& host, const std::string& uri, const std::string& method,
              const std::string& content_type, const std::string& body);
};

struct RouteResponse {
  int error_code;
  std::string error_msg;
  std::string error_body;
  std::string full_path;
  std::string query;
  std::string mime_type;
  size_t client_body_max;
  const std::map<int, std::string> error_pages;
  const LocationData location;
  const RouteRequest& request;
  RouteResponse(const RouteRequest&);
  RouteResponse(const LocationData&, const std::map<int, std::string>&, const RouteRequest&);
};
