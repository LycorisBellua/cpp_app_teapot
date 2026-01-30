#pragma once

#include <set>
#include <string>
#include <map>
#include "Errors_ReturnMsgs.hpp"

struct ResponseData {
  int code;
  std::string code_msg;
  std::string content;
  std::string content_type;
  std::set<std::pair<std::string, std::string> > headers;

  ResponseData();
  ResponseData(const int error_code);
  ResponseData(const int error_code, const std::map<int, std::string>& user_errors);
  ResponseData(const int return_code, const std::string& content, const std::string& content_type);
  ~ResponseData();

};

struct RequestData {
	int client_fd;
  int error_code;
  int port;
  std::string host;
  std::string uri;
  std::string client_ip;
  std::string protocol;
  std::string method;
  std::string content_type;
  std::string body;

  RequestData(int client_fd, int error_code, int port, const std::string& host, const std::string& uri, const std::string& client_ip, const std::string& protocol, const std::string& method, const std::string& content_type,
              const std::string& body);
};
