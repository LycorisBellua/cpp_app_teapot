#pragma once

#include <string>

struct HttpResponse {
  int code;
  std::string content;
  HttpResponse();
  HttpResponse(int, const std::string&);
  HttpResponse(const HttpResponse& src);
  ~HttpResponse();
  HttpResponse& operator=(const HttpResponse& src);
};
