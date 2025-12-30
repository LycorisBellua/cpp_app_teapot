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

  const RouteResponse& response = router.getRoute(RouteRequest(8080, "test.server.name5", "/uploads%2Fnew/%74%65%73%74%2Ej%70g?query", "GET"));
  Debug::PrintRouteResponse(response);

  std::set<std::pair<std::string, int> > ports = router.getPorts();
  typedef std::set<std::pair<std::string, int> >::const_iterator ports_it;
  std::cout << BOLD << "\nAddress/Port Pairs\n" << RESET;
  for (ports_it p = ports.begin(); p != ports.end(); ++p) {
    std::cout << "Address: " << p->first << "  Port: " << p->second << "\n";
  }
}
