#include "Debug.hpp"

namespace {

  typedef std::map<std::string, std::string>::const_iterator mime_it;
  typedef std::vector<ServerData>::const_iterator serv_it;
  typedef std::map<int, std::string>::const_iterator err_it;
  typedef std::vector<LocationData>::const_iterator loc_it;
  typedef std::vector<std::string>::const_iterator met_it;
  typedef std::set<std::pair<std::string, int> >::const_iterator ports_it;

  void printServer(const ServerData& s) {
    std::cout << BOLD << "Port: " << RESET << s.port << "\n"
              << BOLD << "Host: " << RESET << s.host << "\n"
              << BOLD << "Client Max Body Size: " << RESET << s.client_body_max << "\n"
              << BOLD << "\nError Pages\n"
              << RESET;
    for (err_it e = s.errors.begin(); e != s.errors.end(); ++e) {
      std::cout << BOLD << e->first << RESET ": " << e->second << "\n";
    }
  }

  void printLocation(const LocationData& l) {
    if (!l.path.empty()) {
      std::cout << BOLD << "Path: " << RESET << l.path << "\n";
    }
    if (!l.root.empty()) {
      std::cout << BOLD << "Root: " << RESET << l.root << "\n";
    }
    if (!l.index.empty()) {
      std::cout << BOLD << "Index: " << RESET << l.index << "\n";
    }
    std::cout << BOLD << "Autoindex: " << RESET << std::boolalpha << l.autoindex << "\n";
    if (!l.allowed_methods.empty()) {
      std::cout << BOLD << "Allowed Methods: " << RESET;
      for (met_it m = l.allowed_methods.begin(); m != l.allowed_methods.end(); ++m) {
        std::cout << *m << " ";
      }
      std::cout << "\n";
    }
    if (!l.upload_path.empty()) {
      std::cout << BOLD << "Upload Path: " << RESET << l.upload_path << "\n";
    }
    if (!l.cgi_extension.empty()) {
      std::cout << BOLD << "Cgi Extension: " << RESET << l.cgi_extension << "\n";
    }
    if (!l.cgi_interpreter.empty()) {
      std::cout << BOLD << "Cgi Interpreter: " << RESET << l.cgi_interpreter << "\n";
    }
    if (l.redirect.first != 0) {
      std::cout << BOLD << "Redirect: " << l.redirect.first << " " << l.redirect.second << "\n";
    }
  }

  void printErrorPages(const std::map<int, std::string>& err) {
    for (err_it e = err.begin(); e != err.end(); ++e) {
      std::cout << BOLD << e->first << ": " << RESET << e->second << "\n";
    }
  }

}

void Debug::PrintConfig(const Config& conf) {
  const std::map<std::string, std::string>& mime = conf.getMime();
  const std::vector<ServerData>& servers = conf.getServers();

  std::cout << BLUE BOLD << "Config File: " << RESET << conf.getPath() << "\n\n";

  std::cout << BLUE BOLD << "MIME Types:\n" << RESET;
  for (mime_it it = mime.begin(); it != mime.end(); ++it) {
    std::cout << it->first << " = " << it->second << "\n";
  }

  size_t server_number = 0;
  for (serv_it s = servers.begin(); s != servers.end(); ++s) {
    std::cout << BLUE BOLD << "\nSERVER " << server_number << RESET << "\n";
    printServer(*s);
    size_t location_number = 0;
    for (loc_it l = s->locations.begin(); l != s->locations.end(); ++l) {
      std::cout << BLUE BOLD << "\nSERVER " << server_number << " LOCATION " << location_number << RESET << "\n";
      printLocation(*l);
      ++location_number;
    }
    std::cout << "\n";
    ++server_number;
  }
}

void Debug::PrintConfig(const Router& router) {
  const std::vector<ServerData>& servers = router.getServers();

  size_t server_number = 0;
  for (serv_it s = servers.begin(); s != servers.end(); ++s) {
    std::cout << BLUE BOLD << "\nSERVER " << server_number << RESET << "\n";
    printServer(*s);
    size_t location_number = 0;
    for (loc_it l = s->locations.begin(); l != s->locations.end(); ++l) {
      std::cout << BLUE BOLD << "\nSERVER " << server_number << " LOCATION " << location_number << RESET << "\n";
      printLocation(*l);
      ++location_number;
    }
    std::cout << "\n";
    ++server_number;
  }
}

void Debug::PrintRouteResponse(const RouteResponse& response) {
  std::cout << BLUE << "RouteResponse\n"
            << RESET << BOLD << "Error Code: " << RESET << response.error_code << "\n"
            << BOLD << "Error Body: " << RESET << response.error_body << "\n"
            << BOLD << "Full Path: " << RESET << response.full_path << "\n"
            << BOLD << "Mime Type: " << RESET << response.mime_type << "\n"
            << BOLD << "Client Body Max: " << RESET << response.server.client_body_max << "\n\n"
            << UNDERLINE << "Error Pages\n"
            << RESET;
  printErrorPages(response.server.errors);
  std::cout << UNDERLINE << "\nLocation\n" << RESET;
  printLocation(response.location);
}

void Debug::PrintPorts(const std::set<std::pair<std::string, int> >& ports) {
  std::cout << BOLD BLUE << "Port/Address Pairs\n" << RESET;
  for (ports_it p = ports.begin(); p != ports.end(); ++p) {
    std::cout << p->first << " " << p->second << "\n";
  }
}

void Debug::PrintLn(std::string msg) {
  std::cout << RED << msg << RESET << std::endl;
}

void Debug::PrintHttpResponse(const HttpResponse& data) {
  std::cout << BOLD BLUE << "HttpResponse\n" << RESET;
  std::cout << BOLD << "Return Code: " << RESET << data.code << "\n" << BOLD << "Content: " << RESET << data.content << "\n" << std::endl;
}
