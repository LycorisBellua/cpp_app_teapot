#include "RouteInfo.hpp"

RouteInfo::RouteInfo(const std::map<std::string, std::string>& mime, const RequestData& request)
  : error_code(0), mime_list(mime), request(request) {
}

RouteInfo::RouteInfo(const ServerData& srv, const LocationData& loc, const std::map<std::string, std::string>& mime,
                             const RequestData& request)
  : error_code(0), server(srv), location(loc), mime_list(mime), request(request) {
}