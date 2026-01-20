#pragma once

#include <errno.h>  // IWYU pragma: keep
#include <unistd.h>

#include "ErrorPage.hpp"   // IWYU pragma: keep
#include "Filesystem.hpp"  // IWYU pragma: keep
#include "RequestData.hpp"
#include "Log.hpp"  // IWYU pragma: keep
#include "RouteInfo.hpp"

namespace Delete {

  ResponseData handle(const RouteInfo&);

}
