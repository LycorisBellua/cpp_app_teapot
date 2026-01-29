#pragma once

#include <algorithm>
#include <set>
#include <sstream>
#include <string>

#include "Config.hpp"
#include "Errors_ReturnMsgs.hpp"
#include "Filesystem.hpp"
#include "Log.hpp"
#include "RequestData.hpp"
#include "RouteInfo.hpp"
#include "ServerData.hpp"
#include "Delete.hpp"
#include "Get.hpp"
#include "Post.hpp"

class Router {
 public:
  Router(const Config& conf);
  Router(const Router&);
  ~Router();

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
  const LocationData* getLocation(const std::vector<LocationData>&, const std::string&,
                                  const RequestData&) const;
  const std::string getMime(const std::string&) const;

  const ServerData* serverSearch(const std::string& host, const int port) const;
  const ServerData* serverFirstMatchingPort(const int port) const;
  const ServerData* serverFirstMatchingHost(const std::string& host) const;
  std::string decodeUri(const std::string&, const RequestData&) const;
  std::string normalizePath(const std::string&, const RequestData&) const;
  void validMethod(const RequestData&, const LocationData*) const;
  void verifyBodySize(const RequestData&, const ServerData*) const;
  RouteInfo errorReturn(int, const ServerData*, const RequestData&) const;
};
