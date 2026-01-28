#pragma once

#include <string>

#include "ServerData.hpp"
#include "RequestData.hpp"

struct CgiInfo {
  bool is_cgi;
  std::string script_path;
  std::string script_name;
  std::string path_info;
  std::string path_translated;
  std::string interpreter;

  CgiInfo();
};

struct RouteInfo {
  int error_code;
  std::string error_msg;
  std::string error_body;
  std::string full_path;
  std::string query;
  std::string mime_type;
  CgiInfo cgi;
  const ServerData& server;
  const LocationData& location;
  const std::map<std::string, std::string> mime_list;
  const RequestData& request;

  RouteInfo(const ServerData&, const LocationData&, const std::map<std::string, std::string>& mime, const RequestData&);
};