#include <cstdlib>
#include <iostream>

#include "../include/Config.hpp"
#include "../include/Debug.hpp"
#include "../include/Log.hpp"
#include "../include/webserv.hpp"

namespace {
  ConfigData getConfig(const std::string& conf_file) {
    try {
      Config conf(conf_file);
      conf.parse();
      return ConfigData(conf.getServers(), conf.getMime());
    } catch (const std::exception& e) {
      std::cerr << e.what() << std::endl;
      std::exit(EXIT_FAILURE);
    }
  }
}

int main(int argc, char** argv) {
  Log::info("Webserv started");

  if (argc > 2) {
    Log::errorPrint("Error: Too many launch arguments");
    return 1;
  }

  const ConfigData conf = getConfig(argc == 1 ? DEFAULT_CONFIG_FILE_PATH : argv[1]);
  debugPrintConfig(conf);
}
