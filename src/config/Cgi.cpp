#include "Cgi.hpp"

namespace Cgi {

  HttpResponse handle(const RouteResponse& data) {
    return HttpResponse(500, ErrorPage::get(500, data.server.errors));
  }

}
