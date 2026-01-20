#pragma once

#include "ErrorPage.hpp"  // IWYU pragma: keep
#include "RequestData.hpp"
#include "RouteInfo.hpp"

namespace Cgi {

  ResponseData handle(const RouteInfo&);

}
