#pragma once

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#include <fstream>  // IWYU pragma: keep
#include <set>      // IWYU pragma: keep
#include <sstream>  // IWYU pragma: keep
#include <string>

#include "Log.hpp"  // IWYU pragma: keep

namespace Filesystem {

  bool exists(const std::string&);
  bool isDir(const std::string&);
  bool isRegularFile(const std::string&);
  bool isExecutable(const std::string&);
  std::string getCurrentDir();
  std::string normalisePaths(const std::string&, std::string);
  std::pair<bool, std::string> readFile(const std::string&);
  std::set<std::string> getDirListing(const std::string&);
  std::string getfileExtension(const std::string&);

}
