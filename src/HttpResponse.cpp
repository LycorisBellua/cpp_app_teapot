#include "../include/HttpResponse.hpp"

HttpResponse::HttpResponse() : error(true), return_code(500) {}

HttpResponse::HttpResponse(bool error, int return_code, const std::string& content, const std::string& error_message)
    : error(error), return_code(return_code), content(content), error_message(error_message) {}

HttpResponse::HttpResponse(const HttpResponse& src)
    : error(src.error), return_code(src.return_code), content(src.content), error_message(src.error_message) {}

HttpResponse::~HttpResponse() {}

HttpResponse& HttpResponse::operator=(const HttpResponse& src) {
  if (this != &src) {
    error = src.error;
    return_code = src.return_code;
    content = src.content;
    error_message = src.error_message;
  }
  return *this;
}
