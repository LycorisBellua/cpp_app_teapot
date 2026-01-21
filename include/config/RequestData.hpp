#pragma once

#include <set>
#include <string>

struct ResponseData {
  int code;
  std::string code_msg; //TODO
  std::string content;
  std::string content_type; //TODO
  std::set<std::pair<std::string, std::string> > headers;

  ResponseData();

  ResponseData(int, const std::string&, const std::string&);


  ~ResponseData();

};

struct RequestData {
  int error_code; // TODO: If not 0, return error page
  int port;
  std::string host;
  std::string uri;
  std::string client_ip;
  std::string protocol;
  std::string method;
  std::string content_type;
  std::string body;

  RequestData(int error_code, int port, const std::string& host, const std::string& uri, const std::string& method, const std::string& content_type,
              const std::string& body);
};