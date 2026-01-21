#include "RequestData.hpp"

RequestData::RequestData(int error_code, int port, const std::string& host, const std::string& uri, const std::string& method,
                           const std::string& content_type, const std::string& body)
    : error_code(error_code), port(port), host(host), uri(uri), method(method), content_type(content_type), body(body) {}

ResponseData::ResponseData() : code(0) {}

ResponseData::ResponseData(const int return_code, const std::string& code_msg, const std::string& content) : code(return_code), code_msg(code_msg), content(content) {}

ResponseData::~ResponseData() {}