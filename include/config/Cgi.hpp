#pragma once

#include "ErrorPage.hpp"  // IWYU pragma: keep
#include "RequestData.hpp"
#include "RouteInfo.hpp"
#include "Helper.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <string>

namespace Cgi {
  ResponseData handle(const RouteInfo&);
}