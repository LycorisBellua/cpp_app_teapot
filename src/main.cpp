#include "Get.hpp"
#include "RequestData.hpp"
#include "Log.hpp"
#include "Post.hpp"
#include "Router.hpp"
#include "Server.hpp"

int main(int argc, char** argv) {
  /*
          TODO:
          - Use the intra testers (especially for the chunked body).
          - Use the intra testers, and if need be, be able to parse this kind of
          request:
          https://developer.mozilla.org/en-US/docs/Web/HTTP/Reference/Methods/POST#multipart_form_submission
  */
  if (argc > 2) {
    Log::error(
        "Error: Too many arguments / Provide only one configuration "
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
      "The HTTP 201 Created successful response status code indicates that the HTTP request has led to the creation of a resource. This status code "
      "is commonly sent as the result of a POST request. The new resource, or a description and link to the new resource, is created before the "
      "response is returned. The newly-created items can be returned in the body of the response message, but must be locatable by the URL of the "
      "initiating request or by the URL in the value of the Location header provided with the response.";
  const Router test_router = (Config(argv[1]));
  const RequestData test_req(0, 8080, "test.server.name", "/uploads", "HEAD", "", multipart_body);
  const RouteInfo test_route = test_router.getRoute(test_req);
  const ResponseData test_http = Get::handle(test_route);

  return 0;
}
