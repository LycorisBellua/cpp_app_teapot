#include "Log.hpp"
#include "Server.hpp"

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
		const std::string default_config_path = "configs/default.conf";
		Server* server = Server::getInstance(argc == 2 ? argv[1]
			: default_config_path);
		server->runEventLoop();
		delete server;
	}
	catch (const std::exception& e)
	{
		Log::error(e.what());
		return 1;
	}
	return 0;
}
