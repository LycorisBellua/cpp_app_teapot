#pragma once

#include <dirent.h>

#include <algorithm>  // IWYU pragma: keep
#include <cstdio>
#include <ctime>
#include <fstream>  // IWYU pragma: keep
#include <set>      // IWYU pragma: keep
#include <sstream>  // IWYU pragma: keep
#include <string>

#include "ErrorPage.hpp"   // IWYU pragma: keep
#include "Filesystem.hpp"  // IWYU pragma: keep
#include "HttpResponse.hpp"
#include "Log.hpp"  // IWYU pragma: keep
#include "RouteInfo.hpp"

namespace Post {

  HttpResponse upload(const RouteResponse&, const std::string&);

}
