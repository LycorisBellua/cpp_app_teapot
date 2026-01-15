#include "Log.hpp"
#include "Server.hpp"

int main(int argc, char **argv)
{
	/*
		TODO:
		- Use the intra testers (especially for the chunked body).
		- Use the intra testers, and if need be, be able to parse this kind of 
		request:
		https://developer.mozilla.org/en-US/docs/Web/HTTP/Reference/Methods/POST#multipart_form_submission
	*/
	if (argc > 2)
	{
		Log::error("Error: Too many arguments / Provide only one configuration "
			"file path");
		return 1;
	}
	try
	{
		const std::string default_config_path = "configs/default.conf";
		Server(argc == 2 ? argv[1] : default_config_path);
	}
	catch (const std::exception& e)
	{
		Log::error(e.what());
		return 1;
	}
	return 0;
}
