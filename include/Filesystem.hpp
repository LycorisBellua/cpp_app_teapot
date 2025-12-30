#pragma once

#include <sys/stat.h>

#include <string>

namespace Filesystem {
  bool exists(const std::string&);
  bool isDir(const std::string&);
}
