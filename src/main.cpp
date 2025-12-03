#include "../include/webserv.hpp"
#include "../include/Config.hpp"
#include "../include/Debug.hpp"
#include <iostream>
#include <cstdlib>

int main(int argc, char **argv)
{
	if (argc > 2)
	{
		std::cerr << "Error: Too many arguments. Provide only one "
			"configuration file." << std::endl;
		return 1;
	}

	std::string path_config = argc == 1 ? DEFAULT_CONFIG_FILE_PATH : argv[1];

  Config conf(path_config);
  try {
    conf.parse();
    debugPrintConfig(conf);
  } catch (std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    std::exit(EXIT_FAILURE);
  }

	return 0;
}
