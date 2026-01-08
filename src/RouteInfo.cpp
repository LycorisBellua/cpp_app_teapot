#include "../include/RouteInfo.hpp"

RouteResponse::RouteResponse() : error_code(0), client_body_max(0) {}

RouteResponse::RouteResponse(const LocationData& loc, const std::map<int, std::string>& errors)
    : error_code(0), client_body_max(0), error_pages(errors), location(loc) {}

RouteRequest::RouteRequest(int port, const std::string& host, const std::string& uri, const std::string& method)
    : port(port), host(host), uri(uri), method(method) {}
