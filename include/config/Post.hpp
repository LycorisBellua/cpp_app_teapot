#pragma once

#include <dirent.h>

#include <algorithm>  // IWYU pragma: keep
#include <fstream>    // IWYU pragma: keep
#include <set>        // IWYU pragma: keep
#include <sstream>    // IWYU pragma: keep
#include <string>

#include "Cgi.hpp"                // IWYU pragma: keep
#include "Errors_ReturnMsgs.hpp"  // IWYU pragma: keep
#include "Filesystem.hpp"         // IWYU pragma: keep
#include "Log.hpp"                // IWYU pragma: keep
#include "RequestData.hpp"
#include "RouteInfo.hpp"

namespace Post {

  ResponseData handle(const RouteInfo&);

}
