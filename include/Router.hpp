#pragma once

#include <algorithm>  // IWYU pragma: keep
#include <set>
#include <sstream>  // IWYU pragma: keep
#include <string>

#include "../include/Log.hpp"  // IWYU pragma: keep
#include "../include/RouteInfo.hpp"
#include "../include/ServerData.hpp"
#include "Filesystem.hpp"  // IWYU pragma: keep

class Router {
 public:
  Router(const std::vector<ServerData>&, const std::map<std::string, std::string>&);
  Router(const Router&);
  ~Router();

  const std::vector<ServerData>& getServers() const;
  const std::set<std::pair<std::string, int> > getPorts() const;
  const RouteResponse getRoute(const RouteRequest& request) const;

 private:
  // Class Data
  const std::vector<ServerData> servers;
  const std::map<std::string, std::string> mime;

  // OCF Requirements
  Router& operator=(const Router&);

  // Exception Class
  class RouterError : public std::exception {
   public:
    RouterError(const std::string);
    RouterError(const std::string& msg, const RouteRequest& request);
    ~RouterError() throw();
    const char* what() const throw();

   private:
    std::string err_msg;
  };

  const ServerData* getServer(const RouteRequest&) const;
  const LocationData* getLocation(const std::vector<LocationData>&, const std::string&, const RouteRequest&) const;
  const std::string getMime(const std::string&) const;

  std::string decodeUri(const RouteRequest&) const;
  std::string normalizePath(const std::string&, const RouteRequest&) const;
  void validMethod(const RouteRequest&, const LocationData*) const;
  const RouteResponse errorReturn(int, const ServerData*) const;
};
