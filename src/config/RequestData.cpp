#include "RequestData.hpp"

RequestData::RequestData(int error_code, int port, const std::string& host, const std::string& uri, const std::string& method,
                           const std::string& content_type, const std::string& body)
    : error_code(error_code), port(port), host(host), uri(uri), method(method), content_type(content_type), body(body) {}

ResponseData::ResponseData() : code(0) {}

ResponseData::ResponseData(int return_code, const std::string& content) : code(return_code), content(content) {}

ResponseData::ResponseData(const ResponseData& src) : code(src.code), content(src.content), headers(src.headers) {}

ResponseData::~ResponseData() {}

ResponseData& ResponseData::operator=(const ResponseData& src) {
  if (this != &src) {
    code = src.code;
    content = src.content;
    headers = src.headers;
  }
  return *this;
}
