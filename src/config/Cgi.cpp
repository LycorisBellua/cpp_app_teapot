#include "Cgi.hpp"

namespace Cgi {

  ResponseData handle(const RouteInfo& data) {
    return ResponseData(500, ErrorPage::get(500, data.server.errors));
  }

}
