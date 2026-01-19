#include "Cgi.hpp"

namespace Cgi {

  HttpResponse handle(const RouteInfo& data) {
    return HttpResponse(500, ErrorPage::get(500, data.server.errors));
  }

}
