#pragma once

#include <map>
#include <string>

#include "Filesystem.hpp"

namespace ErrorPage {
  std::string get(const int, const std::map<int, std::string>&);
  std::string get(const int);
}

namespace StatusMsg {
  std::string get(const int);
}
