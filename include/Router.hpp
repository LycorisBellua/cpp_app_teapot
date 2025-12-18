#pragma once

#include "../include/RouteInfo.hpp"
#include "../include/ServerData.hpp"

class Router {
 public:
  Router(const std::vector<ServerData>&, const std::map<std::string, std::string>&);
  Router(const Router&);
  ~Router();

  const std::vector<ServerData>& getServers() const;
  const std::map<std::string, std::string>& getMime() const;
  const RouteResponse getRoute(const RouteRequest& request) const;

 private:
  // Class Data
  std::vector<ServerData> servers;
  std::map<std::string, std::string> mime;

  // OCF Requirements
  Router& operator=(const Router&);

  const ServerData* getServer(int, const std::string&) const;
  const LocationData* getLocation(const std::vector<LocationData>&, const std::string&) const;
};
