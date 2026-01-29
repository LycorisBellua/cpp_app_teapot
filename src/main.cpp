#include "Log.hpp"
#include "Server.hpp"

/*
	TODO: The CGI feature needs to non-blocking, and its I/O needs to be 
	checked for readiness by the epoll event loop.
*/

int main(int argc, char** argv)
{
	if (argc > 2)
	{
		Log::error("Error: Too many arguments / Provide only one configuration "
			"file path");
		return 1;
	}
	try
	{
		const std::string default_config_path = "configs/test_site.conf";
		Server(argc == 2 ? argv[1] : default_config_path);
	}
	catch (const std::exception& e)
	{
		Log::error(e.what());
		return 1;
	}
	return 0;
}
