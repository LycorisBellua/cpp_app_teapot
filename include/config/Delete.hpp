#pragma once

#include <errno.h>
#include <unistd.h>

#include "Errors_ReturnMsgs.hpp"
#include "Filesystem.hpp"
#include "Log.hpp"
#include "RequestData.hpp"
#include "RouteInfo.hpp"

namespace Delete {
  ResponseData handle(const RouteInfo&);
}
