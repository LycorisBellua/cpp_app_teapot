#pragma once

#include <set>
#include <sstream>
#include <string>

#include "Errors_ReturnMsgs.hpp"
#include "Filesystem.hpp"
#include "Log.hpp"
#include "RequestData.hpp"
#include "RouteInfo.hpp"
#include "Cgi.hpp"

namespace Get {
  ResponseData handle(const RouteInfo&);
}
