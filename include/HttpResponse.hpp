#pragma once

#include <string>

struct HttpResponse {
  int return_code;
  std::string content;
  std::string error_message;
  HttpResponse();
  HttpResponse(int, const std::string&, const std::string&);
  HttpResponse(const HttpResponse& src);
  ~HttpResponse();
  HttpResponse& operator=(const HttpResponse& src);
};
