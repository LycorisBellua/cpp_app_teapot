#pragma once

#include <string>

#include "ServerData.hpp"
#include "RequestData.hpp"

// TODO: Make server and location const references
struct RouteInfo {
  int error_code;
  std::string error_msg;
  std::string error_body;
  std::string full_path;
  std::string query;
  std::string mime_type;
  std::string path_info;
  std::string path_translated;
  std::string script_name;
  const ServerData& server;
  const LocationData& location;
  const std::map<std::string, std::string> mime_list;
  const RequestData& request;

  RouteInfo(const ServerData&, const LocationData&, const std::map<std::string, std::string>& mime, const RequestData&);
};