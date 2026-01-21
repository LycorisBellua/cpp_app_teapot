#pragma once

#include <algorithm>  // IWYU pragma: keep
#include <set>
#include <sstream>  // IWYU pragma: keep
#include <string>

#include "Log.hpp"  // IWYU pragma: keep
#include "Config.hpp"
#include "RouteInfo.hpp"
#include "ServerData.hpp"
#include "ErrorPage.hpp"   // IWYU pragma: keep
#include "Filesystem.hpp"  // IWYU pragma: keep
#include "RequestData.hpp"

class Router {
 public:
  Router(const Config& conf);
  Router(const Router&);
  ~Router();

  const std::vector<ServerData>& getServers() const;
  std::set<std::pair<std::string, int> > getPorts() const;
  RouteInfo getRoute(const RequestData& request) const;

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
    RouterError(const std::string& msg, const RequestData& request);
    ~RouterError() throw();
    const char* what() const throw();

   private:
    std::string err_msg;
  };

  const ServerData* getServer(const RequestData&) const;
  const LocationData* getLocation(const std::vector<LocationData>&, const std::string&, const RequestData&) const;
  const std::string getMime(const std::string&) const;

  std::string decodeUri(const std::string&, const RequestData&) const;
  std::string normalizePath(const std::string&, const RequestData&) const;
  void validMethod(const RequestData&, const LocationData*) const;
  void verifyBodySize(const RequestData&, const ServerData*) const;
  const RouteInfo errorReturn(int, const ServerData*, const RequestData&) const;
};
