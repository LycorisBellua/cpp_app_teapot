#pragma once

#include <string>

#include "RequestData.hpp"
#include "ServerData.hpp"

struct CgiInfo {
  bool is_cgi;
  pid_t pid;
  int fd_input;
  int fd_output;
  std::string output;
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
  RequestData request;

  RouteInfo(const ServerData&, const LocationData&, const std::map<std::string, std::string>& mime,
            const RequestData&);
};
