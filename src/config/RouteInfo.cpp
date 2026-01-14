#include "RouteInfo.hpp"

RouteRequest::RouteRequest(int error_code, int port, const std::string& host, const std::string& uri, const std::string& method,
                            const std::string& content_type, const std::string& body)
    : error_code(error_code), port(port), host(host), uri(uri), method(method), content_type(content_type), body(body) {}

RouteResponse::RouteResponse(const RouteRequest& request)
    : error_code(0), client_body_max(std::numeric_limits<unsigned int>::max()), request(request) {}

RouteResponse::RouteResponse(const LocationData& loc, const std::map<int, std::string>& errors, const RouteRequest& request)
    : error_code(0), client_body_max(std::numeric_limits<unsigned int>::max()), error_pages(errors), location(loc), request(request) {}
