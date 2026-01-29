#include "Router.hpp"

namespace {

  typedef std::vector<ServerData>::const_iterator srv_it;
  typedef std::vector<LocationData>::const_iterator loc_it;
  typedef std::vector<std::string>::const_iterator strvec_it;
  typedef std::map<std::string, std::string>::const_iterator mime_it;

  bool isHexChar(int c) {
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
  }

  char decodeHex(const std::string& hex) {
    return static_cast<char>(std::strtol(hex.c_str(), NULL, 16));
  }

  std::string removeQuery(const std::string& uri) {
    size_t start = uri.find_first_of("?");

    return (start == uri.npos) ? uri : uri.substr(0, start);
  }

  std::string getQuery(const std::string& uri) {
    size_t query_start = uri.find_first_of("?");
    if (query_start == uri.npos) {
      return "";
    }
    return uri.substr(query_start + 1);
  }

  bool methodImplemented(const std::string& method) {
    return method == "GET" || method == "POST" || method == "DELETE" || method == "HEAD";
  }



  CgiInfo splitCgiPath(const std::string& path, const std::map<std::string, std::string>& cgi_interpreters) {
    CgiInfo result;
    result.script_path = path;

    if (cgi_interpreters.empty()) {
      return result;
    }

    // Try each registered CGI extension
    std::map<std::string, std::string>::const_iterator it;
    for (it = cgi_interpreters.begin(); it != cgi_interpreters.end(); ++it) {
      const std::string& ext = it->first;

      size_t ext_pos = path.find(ext);
      if (ext_pos == std::string::npos) {
        continue;
      }

      size_t script_end = ext_pos + ext.length();

      // Must be at end of path or followed by '/'
      if (script_end < path.length() && path[script_end] != '/') {
        continue;  // Try next extension
      }

      result.script_path = path.substr(0, script_end);
      result.interpreter = it->second;  // Store the interpreter
      result.is_cgi = true;
      if (script_end < path.length()) {
        result.path_info = path.substr(script_end);
      }

      return result;
    }

    // No CGI extension found
    return result;
  }

  void logSuccess(const RequestData& req, const RouteInfo& route) {
    std::stringstream log_str;
    log_str << "[Router] Successfully matched request to Server/Location\n\nRequest:" << "\nPort: "
            << req.port << "\nHost: " << req.host << "\nURI: " << req.uri
            << "\nMethod: " << req.method << "\n\nRoute:" << "\nFull Path: " << route.full_path
            << "\nMime Type: " << route.mime_type << "\nLocation: " << route.location.path;
    Log::info(log_str.str());
  }

}

/* ---------- Constructors / Destructor ---------- */
Router::Router(const Config& conf) : servers(conf.getServers()), mime(conf.getMime()) {}

Router::Router(const Router& src) : servers(src.servers), mime(src.mime) {}

Router::~Router() {}

/* ---------- EXCEPTION ---------- */
Router::RouterError::RouterError(const std::string msg) {
  err_msg = "Router Error:\n" + msg;
  Log::error(err_msg);
}

Router::RouterError::RouterError(const std::string& msg, const RequestData& req) {
  std::stringstream err_string;
  err_string << "[Router] " << msg << "\nPort: " << req.port << "\nHost: " << req.host
             << "\nURI: " << req.uri << "\nMethod: " << req.method;
  err_msg = err_string.str();
  Log::error(err_string.str());
}

Router::RouterError::~RouterError() throw() {}

const char* Router::RouterError::what() const throw() {
  return err_msg.c_str();
}

/* ---------- Getters ---------- */
std::set<std::pair<std::string, int> > Router::getPorts() const {
  std::set<std::pair<std::string, int> > ports;
  for (srv_it s = servers.begin(); s != servers.end(); ++s) {
    ports.insert(std::make_pair(s->host, s->port));
  }
  return ports;
}

ResponseData Router::handle(const RequestData& request) const {
  if (request.error_code != 0) {
    return ResponseData(request.error_code);
  }
  const RouteInfo& data = getRoute(request);
  if (data.error_code != 0) {
    return (data.error_code == 400 || data.error_code == 404) ? ResponseData(data.error_code)
                                    : ResponseData(data.error_code, data.server.errors);
  }
  if (request.method == "GET" || request.method == "HEAD") {
    return Get::handle(data);
  }
  if (request.method == "POST") {
    return Post::handle(data);
  }
  if (request.method == "DELETE") {
    return Delete::handle(data);
  }
  return ResponseData(500);
}

/* ---------- Private ---------- */
RouteInfo Router::getRoute(const RequestData& request) const {
  const std::string query = getQuery(request.uri);
  std::string path;
  try {
    path = removeQuery(request.uri);
    const std::string decoded_path = decodeUri(path, request);
    path = normalizePath(decoded_path, request);
  } catch (const RouterError& e) {
    return errorReturn(400, NULL, request);
  }
  const ServerData* server = NULL;
  const LocationData* location = NULL;
  try {
    server = getServer(request);
    location = getLocation(server->locations, path, request);
  } catch (const RouterError& e) {
    return errorReturn(404, server, request);
  }
  try {
    validMethod(request, location);
  } catch (const RouterError& e) {
    return errorReturn(405, server, request);
  }
  try {
    verifyBodySize(request, server);
  } catch (const RouterError& e) {
    return errorReturn(413, server, request);
  }
  RouteInfo route(*server, *location, mime, request);
  if (location->redirect.first != 0) {
    route.error_code = location->redirect.first;
    return route;
  }
  CgiInfo cgi = splitCgiPath(path, location->cgi);
  route.full_path = Filesystem::normalisePaths(location->root + cgi.script_path.substr(1),
                                               Filesystem::getCurrentDir());
  route.query = query;
  route.mime_type = getMime(cgi.script_path);
  if (!cgi.path_info.empty()) {
    cgi.path_translated = Filesystem::normalisePaths(
        location->root + cgi.path_info.substr(1), Filesystem::getCurrentDir());
  }
  cgi.script_name = cgi.script_path;
  route.cgi = cgi;
  logSuccess(request, route);
  return route;
}

const ServerData* Router::getServer(const RequestData& request) const {
  const ServerData* result = NULL;
  if (!request.host.empty() && request.port != 0) {
    result = serverSearch(request.host, request.port);
    if (result) {
      return result;
    }
    result = serverSearch("0.0.0.0", request.port);
    if (result) {
      return result;
    }
  }
  if (request.host.empty() && request.port != 0) {
    result = serverFirstMatchingPort(request.port);
    if (result) {
      return result;
    }
  }
  if (!request.host.empty() && request.port == 0) {
    result = serverFirstMatchingHost(request.host);
    if (result) {
      return result;
    }
  }
  if (request.host.empty() && request.port == 0 && !servers.empty()) {
    return &servers.front();
  }
  throw RouterError("No Matching server found for request", request);
}

const LocationData* Router::getLocation(const std::vector<LocationData>& locations,
                                        const std::string& path, const RequestData& request) const {
  loc_it match = locations.end();
  for (loc_it loc = locations.begin(); loc != locations.end(); ++loc) {
    if (path.compare(0, loc->path.length(), loc->path) == 0) {
      // Check boundary: must be exact match, followed by '/', or location is root
      if (path.length() == loc->path.length() || path[loc->path.length()] == '/' ||
          loc->path == "/") {
        match =
            (match == locations.end() || (loc->path.length() > match->path.length())) ? loc : match;
      }
    }
  }
  if (match == locations.end()) {
    throw RouterError("Unable to find matching location", request);
  }
  return &(*match);
}

const std::string Router::getMime(const std::string& path) const {
  size_t ext_start = path.find_last_of('.');
  mime_it found = mime.end();
  if (ext_start != path.npos) {
    found = mime.find(path.substr(ext_start));
  }
  return found == mime.end() ? "application/octet-stream" : found->second;
}

const ServerData* Router::serverSearch(const std::string& host, const int port) const {
  for (srv_it server = servers.begin(); server != servers.end(); ++server) {
    if (server->host == host && server->port == port) {
      return &(*server);
    }
  }
  return NULL;
}

const ServerData* Router::serverFirstMatchingPort(const int port) const {
  for (srv_it server = servers.begin(); server != servers.end(); ++server) {
    if (server->port == port) {
      return &(*server);
    }
  }
  return NULL;
}

const ServerData* Router::serverFirstMatchingHost(const std::string& host) const {
  for (srv_it server = servers.begin(); server != servers.end(); ++server) {
    if (server->host == host) {
      return &(*server);
    }
  }
  return NULL;
}

std::string Router::decodeUri(const std::string& uri_no_query, const RequestData& req) const {
  const std::string& uri = uri_no_query;
  std::string result;

  for (size_t i = 0; i < uri.size(); ++i) {
    if (uri[i] == '%' && i + 2 < uri.size()) {
      if (isHexChar(uri[i + 1]) && isHexChar(uri[i + 2])) {
        char decoded = decodeHex(uri.substr(i + 1, 2));
        if (decoded == '\0') {
          throw RouterError("Null byte in URI", req);
        }
        if (decoded > 0 && decoded < 32) {
          throw RouterError("Control character in URI", req);
        }
        if (decoded == 127) {
          throw RouterError("Invalid character in URI", req);
        }
        result += decoded;
        i += 2;
      }
      else {
        throw RouterError("Invalid Hex Characters in URI", req);
      }
    }
    else if (uri[i] == '%') {
      throw RouterError("Invalid Hex Encoding", req);
    }
    else {
      result.push_back(uri[i]);
    }
  }
  return result;
}

std::string Router::normalizePath(const std::string& path, const RequestData& request) const {
  if (path.empty() || path[0] != '/') {
    throw RouterError("Path must start with /", request);
  }

  std::vector<std::string> segments;
  std::string segment;

  // Split path into segments
  for (size_t i = 1; i < path.size(); ++i) {
    if (path[i] == '/') {
      if (!segment.empty()) {
        segments.push_back(segment);
        segment.clear();
      }
      // Skip empty segments (duplicate slashes)
    }
    else {
      segment += path[i];
    }
  }
  // Add last segment if not empty
  if (!segment.empty()) {
    segments.push_back(segment);
  }

  // Process segments to resolve . and ..
  std::vector<std::string> normalized;
  for (size_t i = 0; i < segments.size(); ++i) {
    if (segments[i] == ".") {
      // Skip current directory references
      continue;
    }
    else if (segments[i] == "..") {
      // Go up one directory
      if (!normalized.empty()) {
        normalized.pop_back();
      }
      // If normalized is empty, we're trying to escape root - ignore it
    }
    else {
      normalized.push_back(segments[i]);
    }
  }

  // Rebuild path
  std::string result = "/";
  for (size_t i = 0; i < normalized.size(); ++i) {
    result += normalized[i];
    if (i + 1 < normalized.size()) {
      result += "/";
    }
  }

  return result;
}

void Router::validMethod(const RequestData& req, const LocationData* location) const {
  const std::vector<std::string>& am = location->allowed_methods;
  strvec_it method = std::find(am.begin(), am.end(), req.method);

  bool is_valid = (method != am.end()) || (am.empty() && methodImplemented(req.method));
  if (!is_valid) {
    throw RouterError("Invalid Method for matched location", req);
  }
}

void Router::verifyBodySize(const RequestData& request, const ServerData* server) const {
  if (request.body.size() > server->client_body_max) {
    throw RouterError("Body Size too large for matched server", request);
  }
}

RouteInfo Router::errorReturn(int code, const ServerData* srv, const RequestData& req) const {
  if (srv) {
    RouteInfo response(*srv, LocationData(), std::map<std::string, std::string>(), req);
    response.error_code = code;
    return response;
  }
  else {
    RouteInfo response(ServerData(), LocationData(), std::map<std::string, std::string>(), req);
    response.error_code = code;
    return response;
  }
}
