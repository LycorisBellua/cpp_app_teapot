#pragma once

#include <dirent.h>

#include <fstream>  // IWYU pragma: keep
#include <set>      // IWYU pragma: keep
#include <sstream>  // IWYU pragma: keep
#include <string>   // IWYU pragma: keep

#include "Filesystem.hpp"  // IWYU pragma: keep
#include "HttpResponse.hpp"
#include "Log.hpp"  // IWYU pragma: keep
#include "RouteInfo.hpp"

namespace Get {
  HttpResponse handle(const RouteResponse&);
}
