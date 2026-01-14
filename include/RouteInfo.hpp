#pragma once

#include <limits>
#include <map>
#include <string>

#include "../include/ServerData.hpp"

struct RouteRequest {
  int port;
  std::string host;
  std::string uri;
  std::string method;
  std::string content_type;
  size_t content_length;
  const std::string& body;
  RouteRequest(const std::string& body);
  RouteRequest(int port, const std::string& host, const std::string& uri, const std::string& method, const std::string& content_type,
               size_t content_length, const std::string& body);
};

struct RouteResponse {
  int error_code;
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
