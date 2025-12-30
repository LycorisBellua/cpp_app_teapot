#include "../include/RouteInfo.hpp"

RouteResponse::RouteResponse() : errcode(0), client_body_max(0), location(NULL) {}

RouteRequest::RouteRequest() : port(0) {}

RouteRequest::RouteRequest(int port, const std::string& host, const std::string& uri, const std::string& method)
    : port(port), host(host), uri(uri), method(method) {}
