#pragma once

#include <string>

struct HttpResponse {
  bool error;
  int return_code;
  std::string content;
  std::string error_message;
  HttpResponse();
  HttpResponse(bool, int, const std::string&, const std::string&);
  HttpResponse(const HttpResponse& src);
  ~HttpResponse();
  HttpResponse& operator=(const HttpResponse& src);
};
