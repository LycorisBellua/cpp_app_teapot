#include <fstream>
#include <iostream>

#include "../include/Config.hpp"
#include "../include/Debug.hpp"
#include "../include/Filesystem.hpp"
#include "../include/Get.hpp"
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
  // Debug::PrintConfig(router);
  const RouteResponse& response = router.getRoute(RouteRequest(8080, "test.server.name", "/uploads/pic.jpg", "GET"));
  Debug::PrintRouteResponse(response);

  std::cout << "\n\n";

  std::string index_html = Get::index(response);
  std::ofstream index_output("/tmp/index_test.html", std::ios::trunc);
  index_output << index_html;
  index_output.close();

  std::string error1_html = Filesystem::generateErrorPage("404 Not Found", "Requested page could not be found");
  std::ofstream error1_output("/tmp/error1.html", std::ios::trunc);
  error1_output << error1_html;
  error1_output.close();

  std::string error2_html = Filesystem::generateErrorPage(404, "Requested page could not be found");
  std::ofstream error2_output("/tmp/error2.html", std::ios::trunc);
  error2_output << error2_html;
  error2_output.close();
}
