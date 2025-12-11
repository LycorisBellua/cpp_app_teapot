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

#include "../include/ServerData.hpp"
#include "./webserv.hpp"  // IWYU pragma: keep

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
  const std::string conf_path;
  std::map<std::string, std::string> mime_types;
  std::vector<ServerData> servers;

  enum ParseState { NONE, MIME, SERVER, LOCATION };

  struct ParsingData {
    std::ifstream infile;
    std::string line;
    int line_number;
    ParseState state;
    std::vector<std::string> tokens;
    std::vector<std::string> server_processed;
    std::vector<std::string> location_processed;
    ParsingData(const std::string&);
  };

  class ConfigError : public std::exception {
   public:
    ConfigError(const std::string);
    ConfigError(const ParsingData&, const std::string);
    ~ConfigError() throw();
    const char* what() const throw();

   private:
    std::string err_msg;
  };

  Config(const Config&);
  Config& operator=(const Config&);

  bool parseMime(ParsingData&);
  bool parseServer(ParsingData&);
  bool parseLocation(ParsingData&);

  Config::ParseState validateBlockOpen(const std::vector<std::string>& tokens);
  bool validateBlockClose(const std::vector<std::string>& tokens);

  void setPort(const ParsingData&);
  void setHost(const ParsingData&);
  void setBodySize(const ParsingData&);
  void setErrorPage(const ParsingData&);
  void setPath(const ParsingData&);
  void setMethods(const ParsingData&);
  void setRoot(const ParsingData&);
  void setIndex(const ParsingData&);
  void setAutoIndex(const ParsingData&);
  void setUploadPath(const ParsingData&);
  void setCgiExtension(const ParsingData&);
  void setCgiInterpreter(const ParsingData&);
  void setRedirect(const ParsingData&);

  void verifyRequiredData();
  void setDefaultMime();
};
