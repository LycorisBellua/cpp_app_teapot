#include "Config.hpp"
#include "Debug.hpp"
#include "Get.hpp"
#include "Log.hpp"
#include "Router.hpp"
#include "Server.hpp"
#include <iostream>
#include <fstream>

#define DEFAULT_CONFIG_FILE_PATH "configs/default.conf"

namespace {

  Router getRouter(int ac, char** av) {
    if (ac > 2) {
      Log::error("Error: Too many launch arguments");
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

int main(int argc, char **argv)
{
  Log::info("Webserv started");
  Router router(getRouter(argc, argv));
  //Debug::PrintPorts(router.getPorts());
  Server server = Server(router);
  // TODO: Use the intra testers (especially for the chunked body).
  return 0;
}