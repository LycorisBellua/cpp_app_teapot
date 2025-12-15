#include "../include/Router.hpp"

/* ---------- Constructors / Destructor ---------- */
Router::Router(const std::vector<ServerData>& s, const std::map<std::string, std::string>& m)
    : servers(s), mime(m) {}

Router::Router(const Router& src) : servers(src.servers), mime(src.mime) {}

Router& Router::operator=(const Router& src) {
  if (this != &src) {
    this->servers = src.servers;
    this->mime = src.mime;
  }
  return *this;
}

Router::~Router() {}

/* ---------- Getters ---------- */
const std::vector<ServerData>& Router::getServers() const {
  return this->servers;
}

const std::map<std::string, std::string>& Router::getMime() const {
  return this->mime;
}
