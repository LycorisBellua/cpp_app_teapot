#include "../include/Router.hpp"

namespace {

  typedef std::vector<ServerData>::const_iterator srv_it;
  typedef std::vector<LocationData>::const_iterator loc_it;

  std::string removeQueryAndFragment(const std::string& uri) {
    size_t start = uri.find_first_of("?#");

    return (start == uri.npos) ? uri : uri.substr(0, start);
  }

}

/* ---------- Constructors / Destructor ---------- */
Router::Router(const std::vector<ServerData>& s, const std::map<std::string, std::string>& m)
    : servers(s), mime(m) {}

Router::Router(const Router& src) : servers(src.servers), mime(src.mime) {}

Router::~Router() {}

/* ---------- Getters ---------- */
const std::vector<ServerData>& Router::getServers() const {
  return this->servers;
}

// TODO: remove or make private, not needed by server
const std::map<std::string, std::string>& Router::getMime() const {
  return this->mime;
}

const RouteResponse Router::getRoute(const RouteRequest& request) const {
  std::string path = removeQueryAndFragment(request.uri);
  const ServerData* server = getServer(request.port, request.host);
  const LocationData* location = getLocation(server->locations, path);
  RouteResponse route;
  route.full_path = location->root + request.uri;
  route.client_body_max = server->client_body_max;
  route.error_pages = &server->errors;
  route.location = location;
  return route;
}

/* ---------- Private ---------- */
const ServerData* Router::getServer(int port, const std::string& host) const {
  for (srv_it srv = servers.begin(); srv != servers.end(); ++srv) {
    if (srv->port == port && (srv->host.empty() || srv->host == host)) {
      return &(*srv);
    }
  }
  return NULL;
}

const LocationData* Router::getLocation(const std::vector<LocationData>& locations,
                                        const std::string& path) const {
  for (loc_it loc = locations.begin(); loc != locations.end(); ++loc) {
    if (loc->path == path) {
      return &(*loc);
    }
  }
  return NULL;
}
