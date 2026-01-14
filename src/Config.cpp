#include "../include/Config.hpp"

/* ---------- CONSTRUCTORS / DESTRUCTOR ---------- */
Config::Config(const std::string& conf) : conf_path(conf) {
  Log::info("Config File: " + conf_path);
  parse();
}

Config::~Config() {}

Config::ParsingData::ParsingData(const std::string& conf_file) : infile(conf_file.c_str()), line_number(0), nest_level(0), state(NONE) {}

/* ---------- EXCEPTION ---------- */
Config::ConfigError::ConfigError(const std::string msg) {
  err_msg = "[CONFIG] " + msg;
  Log::error(err_msg);
}

Config::ConfigError::ConfigError(const ParsingData& data, const std::string msg) {
  std::stringstream num;
  num << data.line_number;
  err_msg = "[CONFIG] " + msg + "\nLine " + num.str() + ": " + data.line;
  Log::error(err_msg);
}

Config::ConfigError::~ConfigError() throw() {}

const char* Config::ConfigError::what() const throw() {
  return err_msg.c_str();
}

/* ---------- HELPER FUNCTIONS ---------- */
namespace {

  typedef std::vector<ServerData>::const_iterator c_server_it;
  typedef std::vector<ServerData>::iterator server_it;
  typedef std::vector<LocationData>::const_iterator c_location_it;
  typedef std::vector<LocationData>::iterator location_it;
  typedef std::string::const_iterator string_it;
  typedef std::map<int, std::string>::iterator error_it;

  std::vector<std::string> tokenizeLine(std::string line) {
    size_t comment_start = line.find_first_of("#");
    if (comment_start != line.npos) {
      if (comment_start == 0) {
        line.clear();
      }
      else {
        line = line.substr(0, comment_start);
      }
    }
    std::istringstream line_s(line);
    std::vector<std::string> result;
    std::string token;
    while (line_s >> token) {
      result.push_back(token);
    }
    return result;
  }

  bool findToken(const std::vector<std::string>& processed, const std::string& token) {
    return std::find(processed.begin(), processed.end(), token) != processed.end();
  }

}

/* ---------- CLASS MEMBER METHODS ---------- */
const std::string& Config::getPath() const {
  return this->conf_path;
}

const std::map<std::string, std::string>& Config::getMime() const {
  return this->mime_types;
}

const std::vector<ServerData>& Config::getServers() const {
  return this->servers;
}

void Config::parse() {
  ParsingData data(conf_path);
  if (!data.infile.is_open()) {
    throw ConfigError("Failed to open Config File: " + conf_path);
  }
  while (std::getline(data.infile, data.line, '\n')) {
    ++data.line_number;
    data.tokens = tokenizeLine(data.line);
    if (data.tokens.empty()) {
      continue;  // Skip empty lines
    }
    switch (data.state) {
      case NONE:
        handleNoBlock(data);
        break;
      case MIME:
        handleMimeBlock(data);
        break;
      case SERVER:
        handleServerBlock(data);
        break;
      case LOCATION:
        handleLocationBlock(data);
        break;
    }
  }
  verifyRequiredData();
  normalisePaths();
}

void Config::handleNoBlock(ParsingData& data) {
  data.state = validateBlockOpen(data);
  if (data.state == SERVER) {
    servers.push_back(ServerData());
    data.server_processed.clear();
  }
  else if (data.state != MIME) {
    throw ConfigError(data, "Invalid Line");
  }
}

void Config::handleMimeBlock(ParsingData& data) {
  if (validateBlockClose(data)) {
    data.state = NONE;
    return;
  }
  parseMime(data);
}

void Config::handleServerBlock(ParsingData& data) {
  if (validateBlockClose(data)) {
    data.state = NONE;
    return;
  }
  if (validateBlockOpen(data) == LOCATION) {
    data.state = LOCATION;
    servers.back().locations.push_back(LocationData());
    data.location_processed.clear();
    parseLocation(data);
    return;
  }
  parseServer(data);
}

void Config::handleLocationBlock(ParsingData& data) {
  if (validateBlockClose(data)) {
    data.state = SERVER;
    return;
  }
  parseLocation(data);
}

void Config::parseMime(ParsingData& data) {
  size_t divide = data.tokens[0].find_first_of("/");
  if (data.tokens.size() == 1 || divide == data.tokens[0].npos || divide == 0 || divide == data.tokens[0].length() - 1) {
    throw ConfigError(data, "Invalid Mime Directive");
  }
  for (size_t i = 1; i < data.tokens.size(); ++i) {
    mime_types["." + data.tokens[i]] = data.tokens[0];
  }
}

Config::ServerDirective Config::strToServerDirective(const ParsingData& data) {
  std::string type = data.tokens[0].substr(0, 11) == "error_page_" ? "error_page_" : data.tokens[0];
  static std::map<std::string, ServerDirective> types_map;
  if (types_map.empty()) {
    types_map["listen"] = PORT;
    types_map["host"] = HOST;
    types_map["server_name"] = NAME;
    types_map["client_max_body_size"] = BODY;
    types_map["error_page_"] = ERR;
  }
  std::map<std::string, ServerDirective>::const_iterator it = types_map.find(type);
  return it != types_map.end() ? it->second : INVALID;
}

void Config::parseServer(ParsingData& data) {
  if (findToken(data.server_processed, data.tokens[0])) {
    throw ConfigError(data, "Duplicate Server Directive");
  }
  switch (strToServerDirective(data)) {
    case PORT:
      setPort(data);
      break;
    case HOST:
      setHost(data);
      break;
    case NAME:
      setName(data);
      break;
    case BODY:
      setBodySize(data);
      break;
    case ERR:
      setErrorPage(data);
      break;
    case INVALID:
      throw ConfigError(data, "Invalid Server Directive");
  }
  data.server_processed.push_back(data.tokens[0]);
}

Config::LocationDirective Config::strToLocationDirective(const ParsingData& data) {
  const std::string& type = data.tokens[0];
  static std::map<std::string, LocationDirective> types_map;
  if (types_map.empty()) {
    types_map["location"] = LOC;
    types_map["allowed_methods"] = MET;
    types_map["root"] = ROOT;
    types_map["index"] = IND;
    types_map["autoindex"] = AUTOIND;
    types_map["upload_path"] = UPLOAD;
    types_map["cgi_extension"] = CGI_EXT;
    types_map["cgi_interpreter"] = CGI_INT;
    types_map["redirect"] = REDIR;
  }
  std::map<std::string, LocationDirective>::const_iterator it = types_map.find(type);
  return it != types_map.end() ? it->second : INVLD;
}

void Config::parseLocation(ParsingData& data) {
  if (findToken(data.location_processed, data.tokens[0])) {
    throw ConfigError(data, "Duplicate Location Directive");
  }
  switch (strToLocationDirective(data)) {
    case LOC:
      setPath(data);
      break;
    case MET:
      setMethods(data);
      break;
    case ROOT:
      setRoot(data);
      break;
    case IND:
      setIndex(data);
      break;
    case AUTOIND:
      setAutoIndex(data);
      break;
    case UPLOAD:
      setUploadPath(data);
      break;
    case CGI_EXT:
      setCgiExtension(data);
      break;
    case CGI_INT:
      setCgiInterpreter(data);
      break;
    case REDIR:
      setRedirect(data);
      break;
    case INVLD:
      throw ConfigError(data, "Invalid Location Directive");
  }
  data.location_processed.push_back(data.tokens[0]);
}

Config::ParseState Config::validateBlockOpen(ParsingData& data) {
  const std::vector<std::string>& tokens = data.tokens;
  static std::map<std::string, std::pair<ParseState, int> > open_states;
  if (open_states.empty()) {
    open_states["mime"] = std::make_pair(MIME, 0);
    open_states["server"] = std::make_pair(SERVER, 0);
    open_states["location"] = std::make_pair(LOCATION, 1);
  }
  std::map<std::string, std::pair<ParseState, int> >::const_iterator it = open_states.find(tokens[0]);
  if (it == open_states.end() || tokens.size() > 3 || (tokens.size() == 2 && tokens[1] != "{") || (tokens.size() == 3 && tokens[2] != "{")) {
    return NONE;
  }
  if (it->second.second != data.nest_level) {
    throw ConfigError(data, "Can't open " + tokens[0] + " block here. Ensure previous blocks are closed");
  }
  ++data.nest_level;
  return it->second.first;
}

bool Config::validateBlockClose(ParsingData& data) {
  if (data.tokens.size() == 1 && data.tokens[0] == "}") {
    --data.nest_level;
    return true;
  }
  return false;
}

void Config::setPort(const ParsingData& data) {
  const std::vector<std::string>& port = data.tokens;
  if (port.size() != 2 || port[1].empty()) {
    throw ConfigError(data, "One listen port must be specified per server");
  }
  if (port[1].length() == 0 || port[1].length() > 5 || port[1].find_first_not_of("1234567890") != port[1].npos) {
    throw ConfigError(data, "Port value must be a number from 1-65535");
  }
  int port_number = std::atoi(port[1].c_str());
  if (port_number < 1 || port_number > 65535) {
    throw ConfigError(data, "Port value must be a number from 1-65535");
  }
  servers.back().port = port_number;
}

void Config::setHost(const ParsingData& data) {
  const std::vector<std::string>& host = data.tokens;
  if (host.size() != 2 || host[1].empty()) {
    throw ConfigError(data, "One host must be specified per server");
  }
  if (host[1] == "localhost") {
    servers.back().host = host[1];
    return;
  }
  if (host[1].find_first_not_of("1234567890.") != host[1].npos || host[1].length() < 7 || host[1].length() > 15) {
    throw ConfigError(data, "Invalid Host Value");
  }
  std::istringstream address(host[1]);
  int a, b, c, d;
  char dot1, dot2, dot3;
  if (!(address >> a >> dot1 >> b >> dot2 >> c >> dot3 >> d)) {
    throw ConfigError(data, "Invalid IP address");
  }
  if (dot1 != '.' || dot2 != '.' || dot3 != '.') {
    throw ConfigError(data, "Invalid IP address");
  }
  if (address.peek() != EOF) {
    throw ConfigError(data, "Invalid IP address");
  }
  if (a < 0 || a > 255 || b < 0 || b > 255 || c < 0 || c > 255 || d < 0 || d > 255) {
    throw ConfigError(data, "Invalid IP address");
  }
  servers.back().host = host[1];
}

void Config::setName(const ParsingData& data) {
  const std::string& name = data.tokens[1];
  if (name.length() == 0) {
    throw ConfigError(data, "Server name cannot be empty");
  }
  if (name.length() > 253) {
    throw ConfigError(data, "Server name too long (max: 253 characters)");
  }
  std::string allowed("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890-.");
  if (name.find_first_not_of(allowed) != name.npos) {
    throw ConfigError(data, "Server name may contain alphanumeric characters and '-' and '.'");
  }
  if (name[0] == '-' || name[0] == '.' || name[name.length() - 1] == '-' || name[name.length() - 1] == '.') {
    throw ConfigError(data, "Server name cannot begin or end with '-' or '.'");
  }
  int count = 0;
  for (string_it it = name.begin(); it != name.end(); ++it) {
    if (*it == '.') {
      if ((it + 1) != name.end() && *(it + 1) == '.') {
        throw ConfigError(data, "Server name must not have consecutive '.'");
      }
      if (count > 63 || count == 0) {
        throw ConfigError(data, "Each label must be between 1 and 63 characters");
      }
      count = 0;
    }
    else {
      ++count;
    }
  }
  if (count > 63 || count == 0) {
    throw ConfigError(data, "Each label must be between 1 and 63 characters");
  }
  servers.back().name = name;
}

void Config::setBodySize(const ParsingData& data) {
  const std::vector<std::string>& body = data.tokens;
  if (body.size() != 2 || body[1].empty()) {
    throw ConfigError(data, "One client_max_body_size must be specified per server");
  }
  if (body[1].find_first_not_of("1234567890") != body[1].npos) {
    throw ConfigError(data, "Invalid Body Size value");
  }
  if (body[1].length() == 1 && body[1][0] == '0') {
    servers.back().client_body_max = 0;
  }
  char* end;
  size_t result = std::strtoul(body[1].c_str(), &end, 10);
  if (errno == ERANGE || *end != '\0' || result == 0) {
    throw ConfigError(data, "Invalid Body Size value");
  }
  servers.back().client_body_max = result;
}

void Config::setErrorPage(const ParsingData& data) {
  const std::vector<std::string>& errpage = data.tokens;
  if (errpage.size() != 2 || errpage[1].empty()) {
    throw ConfigError(data, "One file must be specified per error page");
  }
  if (errpage[0].length() != 14) {
    throw ConfigError(data, "Invalid error type");
  }
  std::string error_num = errpage[0].substr(11);
  if (error_num.length() != 3 || error_num.find_first_not_of("1234567890") != error_num.npos) {
    throw ConfigError(data, "Invalid error number");
  }
  if (!Filesystem::exists(errpage[1])) {
    throw ConfigError(data, "Error page file does not exist: " + errpage[1]);
  }
  if (!Filesystem::isRegularFile(errpage[1])) {
    throw ConfigError(data, "Specified error page is not a regular file: " + errpage[1]);
  }
  servers.back().errors[std::atoi(error_num.c_str())] = errpage[1];
}

void Config::setPath(const ParsingData& data) {
  std::vector<std::string> path = data.tokens;
  if (path.size() != 3 || path[1].empty()) {
    throw ConfigError(data, "One path must be specified per location");
  }
  if (path[1][0] != '/' || path[2] != "{") {
    throw ConfigError(data, "Invalid Path");
  }
  std::string allowed("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890-_./");
  if (path[1].find_first_not_of(allowed) != path[1].npos || path[1].find("//") != path[1].npos) {
    throw ConfigError(data, "Invalid Path");
  }
  if (path[1].length() > 1 && path[1][path[1].length() - 1] == '/') {
    path[1] = path[1].substr(0, path[1].length() - 1);
  }
  servers.back().locations.back().path = path[1];
}

void Config::setMethods(const ParsingData& data) {
  const std::vector<std::string>& methods = data.tokens;
  if (methods.size() < 2 || methods.size() > 4) {
    throw ConfigError(data, "A minimum of 1 and maximum of 3 allowed methods must be specified");
  }
  for (std::vector<std::string>::const_iterator m = methods.begin() + 1; m != methods.end(); ++m) {
    if (*m != "GET" && *m != "POST" && *m != "DELETE" && *m != "HEAD") {
      throw ConfigError(data, "Invalid Method: " + *m);
    }
    for (std::vector<std::string>::const_iterator m2 = m + 1; m2 != methods.end(); ++m2) {
      if (*m == *m2) {
        throw ConfigError(data, "Each method may only be specified once per location");
      }
    }
  }
  servers.back().locations.back().allowed_methods.assign(methods.begin() + 1, methods.end());
}

void Config::setRoot(const ParsingData& data) {
  const std::vector<std::string>& root = data.tokens;
  if (root.size() != 2 || root[1].empty()) {
    throw ConfigError(data, "A single root path must be specified");
  }
  if (!Filesystem::exists(root[1])) {
    throw ConfigError(data, "Root directory does not exist");
  }
  if (!Filesystem::isDir(root[1])) {
    throw ConfigError(data, "Root must be a directory");
  }
  servers.back().locations.back().root = root[1];
}

void Config::setIndex(const ParsingData& data) {
  const std::vector<std::string>& index = data.tokens;
  if (index.size() != 2 || index[1].empty() || index[1][0] == '.') {
    throw ConfigError(data, "Invalid index");
  }
  std::string allowed("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890-_.");
  if (index[1].find_first_not_of(allowed) != index[1].npos) {
    throw ConfigError(data, "Invalid characters in index");
  }
  size_t dot = index[1].find_first_of(".");
  if (dot == index[1].npos) {
    throw ConfigError(data, "Index must have a file extension");
  }
  std::string extension = index[1].substr(dot + 1);
  if (extension.empty() || extension.find_first_of(".") != extension.npos) {
    throw ConfigError(data, "Index must have a valid file extension");
  }
  servers.back().locations.back().index = index[1];
}

void Config::setAutoIndex(const ParsingData& data) {
  const std::vector<std::string>& autoindex = data.tokens;
  if (autoindex.size() != 2 || autoindex[1].empty() || (autoindex[1] != "true" && autoindex[1] != "false")) {
    throw ConfigError(data, "A single 'true' or 'false' must be specified");
  }
  servers.back().locations.back().autoindex = autoindex[1] == "true";
}

void Config::setUploadPath(const ParsingData& data) {
  const std::vector<std::string>& path = data.tokens;
  if (path.size() != 2 || path[1].empty()) {
    throw ConfigError(data, "One upload path must be specified");
  }
  std::string allowed("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_/");
  if (path[1].find_first_not_of(allowed) != path[1].npos) {
    throw ConfigError(data, "Invalid characters in upload path");
  }
  if (path[1].find("//") != path[1].npos) {
    throw ConfigError(data, "A valid upload path must not contain '//'");
  }
  servers.back().locations.back().upload_path = path[1];
}

void Config::setCgiExtension(const ParsingData& data) {
  const std::vector<std::string> cgi_ext = data.tokens;
  if (cgi_ext.size() != 2 || cgi_ext[1].empty() || cgi_ext[1] != ".py") {
    throw ConfigError(data, "One CGI extension must be specified (.py)");
  }
  servers.back().locations.back().cgi_extension = cgi_ext[1];
}

void Config::setCgiInterpreter(const ParsingData& data) {
  const std::vector<std::string>& cgi_int = data.tokens;
  if (cgi_int.size() != 2 || cgi_int[1].empty()) {
    throw ConfigError(data, "One CGI Interpreter must be specified");
  }
  if (!Filesystem::exists(cgi_int[1])) {
    throw ConfigError(data, "Provided CGI Interpreter does not exist");
  }
  if (!Filesystem::isRegularFile(cgi_int[1])) {
    throw ConfigError(data, "Provided CGI Interpreter is not a file");
  }
  if (!Filesystem::isExecutable(cgi_int[1])) {
    throw ConfigError(data, "Provided CGI Interpreter is not executable");
  }
  servers.back().locations.back().cgi_interpreter = cgi_int[1];
}

void Config::setRedirect(const ParsingData& data) {
  const std::vector<std::string> redir = data.tokens;
  if (redir.size() != 3 || redir[1].empty() || redir[2].empty() || redir[2][0] != '/') {
    throw ConfigError(data, "A valid response code and path must be specified");
  }
  if (redir[1] != "301" && redir[1] != "302") {
    throw ConfigError(data, "Invalid Response Code");
  }
  std::string allowed("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890-_./");
  if (redir[2].find_first_not_of(allowed) != redir[2].npos) {
    throw ConfigError(data, "Invalid characters in path");
  }
  size_t dot = redir[2].find_first_of('.');
  if (dot != redir[2].npos) {
    std::string extension = redir[2].substr(dot + 1);
    if (extension.find_first_of('.') != extension.npos) {
      throw ConfigError(". is only valid for a file extension");
    }
  }
  servers.back().locations.back().redirect = std::make_pair(std::atoi(redir[1].c_str()), redir[2]);
}

void Config::verifyRequiredData() {
  if (mime_types.empty()) {
    setDefaultMime();
  }
  if (servers.empty()) {
    throw ConfigError("No servers defined in config file");
  }

  for (c_server_it srv = servers.begin(); srv != servers.end(); ++srv) {
    verifyServer(*srv);
    for (c_location_it loc = srv->locations.begin(); loc != srv->locations.end(); ++loc) {
      verifyLocation(*loc);
    }
  }

  verifyVirtualHosts();
  Log::info("Config file succesfully parsed: " + conf_path);
}

void Config::verifyServer(const ServerData& srv) const {
  if (srv.port == 0) {
    throw ConfigError("A port must be specified for each server");
  }
  if (srv.host.empty()) {
    throw ConfigError("A host must be specified for each server");
  }
  if (srv.locations.empty()) {
    throw ConfigError("At least one location block is required for each server");
  }
}

void Config::verifyLocation(const LocationData& loc) const {
  // Redirect only requires path, allowed methods and redirect
  if (loc.redirect.first != 0) {
    return;
  }
  // If one cgi field is present, the other must be
  if (!loc.cgi_extension.empty() && loc.cgi_interpreter.empty()) {
    throw ConfigError("Location " + loc.path + ": cgi extension specified without cgi interpreter");
  }
  if (loc.cgi_extension.empty() && !loc.cgi_interpreter.empty()) {
    throw ConfigError("Location " + loc.path + ": cgi_interpreter specified without cgi extension");
  }
  // Root required if not redirect
  if (loc.root.empty()) {
    throw ConfigError("Location " + loc.path + ": root must be specified for all non-redirect locations");
  }
  if (std::find(loc.allowed_methods.begin(), loc.allowed_methods.end(), "POST") != loc.allowed_methods.end()) {
    if (loc.cgi_interpreter.empty() && loc.upload_path.empty()) {
      throw ConfigError("Location " + loc.path + ": CGI interpreter or upload_path must be specified for POST locations");
    }
  }
}

void Config::verifyVirtualHosts() const {
  for (c_server_it current = servers.begin(); current != servers.end(); ++current) {
    for (c_server_it next = current + 1; next != servers.end(); ++next) {
      if (next->port == current->port) {
        const std::string& current_name = current->name;
        const std::string& next_name = next->name;
        std::stringstream port_number;
        port_number << current->port;
        if (current_name.length() == 0 || next_name.length() == 0) {
          throw ConfigError("Multiple servers using port " + port_number.str() + ". Server names must be specified to use virtual hosts");
        }
        if (current_name == next_name) {
          throw ConfigError("Multiple servers on port " + port_number.str() + ". Unique names are required for virtual hosting");
        }
      }
    }
  }
}

void Config::normalisePaths() {
  std::string current_dir = Filesystem::getCurrentDir();
  for (server_it s = servers.begin(); s != servers.end(); ++s) {
    for (error_it e = s->errors.begin(); e != s->errors.end(); ++e) {
      e->second = Filesystem::normalisePaths(e->second, current_dir);
    }
    for (location_it l = s->locations.begin(); l != s->locations.end(); ++l) {
      if (!l->root.empty()) {
        l->root = Filesystem::normalisePaths(l->root, current_dir);
      }
      if (!l->cgi_interpreter.empty()) {
        l->cgi_interpreter = Filesystem::normalisePaths(l->cgi_interpreter, current_dir);
      }
      if (!l->upload_path.empty()) {
        l->upload_path = Filesystem::normalisePaths(l->upload_path, l->root);
        if (!Filesystem::exists(l->upload_path)) {
          throw ConfigError("Upload Path does not exist: " + l->upload_path);
        }
        if (!Filesystem::isDir(l->upload_path)) {
          throw ConfigError("Upload Path is not a directory: " + l->upload_path);
        }
      }
    }
  }
}

void Config::setDefaultMime() {
  mime_types[".html"] = "text/html";
  mime_types[".htm"] = "text/html";
  mime_types[".css"] = "text/css";
  mime_types[".txt"] = "text/plain";
  mime_types[".xml"] = "text/xml";
  mime_types[".py"] = "text/x-python";
  mime_types[".js"] = "text/javascript";
  mime_types[".json"] = "application/json";
  mime_types[".jpg"] = "image/jpeg";
  mime_types[".jpeg"] = "image/jpeg";
  mime_types[".png"] = "image/png";
  mime_types[".gif"] = "image/gif";
  mime_types[".svg"] = "image/svg+xml";
  mime_types[".ico"] = "image/x-icon";
  mime_types[".woff"] = "font/woff";
  mime_types[".woff2"] = "font/woff2";
  mime_types[".ttf"] = "font/ttf";
  mime_types[".pdf"] = "application/pdf";
  mime_types[".zip"] = "application/zip";
  mime_types[".tar"] = "application/x-tar";
  mime_types[".gz"] = "application/gzip";
  Log::info("Using default mime types");
}
