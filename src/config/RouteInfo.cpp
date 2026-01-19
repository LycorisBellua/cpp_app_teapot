#include "RouteInfo.hpp"

RouteRequest::RouteRequest(int error_code, int port, const std::string& host, const std::string& uri, const std::string& method,
                           const std::string& content_type, const std::string& body)
    : error_code(error_code), port(port), host(host), uri(uri), method(method), content_type(content_type), body(body) {}

RouteResponse::RouteResponse(const std::map<std::string, std::string>& mime, const RouteRequest& request)
    : error_code(0), mime_list(mime), request(request) {}

RouteResponse::RouteResponse(const ServerData& srv, const LocationData& loc, const std::map<std::string, std::string>& mime,
                             const RouteRequest& request)
    : error_code(0), server(srv), location(loc), mime_list(mime), request(request) {}