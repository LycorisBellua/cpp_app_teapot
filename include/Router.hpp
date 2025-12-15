#pragma once

#include "../include/ServerData.hpp"

class Router {
 public:
  Router(const std::vector<ServerData>&, const std::map<std::string, std::string>&);
  Router(const Router&);
  Router& operator=(const Router&);
  ~Router();

  const std::vector<ServerData>& getServers() const;
  const std::map<std::string, std::string>& getMime() const;

 private:
  // Class Data
  std::vector<ServerData> servers;
  std::map<std::string, std::string> mime;

  // OCF Requirements
};
