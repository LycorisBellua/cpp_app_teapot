#pragma once

#include <set>      // IWYU pragma: keep
#include <sstream>  // IWYU pragma: keep
#include <string>   // IWYU pragma: keep

#include "Errors_ReturnMsgs.hpp"  // IWYU pragma: keep
#include "Filesystem.hpp"         // IWYU pragma: keep
#include "Log.hpp"                // IWYU pragma: keep
#include "RequestData.hpp"
#include "RouteInfo.hpp"

namespace Get {
  ResponseData handle(const RouteInfo&);
}
