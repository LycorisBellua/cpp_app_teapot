#pragma once

#include <sys/stat.h>

#include <algorithm>  // IWYU pragma: keep
#include <cerrno>     // IWYU pragma: keep
#include <cstdlib>
#include <exception>  // IWYU pragma: keep
#include <fstream>    // IWYU pragma: keep
#include <iostream>   // IWYU pragma: keep
#include <map>
#include <sstream>  // IWYU pragma: keep
#include <string>
#include <vector>

#include "./webserv.hpp"  // IWYU pragma: keep

enum ParseState { NONE, MIME, SERVER, LOCATION };

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
  size_t port;
  std::string host;
  size_t client_body_max;
  std::map<int, std::string> errors;
  std::vector<LocationData> locations;

  ServerData();
};

class Config {
 public:
  Config();
  Config(const std::string&);
  ~Config();

  void parse();
  const std::string& getPath() const;
  const std::map<std::string, std::string>& getMime() const;
  const std::vector<ServerData>& getServers() const;

 private:
  Config(const Config&);
  Config& operator=(const Config&);
  void setDefaultMime();
  bool parseMime(const std::vector<std::string>&, std::string&, int&);
  bool parseServer(const std::vector<std::string>&, const std::string&, std::vector<std::string>&,
                   int&);
  bool parseLocation(const std::vector<std::string>&, const std::string&, std::vector<std::string>&,
                     int&);
  void verifyRequiredData();

  const std::string conf_path;
  std::map<std::string, std::string> mime_types;
  std::vector<ServerData> servers;

  class ConfigError : public std::exception {
   public:
    ConfigError(std::string);
    ConfigError(const int&, const std::string&, const std::string);
    ~ConfigError() throw();
    const char* what() const throw();

   private:
    std::string err_msg;
  };
};
