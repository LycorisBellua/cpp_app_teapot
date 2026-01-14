#pragma once

#include "ErrorPage.hpp"  // IWYU pragma: keep
#include "HttpResponse.hpp"
#include "RouteInfo.hpp"

namespace Cgi {

  HttpResponse handle(const RouteResponse&);

}
