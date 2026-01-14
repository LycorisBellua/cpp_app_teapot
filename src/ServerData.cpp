#include "../include/ServerData.hpp"

LocationData::LocationData() : autoindex(false), redirect(0, "") {}

LocationData::LocationData(const LocationData& src)
    : path(src.path),
      allowed_methods(src.allowed_methods),
      root(src.root),
      index(src.index),
      autoindex(src.autoindex),
      upload_path(src.upload_path),
      cgi_extension(src.cgi_extension),
      cgi_interpreter(src.cgi_interpreter),
      redirect(src.redirect) {}

LocationData::~LocationData() {}

LocationData& LocationData::operator=(const LocationData& src) {
  if (this != &src) {
    this->path = src.path;
    this->allowed_methods = src.allowed_methods;
    this->root = src.root;
    this->index = src.index;
    this->autoindex = src.autoindex;
    this->upload_path = src.upload_path;
    this->cgi_extension = src.cgi_extension;
    this->cgi_interpreter = src.cgi_interpreter;
    this->redirect = src.redirect;
  }
  return *this;
}

ServerData::ServerData() : port(0), client_body_max(1000000) {}

ServerData::ServerData(const ServerData& src)
    : port(src.port), host(src.host), name(src.name), client_body_max(src.client_body_max), errors(src.errors), locations(src.locations) {}

ServerData::~ServerData() {}

ServerData& ServerData::operator=(const ServerData& src) {
  if (this != &src) {
    this->port = src.port;
    this->host = src.host;
    this->name = src.name;
    this->client_body_max = src.client_body_max;
    this->errors = src.errors;
    this->locations = src.locations;
  }
  return *this;
}
