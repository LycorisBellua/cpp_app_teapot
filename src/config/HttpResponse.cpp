#include "HttpResponse.hpp"

RouteRequest::RouteRequest(int error_code, int port, const std::string& host, const std::string& uri, const std::string& method,
                           const std::string& content_type, const std::string& body)
    : error_code(error_code), port(port), host(host), uri(uri), method(method), content_type(content_type), body(body) {}

HttpResponse::HttpResponse() : code(500) {}

HttpResponse::HttpResponse(int return_code, const std::string& content) : code(return_code), content(content) {}

HttpResponse::HttpResponse(const HttpResponse& src) : code(src.code), content(src.content) {}

HttpResponse::~HttpResponse() {}

HttpResponse& HttpResponse::operator=(const HttpResponse& src) {
  if (this != &src) {
    code = src.code;
    content = src.content;
  }
  return *this;
}
