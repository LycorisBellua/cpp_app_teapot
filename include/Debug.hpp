#pragma once

#include <cstdlib>   // IWYU pragma: keep
#include <iostream>  // IWYU pragma: keep

#include "Colors.hpp"  // IWYU pragma: keep
#include "Config.hpp"
#include "Router.hpp"

namespace Debug {
  void PrintConfig(const Config& conf);
  void PrintConfig(const Router& conf);
  void PrintLn(std::string msg);
}
