#include "../include/Config.hpp"

// CONSTRUCTORS / DESTRUCTOR
ServerData::ServerData() : port(0), client_body_max(1000000) {}

LocationData::LocationData() : autoindex(false), redirect(0, "") {}


Config::Config() : conf_path(DEFAULT_CONFIG_FILE_PATH) {
  setDefaultMime();
}

Config::Config(const std::string& conf) : conf_path(conf) {
  setDefaultMime();
}

Config::Config(const Config& src) {
  // Private - not to be used
  (void)src;
}

Config::~Config() {}

Config& Config::operator=(const Config& src) {
  // Private - not to be used
  (void)src;
  setDefaultMime();
  return *this;
}

// HELPER FUNCTIONS

// Remove comments and leading/trailing whitespace, split on whitespace
static std::vector<std::string> tokenizeLine(std::string line) {
  size_t comment_start = line.find_first_of("#");

  if (comment_start != line.npos) {
    if (comment_start == 0)
      line.clear();
    else
      line = line.substr(0, comment_start);
  }

  std::istringstream line_s(line);
  std::vector<std::string> result;
  std::string token;

  while (line_s >> token) {
    result.push_back(token);
  }
  return result;
}

static ParseState validateBlockOpen(const std::vector<std::string>& tokens) {
  std::map<std::string, ParseState> states;
  states["mime"] = MIME;
  states["server"] = SERVER;
  states["location"] = LOCATION;

  if (states.find(tokens[0]) == states.end() || tokens.size() > 3 || (tokens.size() == 2 && tokens[1] != "{") || (tokens.size() == 3 && tokens[2] != "{"))
    return NONE;

  return states[tokens[0]];
}

static bool validateBlockClose(const std::vector<std::string>& tokens) {
  return tokens.size() == 1 && tokens[0] == "}";
}

static bool validatePort(const std::string& port) {
  if (port.length() == 0 || port.length() > 5 || port.find_first_not_of("1234567890") != port.npos)
    return false;
  return true;
}

static bool validateHost(const std::string& host) {
  if (host.empty())
    return false;
  if (host == "localhost")
    return true;
  if (host.find_first_not_of("1234567890.") != host.npos || host.length() < 7 || host.length() > 15)
    return false;
  std::istringstream address(host);
  int a, b, c, d;
  char dot1, dot2, dot3;
  if (!(address >> a >> dot1 >> b >> dot2 >> c >> dot3 >> d))
    return false;
  if (dot1 != '.' || dot2 != '.' || dot3 != '.')
    return false;
  if (address.peek() != EOF)
    return false;
  if (a < 0 || a > 255 || b < 0 || b > 255 || c < 0 || c > 255 || d < 0 || d > 255)
    return false;
  return true;
}

static bool validateBodySize(const std::string& body) {
  if (body.empty() || body.find_first_not_of("1234567890") != body.npos)
    return false;
  if (body.length() == 1 && body[0] == '0')
    return true;

  char* end;
  size_t result = std::strtoul(body.c_str(), &end, 10);
  if (errno == ERANGE || *end != '\0' || result == 0)
    return false;
  return true;
}

static bool validateErrorPage(const std::vector<std::string>& tokens) {
  if (tokens[0].length() != 14)
    return false;
  std::string errpage_num = tokens[0].substr(11);
  if (errpage_num.length() != 3 || errpage_num.find_first_not_of("1234567890") != errpage_num.npos)
    return false;
  std::ifstream errfile(tokens[1].c_str());
  if (!errfile.is_open())
    return false;
  errfile.close();
  return true;
}

static bool validatePath(const std::vector<std::string>& tokens) {
  if (tokens.size() != 3 || tokens[1].empty() || tokens[1][0] != '/' || tokens[2] != "{")
    return false;
  std::string path = tokens[1];
  std::string allowed("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890-_./");
  if (path.find_first_not_of(allowed) != path.npos || path.find("//") != path.npos)
    return false;
  return true;
}

static bool validateMethods(const std::vector<std::string>& tokens) {
  if (tokens.size() < 2 || tokens.size() > 4)
    return false;
  
  for (std::vector<std::string>::const_iterator t = tokens.begin() + 1; t != tokens.end(); ++t) {
    if (*t != "GET" && *t != "POST" && *t != "DELETE")
      return false;
    for (std::vector<std::string>::const_iterator t2 = t + 1; t2 != tokens.end(); ++t2)
      if (*t == *t2)
        return false;
  }
  return true;
}

static bool validateRoot(const std::vector<std::string>& tokens) {
  if (tokens.size() != 2 || tokens[1].empty())
    return false;

  struct stat info;
  if (stat(tokens[1].c_str(), &info) != 0)
    return false;

  if (!S_ISDIR(info.st_mode))
    return false;

  return true;
}

static bool validateIndex(const std::vector<std::string>& tokens) {
  if (tokens.size() != 2 || tokens[1].empty() || tokens[1][0] == '.')
    return false;
  std::string allowed("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890-_.");
  if (tokens[1].find_first_not_of(allowed) != tokens[1].npos)
    return false;
  size_t dot = tokens[1].find_first_of(".");
  if (dot == tokens[1].npos)
    return false;
  std::string extension = tokens[1].substr(dot + 1);
  if (extension.empty() || extension.find_first_of(".") != extension.npos)
    return false;
  return true;
}

static bool validateAutoIndex(const std::vector<std::string>& tokens) {
  if (tokens.size() != 2 || tokens[1].empty())
    return false;
  return tokens[1] == "true" || tokens[1] == "false";
}

static bool validateUploadPath(const std::vector<std::string>& tokens) {
  if (tokens.size() != 2 || tokens[1].empty())
    return false;
  
  std::string path = tokens[1];
  std::string allowed("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_/");
  
  if (path.find_first_not_of(allowed) != std::string::npos)
    return false;
  
  if (path.find("//") != std::string::npos)
    return false;
  
  return true;
}

static bool validateCgiExtension(const std::vector<std::string>& tokens) {
  if (tokens.size() != 2 || tokens[1].empty())
    return false;
  return tokens[1] == ".py";
}

static bool validateCgiInterpreter(const std::vector<std::string>& tokens) {
  if (tokens.size() != 2 || tokens[1].empty())
    return false;
  
  struct stat info;
  if (stat(tokens[1].c_str(), &info) != 0)
    return false;
  
  if (!S_ISREG(info.st_mode))
    return false;
  
  if (!(info.st_mode & S_IXUSR))
    return false;
  
  return true;
}

static bool validateRedirect(const std::vector<std::string>& tokens) {
  if (tokens.size() != 3 || tokens[1].empty() || tokens[2].empty() || tokens[2][0] != '/')
    return false;
  if (tokens[1] != "301" && tokens[1] != "302")
    return false;
  std::string allowed("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890-_./");
  if (tokens[2].find_first_not_of(allowed) != tokens[2].npos)
    return false;
  size_t dot = tokens[2].find_first_of('.');
  if (dot != tokens[2].npos) {
    std::string extension = tokens[2].substr(dot + 1);
    if (extension.find_first_of('.') != extension.npos)
      return false;
  }
  return true;
}

// CLASS FUNCTIONS

void Config::parse() {
  std::ifstream infile(conf_path.c_str());
  std::string line;
  ParseState state = NONE;

  if (!infile.is_open())
    throw std::runtime_error("Failed to open configuration file: " + conf_path);

  while (std::getline(infile, line, '\n')) {
    std::vector<std::string> tokens = tokenizeLine(line);
    if (tokens.empty())
      continue;

    if (state == NONE) {
      state = validateBlockOpen(tokens);
      if (state == SERVER) {
        servers.push_back(ServerData());
        continue;
      } else if (state == MIME)
        continue;
      else if (state == NONE && validateBlockClose(tokens))
        break;
      else
        throw std::runtime_error("Invalid Line in config file: " + line);
    }

    if (state == MIME) {
      if (validateBlockClose(tokens)) {
        state = NONE;
        continue;
      }
      if (!parseMime(tokens))
        throw std::runtime_error("Invalid Mime Type in config file: " + line);
    }

    else if (state == SERVER) {
      if (validateBlockClose(tokens)) {
        state = NONE;
        continue;
      }
      if (validateBlockOpen(tokens) == LOCATION) {
        state = LOCATION;
        servers.back().locations.push_back(LocationData());
      }
      else if (!parseServer(tokens))
        throw std::runtime_error("Invalid Server Directive in config file: " + line);
    }

    else if (state == LOCATION) {
      if (validateBlockClose(tokens)) {
        state = SERVER;
        continue;
      }
      if (!parseLocation(tokens))
        throw std::runtime_error("Invalid Location Directive in config file: " + line);
    }
  }
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

bool Config::parseMime(const std::vector<std::string>& tokens) {
  size_t divide = tokens[0].find_first_of("/");
  if (tokens.size() == 1 || divide == tokens[0].npos || divide == 0 || divide == tokens[0].length() - 1)
    return false;

  for (size_t i = 1; i < tokens.size(); ++i) {
    mime_types["." + tokens[i]] = tokens[0];
  }

  return true;
}

bool Config::parseServer(const std::vector<std::string>& tokens) {
  if (tokens.size() > 2)
    return false;
  if (tokens[0] == "listen" && validatePort(tokens[1]))
    servers.back().port = std::atoi(tokens[1].c_str());
  else if (tokens[0] == "host" && validateHost(tokens[1]))
    servers.back().host = tokens[1];
  else if (tokens[0] == "client_max_body_size" && validateBodySize(tokens[1]))
    servers.back().client_body_max = std::atol(tokens[1].c_str());
  else if (tokens[0].compare(0, 11, "error_page_") == 0 && validateErrorPage(tokens))
    servers.back().errors[std::atoi(tokens[0].substr(11).c_str())] = tokens[1];
  else
    return false;
  return true;
}

bool Config::parseLocation(const std::vector<std::string>& tokens) {
  if (tokens[0] == "location" && validatePath(tokens))
    servers.back().locations.back().path = tokens[1].substr(1);
  else if (tokens[0] == "allowed_methods" && validateMethods(tokens))
    servers.back().locations.back().allowed_methods.assign(tokens.begin() + 1, tokens.end());
  else if (tokens[0] == "root" && validateRoot(tokens))
    servers.back().locations.back().root = tokens[1];
  else if (tokens[0] == "index" && validateIndex(tokens))
    servers.back().locations.back().index = tokens[1];
  else if (tokens[0] == "autoindex" && validateAutoIndex(tokens))
    servers.back().locations.back().autoindex = tokens[1] == "true" ? true : false;
  else if (tokens[0] == "upload_path" && validateUploadPath(tokens))
    servers.back().locations.back().upload_path = tokens[1];
  else if (tokens[0] == "cgi_extension" && validateCgiExtension(tokens))
    servers.back().locations.back().cgi_extension = tokens[1];
  else if (tokens[0] == "cgi_interpreter" && validateCgiInterpreter(tokens))
    servers.back().locations.back().cgi_interpreter = tokens[1];
  else if (tokens[0] == "redirect" && validateRedirect(tokens))
    servers.back().locations.back().redirect = std::make_pair(std::atoi(tokens[1].c_str()), tokens[2]);
  else
    return false;
  return true;
}
