#pragma once

#include <cstdlib>
#include <map>
#include <string>
#include <vector>

struct LocationData {
  std::string path;
  std::vector<std::string> allowed_methods;
  std::string root;
  std::string index;
  bool autoindex;
  std::string upload_path;
  std::string cgi_extension;
  std::string cgi_interpreter;
  std::pair<int, std::string> redirect;

  LocationData();
};

struct ServerData {
  int port;
  std::string host;
  size_t client_body_max;
  std::map<int, std::string> errors;
  std::vector<LocationData> locations;

  ServerData();
};
