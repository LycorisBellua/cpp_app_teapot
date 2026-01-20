#pragma once

#include <cstdlib>   // IWYU pragma: keep
#include <iostream>  // IWYU pragma: keep

#include "Colors.hpp"  // IWYU pragma: keep
#include "Config.hpp"
#include "RequestData.hpp"
#include "Router.hpp"

namespace Debug {
  void PrintConfig(const Config&);
  void PrintConfig(const Router&);
  void PrintRouteInfo(const RouteInfo&);
  void PrintPorts(const std::set<std::pair<std::string, int> >&);
  void PrintLn(std::string msg);
  void PrintResponseData(const ResponseData&);
}
