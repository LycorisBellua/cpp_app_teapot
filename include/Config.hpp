#pragma once

#include <sys/stat.h>

#include <algorithm>  // IWYU pragma: keep
#include <cerrno>     // IWYU pragma: keep
#include <cstdlib>    // IWYU pragma: keep
#include <exception>  // IWYU pragma: keep
#include <fstream>    // IWYU pragma: keep
#include <iostream>   // IWYU pragma: keep
#include <map>
#include <sstream>  // IWYU pragma: keep
#include <string>
#include <vector>

#include "Filesystem.hpp"  // IWYU pragma: keep
#include "Log.hpp"         // IWYU pragma: keep
#include "ServerData.hpp"

class Config {
 public:
  Config(const std::string&);
  ~Config();

  const std::string& getPath() const;
  const std::map<std::string, std::string>& getMime() const;
  const std::vector<ServerData>& getServers() const;

 private:
  // Member Data
  const std::string conf_path;
  std::vector<ServerData> servers;
  std::map<std::string, std::string> mime_types;

  // OCF Requirements
  Config();
  Config(const Config&);
  Config& operator=(const Config&);

  // Parsing State
  enum ParseState { NONE, MIME, SERVER, LOCATION };

  struct ParsingData {
    std::ifstream infile;
    std::string line;
    int line_number;
    int nest_level;
    ParseState state;
    std::vector<std::string> tokens;
    std::vector<std::string> server_processed;
    std::vector<std::string> location_processed;
    ParsingData(const std::string&);
  };

  // Exception Class
  class ConfigError : public std::exception {
   public:
    ConfigError(const std::string);
    ConfigError(const ParsingData&, const std::string);
    ~ConfigError() throw();
    const char* what() const throw();

   private:
    std::string err_msg;
  };

  // Top Level Parsing
  void parse();
  void handleNoBlock(ParsingData&);
  void handleMimeBlock(ParsingData&);
  void handleServerBlock(ParsingData&);
  void handleLocationBlock(ParsingData&);

  void parseMime(ParsingData&);

  enum ServerDirective { PORT, HOST, NAME, BODY, ERR, INVALID };
  ServerDirective strToServerDirective(const ParsingData&);
  void parseServer(ParsingData&);

  enum LocationDirective { LOC, MET, ROOT, IND, AUTOIND, UPLOAD, CGI_EXT, CGI_INT, REDIR, INVLD };
  LocationDirective strToLocationDirective(const ParsingData&);
  void parseLocation(ParsingData&);

  // Block Open/Close
  Config::ParseState validateBlockOpen(ParsingData&);
  bool validateBlockClose(ParsingData&);

  // Validate and set
  void setPort(const ParsingData&);
  void setHost(const ParsingData&);
  void setName(const ParsingData&);
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

  // Final Verification
  void verifyRequiredData();
  void verifyServer(const ServerData&) const;
  void verifyLocation(const LocationData&) const;
  void verifyVirtualHosts() const;
  void setDefaultMime();
};
