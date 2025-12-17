#include <iostream>

#include "../include/Config.hpp"
#include "../include/Debug.hpp"
#include "../include/Log.hpp"
#include "../include/Router.hpp"
#include "../include/webserv.hpp"

namespace {

  Router getRouter(int ac, char** av) {
    if (ac > 2) {
      Log::errorPrint("Error: Too many launch arguments");
      std::exit(EXIT_FAILURE);
    }
    try {
      Config conf(ac == 2 ? av[1] : DEFAULT_CONFIG_FILE_PATH);
      return Router(conf.getServers(), conf.getMime());
    } catch (const std::exception& e) {
      std::cerr << e.what() << std::endl;
      std::exit(EXIT_FAILURE);
    }
  }

}

int main(int argc, char** argv) {
  Log::info("Webserv started");
  Router router(getRouter(argc, argv));
  Debug::PrintConfig(router);
}
