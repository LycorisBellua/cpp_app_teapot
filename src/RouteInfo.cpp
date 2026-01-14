#include "../include/RouteInfo.hpp"

RouteRequest::RouteRequest(const std::string& body) : port(0), content_length(0), body(body) {}

RouteRequest::RouteRequest(int port, const std::string& host, const std::string& uri, const std::string& method, const std::string& content_type,
                           size_t content_length, const std::string& body)
    : port(port), host(host), uri(uri), method(method), content_type(content_type), content_length(content_length), body(body) {}

RouteResponse::RouteResponse(const RouteRequest& request) : error_code(0), client_body_max(0), request(request) {}

RouteResponse::RouteResponse(const LocationData& loc, const std::map<int, std::string>& errors, const RouteRequest& request)
    : error_code(0), client_body_max(0), error_pages(errors), location(loc), request(request) {}
