#pragma once

#include <string>

struct HttpResponse {
  int code;
  std::string code_msg; //TODO
  std::string content_type; //TODO
  std::string content;
  HttpResponse();
  HttpResponse(int, const std::string&);
  HttpResponse(const HttpResponse& src);
  ~HttpResponse();
  HttpResponse& operator=(const HttpResponse& src);
};

struct RouteRequest {
  int error_code;  // TODO: If not 0, return error page
  int port;
  std::string host;
  std::string uri;
  std::string method;
  std::string content_type;
  std::string body;
  RouteRequest(int error_code, int port, const std::string& host, const std::string& uri, const std::string& method, const std::string& content_type,
               const std::string& body);
};