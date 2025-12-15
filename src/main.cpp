#include <iostream>

#include "../include/Config.hpp"
#include "../include/Log.hpp"
#include "../include/Router.hpp"
#include "../include/webserv.hpp"

namespace {

  Router getRouter(const std::string& conf_file) {
    std::vector<ServerData> servers;
    std::map<std::string, std::string> mime;
    try {
      Config conf(conf_file);
      conf.parse();
      servers = conf.getServers();
      mime = conf.getMime();
    } catch (const std::exception& e) {
      std::cerr << e.what() << "\n";
    }
    return Router(servers, mime);
  }

}

int main(int argc, char** argv) {
  Log::info("Webserv started");

  if (argc > 2) {
    Log::errorPrint("Error: Too many launch arguments");
    return 1;
  }

  Router router(getRouter(argc == 2 ? argv[1] : DEFAULT_CONFIG_FILE_PATH));
}
