#pragma once

#include <errno.h>  // IWYU pragma: keep
#include <unistd.h>

#include "Errors_ReturnMsgs.hpp"  // IWYU pragma: keep
#include "Filesystem.hpp"         // IWYU pragma: keep
#include "Log.hpp"                // IWYU pragma: keep
#include "RequestData.hpp"
#include "RouteInfo.hpp"

namespace Delete {

  ResponseData handle(const RouteInfo&);

}
