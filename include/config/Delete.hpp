#pragma once

#include <errno.h>  // IWYU pragma: keep
#include <unistd.h>

#include "ErrorPage.hpp"   // IWYU pragma: keep
#include "Filesystem.hpp"  // IWYU pragma: keep
#include "HttpResponse.hpp"
#include "Log.hpp"  // IWYU pragma: keep
#include "RouteInfo.hpp"

namespace Delete {

  HttpResponse handle(const RouteResponse&);

}
