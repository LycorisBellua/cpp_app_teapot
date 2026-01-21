#pragma once

#include <sys/wait.h>
#include <unistd.h>

#include <cstdlib>
#include <string>

#include "Errors_ReturnMsgs.hpp"  // IWYU pragma: keep
#include "Helper.hpp"
#include "RequestData.hpp"
#include "RouteInfo.hpp"

namespace Cgi {
  ResponseData handle(const RouteInfo&);
}