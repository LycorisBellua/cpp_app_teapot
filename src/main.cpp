#include "HttpResponse.hpp"
#include "Log.hpp"
#include "Server.hpp"
#include "Router.hpp"
#include "Get.hpp"
#include "Post.hpp"

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
	/*try
	{
		const std::string default_config_path = "configs/default.conf";
		Server(argc == 2 ? argv[1] : default_config_path);
	}
	catch (const std::exception& e)
	{
		Log::error(e.what());
		return 1;
	}*/
  const std::string multipart_body =
        "--delimiter12345\r\n"
        "Content-Disposition: form-data; name=\"field1\"\r\n"
        "\r\n"
        "value1\r\n"
        "--delimiter12345\r\n"
        "Content-Disposition: form-data; name=\"field2\"; filename=\"example.txt\"\r\n"
        "\r\n"
        "value2\r\n"
        "--delimiter12345--\r\n";
  const Router test_router= (Config(argv[1]));
  const RouteRequest test_req(0, 8080, "test.server.name", "/uploads", "POST", "multipart/form-data;boundary=\"delimiter12345\"", multipart_body);
  const RouteResponse test_route = test_router.getRoute(test_req);
  const HttpResponse test_http = Post::handle(test_route);


  return 0;
}
