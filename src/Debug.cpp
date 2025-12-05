#include "../include/Debug.hpp"

void debugPrintConfig(const Config& conf) {
  std::map<std::string, std::string> mime = conf.getMime();
  std::vector<ServerData> servers = conf.getServers();

  std::cout << BLUE BOLD << "Config File: " << RESET << conf.getPath() << "\n\n";

  std::cout << BLUE BOLD << "MIME Types:\n" << RESET;
  for (std::map<std::string, std::string>::iterator it = mime.begin(); it != mime.end(); ++it) {
    std::cout << it->first << " = " << it->second << "\n";
  }

  size_t server_number = 0;
  for (std::vector<ServerData>::iterator s = servers.begin(); s != servers.end(); ++s) {
    std::cout << BLUE BOLD << "\nSERVER " << server_number << RESET << "\n"
              << BOLD << "Port: " << RESET << s->port << "\n"
              << BOLD << "Host: " << RESET << s->host << "\n"
              << BOLD << "Client Max Body Size: " << RESET << s->client_body_max << "\n"
              << BOLD << "\nError Pages\n"
              << RESET;

    for (std::map<int, std::string>::iterator e = s->errors.begin(); e != s->errors.end(); ++e) {
      std::cout << BOLD << e->first << RESET ": " << e->second << "\n";
    }

    size_t location_number = 0;
    for (std::vector<LocationData>::iterator l = s->locations.begin(); l != s->locations.end();
         ++l) {
      std::cout << BLUE BOLD << "\nSERVER " << server_number << " LOCATION " << location_number
                << RESET << "\n";
      if (!l->path.empty()) {
        std::cout << BOLD << "Path: " << RESET << l->path << "\n";
      }
      if (!l->root.empty()) {
        std::cout << BOLD << "Root: " << RESET << l->root << "\n";
      }
      if (!l->index.empty()) {
        std::cout << BOLD << "Index: " << RESET << l->index << "\n";
      }
      std::cout << BOLD << "Autoindex: " << RESET << std::boolalpha << l->autoindex << "\n";
      if (!l->allowed_methods.empty()) {
        std::cout << BOLD << "Allowed Methods: " << RESET;
        for (std::vector<std::string>::iterator m = l->allowed_methods.begin();
             m != l->allowed_methods.end(); ++m) {
          std::cout << *m << " ";
        }
        std::cout << "\n";
      }
      if (!l->upload_path.empty()) {
        std::cout << BOLD << "Upload Path: " << RESET << l->upload_path << "\n";
      }
      if (!l->cgi_extension.empty()) {
        std::cout << BOLD << "Cgi Extension: " << RESET << l->cgi_extension << "\n";
      }
      if (!l->cgi_interpreter.empty()) {
        std::cout << BOLD << "Cgi Interpreter: " << RESET << l->cgi_interpreter << "\n";
      }
      if (l->redirect.first != 0) {
        std::cout << BOLD << "Redirect: " << l->redirect.first << " " << l->redirect.second << "\n";
      }
      ++location_number;
    }
    std::cout << "\n";
    ++server_number;
  }
}
