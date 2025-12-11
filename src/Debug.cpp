#include "../include/Debug.hpp"

void debugPrintConfig(const Config& conf) {
  const std::map<std::string, std::string>& mime = conf.getMime();
  const std::vector<ServerData>& servers = conf.getServers();

  std::cout << BLUE BOLD << "Config File: " << RESET << conf.getPath() << "\n\n";

  std::cout << BLUE BOLD << "MIME Types:\n" << RESET;
  for (mime_it it = mime.begin(); it != mime.end(); ++it) {
    std::cout << it->first << " = " << it->second << "\n";
  }

  size_t server_number = 0;
  for (serv_it s = servers.begin(); s != servers.end(); ++s) {
    std::cout << BLUE BOLD << "\nSERVER " << server_number << RESET << "\n"
              << BOLD << "Port: " << RESET << s->port << "\n"
              << BOLD << "Host: " << RESET << s->host << "\n"
              << BOLD << "Client Max Body Size: " << RESET << s->client_body_max << "\n"
              << BOLD << "\nError Pages\n"
              << RESET;

    for (err_it e = s->errors.begin(); e != s->errors.end(); ++e) {
      std::cout << BOLD << e->first << RESET ": " << e->second << "\n";
    }

    size_t location_number = 0;
    for (loc_it l = s->locations.begin(); l != s->locations.end(); ++l) {
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
        for (met_it m = l->allowed_methods.begin(); m != l->allowed_methods.end(); ++m) {
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

void debugPrintConfig(const ConfigData& conf) {
  const std::map<std::string, std::string>& mime = conf.mime;
  const std::vector<ServerData>& servers = conf.servers;

  std::cout << BLUE BOLD << "MIME Types:\n" << RESET;
  for (mime_it it = mime.begin(); it != mime.end(); ++it) {
    std::cout << it->first << " = " << it->second << "\n";
  }

  size_t server_number = 0;
  for (serv_it s = servers.begin(); s != servers.end(); ++s) {
    // Print server details
    std::cout << BLUE BOLD << "\nSERVER " << server_number << RESET << "\n"
              << BOLD << "Port: " << RESET << s->port << "\n"
              << BOLD << "Host: " << RESET << s->host << "\n"
              << BOLD << "Client Max Body Size: " << RESET << s->client_body_max << "\n"
              << BOLD << "\nError Pages\n"
              << RESET;

    for (err_it e = s->errors.begin(); e != s->errors.end(); ++e) {
      std::cout << BOLD << e->first << RESET ": " << e->second << "\n";
    }

    // Print location details for each server
    size_t location_number = 0;
    for (loc_it l = s->locations.begin(); l != s->locations.end(); ++l) {
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
        for (met_it m = l->allowed_methods.begin(); m != l->allowed_methods.end(); ++m) {
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
void debugPrintLn(std::string msg) {
  std::cout << RED << msg << RESET << std::endl;
}
