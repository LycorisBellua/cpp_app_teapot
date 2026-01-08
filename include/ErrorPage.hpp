#pragma once

#include <map>
#include <string>

#include "Filesystem.hpp"  // IWYU pragma: keep

namespace ErrorPage {
  std::string get(int, const std::map<int, std::string>&);
  std::string get(int);
}
