#pragma once

#include <string>

struct ResponseData {
  int code;
  std::string code_msg; //TODO
  std::string content_type; //TODO
  std::string content;
  ResponseData();
  ResponseData(int, const std::string&);
  ResponseData(const ResponseData& src);
  ~ResponseData();
  ResponseData& operator=(const ResponseData& src);
};

struct RequestData {
  int error_code;  // TODO: If not 0, return error page
  int port;
  std::string host;
  std::string uri;
  std::string method;
  std::string content_type;
  std::string body;
  RequestData(int error_code, int port, const std::string& host, const std::string& uri, const std::string& method, const std::string& content_type,
               const std::string& body);
};