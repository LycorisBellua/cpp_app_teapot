#pragma once

#include <dirent.h>

#include <algorithm>
#include <fstream>
#include <set>
#include <sstream>
#include <string>

#include "Cgi.hpp"
#include "Errors_ReturnMsgs.hpp"
#include "Filesystem.hpp"
#include "Log.hpp"
#include "RequestData.hpp"
#include "RouteInfo.hpp"

namespace Post {
  ResponseData handle(const RouteInfo&);
}
