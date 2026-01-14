#include "HttpResponse.hpp"

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
