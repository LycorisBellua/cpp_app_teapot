#include "webserv.hpp"
#include <iostream>

int main(int argc, char **argv)
{
	if (argc > 2)
	{
		std::cerr << "Error: Too many arguments. Provide only one "
			"configuration file." << std::endl;
		return 1;
	}

	std::string path_config = argc == 1 ? DEFAULT_CONFIG_FILE_PATH : argv[1];
	std::cout << "Debug: The path to the config file is " << path_config
		<< std::endl;

	return 0;
}
