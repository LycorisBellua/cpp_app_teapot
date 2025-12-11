#include "../include/Config.hpp"

/* ---------- CONSTRUCTORS / DESTRUCTOR ---------- */
Config::Config() : conf_path(DEFAULT_CONFIG_FILE_PATH) {}

Config::Config(const std::string& conf) : conf_path(conf) {}

Config::Config(const Config& src) {
  // Private - not to be used
  (void)src;
}

Config::~Config() {}

Config& Config::operator=(const Config& src) {
  // Private - not to be used
  (void)src;
  return *this;
}

Config::ParsingData::ParsingData(const std::string& conf_file)
    : infile(conf_file.c_str()), line_number(0), state(NONE) {}

/* ---------- EXCEPTION ---------- */
Config::ConfigError::ConfigError(const std::string msg) {
  err_msg = "Config File Error:\n" + msg;
}

Config::ConfigError::ConfigError(const ParsingData& data, const std::string msg) {
  std::stringstream num;
  num << data.line_number;
  err_msg = "Config File Error: " + msg + "\nLine " + num.str() + ": " + data.line;
}

Config::ConfigError::~ConfigError() throw() {}

const char* Config::ConfigError::what() const throw() {
  return err_msg.c_str();
}

/* ---------- HELPER FUNCTIONS ---------- */
static std::vector<std::string> tokenizeLine(std::string line) {
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

static bool findToken(const std::vector<std::string>& processed, const std::string& token) {
  return std::find(processed.begin(), processed.end(), token) != processed.end();
}

/* ---------- CLASS MEMBER METHODS ---------- */
void Config::parse() {
  ParsingData data(conf_path);
  if (!data.infile.is_open()) {
    throw ConfigError("Failed to open Config File: " + conf_path);
  }
  while (std::getline(data.infile, data.line, '\n')) {
    ++data.line_number;
    data.tokens = tokenizeLine(data.line);
    if (data.tokens.empty()) {
      continue;
    }

    // Not in any block. Either open Server/Mime block, end file or error
    if (data.state == NONE) {
      data.state = validateBlockOpen(data.tokens);
      if (data.state == SERVER) {
        servers.push_back(ServerData());
        data.server_processed.clear();
        continue;
      }
      else if (data.state == MIME) {
        continue;
      }
      else if (data.state == NONE && validateBlockClose(data.tokens)) {
        break;
      }
      else {
        throw ConfigError(data, "Invalid line");
      }
    }

    // Close mime block or parse mime value
    if (data.state == MIME) {
      if (validateBlockClose(data.tokens)) {
        data.state = NONE;
        continue;
      }
      parseMime(data);
    }

    // Close server block, open location block, or parse server value
    else if (data.state == SERVER) {
      if (validateBlockClose(data.tokens)) {
        data.state = NONE;
        continue;
      }
      if (validateBlockOpen(data.tokens) == LOCATION) {
        data.state = LOCATION;
        servers.back().locations.push_back(LocationData());
        data.location_processed.clear();
      }
      else {
        parseServer(data);
      }
    }

    // Close location block or parse location value
    if (data.state == LOCATION) {
      if (validateBlockClose(data.tokens)) {
        data.state = SERVER;
        continue;
      }
      parseLocation(data);
    }
  }
  verifyRequiredData();
}

const std::string& Config::getPath() const {
  return this->conf_path;
}

const std::map<std::string, std::string>& Config::getMime() const {
  return this->mime_types;
}

const std::vector<ServerData>& Config::getServers() const {
  return this->servers;
}

bool Config::parseMime(ParsingData& data) {
  size_t divide = data.tokens[0].find_first_of("/");
  if (data.tokens.size() == 1 || divide == data.tokens[0].npos || divide == 0 ||
      divide == data.tokens[0].length() - 1) {
    throw ConfigError(data, "Invalid Mime Directive");
  }
  for (size_t i = 1; i < data.tokens.size(); ++i) {
    mime_types["." + data.tokens[i]] = data.tokens[0];
  }
  return true;
}

bool Config::parseServer(ParsingData& data) {
  if (findToken(data.server_processed, data.tokens[0])) {
    throw ConfigError(data, "Duplicate Server Directive");
  }
  if (data.tokens[0] == "listen") {
    setPort(data);
  }
  else if (data.tokens[0] == "host") {
    setHost(data);
  }
  else if (data.tokens[0] == "client_max_body_size") {
    setBodySize(data);
  }
  else if (data.tokens[0].compare(0, 11, "error_page_") == 0) {
    setErrorPage(data);
  }
  else {
    throw ConfigError(data, "Invalid Server Directive");
  }
  data.server_processed.push_back(data.tokens[0]);
  return true;
}

bool Config::parseLocation(ParsingData& data) {
  if (findToken(data.location_processed, data.tokens[0])) {
    throw ConfigError(data, "Duplicate Location Directive");
  }
  if (data.tokens[0] == "location") {
    setPath(data);
  }
  else if (data.tokens[0] == "allowed_methods") {
    setMethods(data);
  }
  else if (data.tokens[0] == "root") {
    setRoot(data);
  }
  else if (data.tokens[0] == "index") {
    setIndex(data);
  }
  else if (data.tokens[0] == "autoindex") {
    setAutoIndex(data);
  }
  else if (data.tokens[0] == "upload_path") {
    setUploadPath(data);
  }
  else if (data.tokens[0] == "cgi_extension") {
    setCgiExtension(data);
  }
  else if (data.tokens[0] == "cgi_interpreter") {
    setCgiInterpreter(data);
  }
  else if (data.tokens[0] == "redirect") {
    setRedirect(data);
  }
  else {
    throw ConfigError(data, "Invalid Location Directive");
  }
  data.location_processed.push_back(data.tokens[0]);
  return true;
}

Config::ParseState Config::validateBlockOpen(const std::vector<std::string>& tokens) {
  static std::map<std::string, ParseState> open_states;
  open_states["mime"] = MIME;
  open_states["server"] = SERVER;
  open_states["location"] = LOCATION;
  if (open_states.find(tokens[0]) == open_states.end() || tokens.size() > 3 ||
      (tokens.size() == 2 && tokens[1] != "{") || (tokens.size() == 3 && tokens[2] != "{")) {
    return NONE;
  }
  return open_states[tokens[0]];
}

bool Config::validateBlockClose(const std::vector<std::string>& tokens) {
  if (tokens.size() == 1 && tokens[0] == "}") {
    return true;
  }
  return false;
}

void Config::setPort(const ParsingData& data) {
  const std::vector<std::string>& port(data.tokens);
  if (port.size() != 2 || port[1].empty()) {
    throw ConfigError(data, "One listen port must be specified per server");
  }
  if (port[1].length() == 0 || port[1].length() > 5 ||
      port[1].find_first_not_of("1234567890") != port[1].npos) {
    throw ConfigError(data, "Port value must be a number from 1-65535");
  }
  int port_number = std::atoi(port[1].c_str());
  if (port_number < 1 || port_number > 65535) {
    throw ConfigError(data, "Port value must be a number from 1-65535");
  }
  servers.back().port = port_number;
}

void Config::setHost(const ParsingData& data) {
  const std::vector<std::string>& host(data.tokens);
  if (host.size() != 2 || host[1].empty()) {
    throw ConfigError(data, "One host must be specified per server");
  }
  if (host[1] == "localhost") {
    servers.back().host = host[1];
    return;
  }
  if (host[1].find_first_not_of("1234567890.") != host[1].npos || host[1].length() < 7 ||
      host[1].length() > 15) {
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

void Config::setBodySize(const ParsingData& data) {
  const std::vector<std::string>& body(data.tokens);
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
  const std::vector<std::string>& errpage(data.tokens);
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
  std::ifstream errfile(errpage[1].c_str());
  if (!errfile.is_open()) {
    throw ConfigError(data, "Invalid file");
  }
  errfile.close();
  servers.back().errors[std::atoi(error_num.c_str())] = errpage[1];
}

void Config::setPath(const ParsingData& data) {
  const std::vector<std::string>& path(data.tokens);
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
  servers.back().locations.back().path = path[1];
}

void Config::setMethods(const ParsingData& data) {
  const std::vector<std::string>& methods(data.tokens);
  if (methods.size() < 2 || methods.size() > 4) {
    throw ConfigError(data, "A minimum of 1 and maximum of 3 allowed methods must be specified");
  }
  for (std::vector<std::string>::const_iterator m = methods.begin() + 1; m != methods.end(); ++m) {
    if (*m != "GET" && *m != "POST" && *m != "DELETE") {
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
  const std::vector<std::string>& root(data.tokens);
  if (root.size() != 2 || root[1].empty()) {
    throw ConfigError(data, "A single root path must be specified");
  }
  struct stat info;
  if (stat(root[1].c_str(), &info) != 0) {
    throw ConfigError(data, "Root directory does not exist");
  }
  if (!S_ISDIR(info.st_mode)) {
    throw ConfigError(data, "Root must be a directory");
  }
  servers.back().locations.back().root = root[1];
}

void Config::setIndex(const ParsingData& data) {
  const std::vector<std::string>& index(data.tokens);
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
  const std::vector<std::string>& autoindex(data.tokens);
  if (autoindex.size() != 2 || autoindex[1].empty() ||
      (autoindex[1] != "true" && autoindex[1] != "false")) {
    throw ConfigError(data, "A single 'true' or 'false' must be specified");
  }
  servers.back().locations.back().autoindex = autoindex[1] == "true";
}

void Config::setUploadPath(const ParsingData& data) {
  const std::vector<std::string>& path(data.tokens);
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
  const std::vector<std::string> cgi_ext(data.tokens);
  if (cgi_ext.size() != 2 || cgi_ext[1].empty() || cgi_ext[1] != ".py") {
    throw ConfigError(data, "One CGI extension must be specified (.py)");
  }
  servers.back().locations.back().cgi_extension = cgi_ext[1];
}

void Config::setCgiInterpreter(const ParsingData& data) {
  const std::vector<std::string>& cgi_int(data.tokens);
  if (cgi_int.size() != 2 || cgi_int[1].empty()) {
    throw ConfigError(data, "One CGI Interpreter must be specified");
  }
  struct stat info;
  if (stat(cgi_int[1].c_str(), &info) != 0) {
    throw ConfigError(data, "Provided CGI Interpreter does not exist");
  }
  if (!S_ISREG(info.st_mode)) {
    throw ConfigError(data, "Value is not a file");
  }
  if (!(info.st_mode & S_IXUSR)) {
    throw ConfigError(data, "File is not executable");
  }
  servers.back().locations.back().cgi_interpreter = cgi_int[1];
}

void Config::setRedirect(const ParsingData& data) {
  const std::vector<std::string> redir(data.tokens);
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
  for (std::vector<ServerData>::const_iterator s = servers.begin(); s != servers.end(); ++s) {
    if (s->port == 0) {
      throw ConfigError("A port must be specified for each server");
    }
    if (s->host.empty()) {
      throw ConfigError("A host must be specified for each server");
    }
    if (s->locations.empty()) {
      throw ConfigError("At least one location block is required for each server");
    }
    for (std::vector<LocationData>::const_iterator l = s->locations.begin();
         l != s->locations.end(); ++l) {
      // Path always required - presence guaranteed by parsing
      if (l->allowed_methods.empty()) {
        throw ConfigError("Location: " + l->path +
                          "\nAllowed_methods must be specified for each location");
      }
      // Redirect only requires path, allowed methods and redirect
      if (l->redirect.first != 0) {
        continue;
      }
      // If one cgi field is present, the other must be
      if (!l->cgi_extension.empty() && l->cgi_interpreter.empty()) {
        throw ConfigError("Location: " + l->path +
                          ": cgi extension specified without cgi interpreter");
      }
      if (l->cgi_extension.empty() && !l->cgi_interpreter.empty()) {
        throw ConfigError("Location: " + l->path +
                          ": cgi_interpreter specified without cgi extension");
      }
      // Root required if not redirect
      if (l->root.empty()) {
        throw ConfigError("Location: " + l->path +
                          ": root must be specified for all non-redirect locations");
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
}
