#include <fstream>
#include <iostream>

#include "../include/Config.hpp"
#include "../include/Debug.hpp"
#include "../include/Get.hpp"
#include "../include/Log.hpp"
#include "../include/Router.hpp"
#include "../include/webserv.hpp"

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

int main(int argc, char** argv) {
  Log::info("Webserv started");
  Router router(getRouter(argc, argv));
  Debug::PrintPorts(router.getPorts());
  // Debug::PrintConfig(router);
  /*const RouteResponse& response = router.getRoute(RouteRequest(8080, "test.server.name", "/uploads/laptop.jpg", "GET"));
  Debug::PrintRouteResponse(response);

  std::ofstream error_file("tests/error_test.html", std::ios::binary);
  error_file << response.error_body;
  error_file.close();

  std::cout << "\n\n";

  HttpResponse result = Get::handle(response);
  Debug::PrintHttpResponse(result);

  std::ofstream output("tests/file_test", std::ios::binary);
  output << result.content;
  output.close();*/
}
