#pragma once

#include <set>      // IWYU pragma: keep
#include <sstream>  // IWYU pragma: keep
#include <string>   // IWYU pragma: keep

#include "ErrorPage.hpp"   // IWYU pragma: keep
#include "Filesystem.hpp"  // IWYU pragma: keep
#include "RequestData.hpp"
#include "Log.hpp"  // IWYU pragma: keep
#include "RouteInfo.hpp"

namespace Get {
  ResponseData handle(const RouteInfo&);
}
