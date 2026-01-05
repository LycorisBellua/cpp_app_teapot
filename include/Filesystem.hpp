#pragma once

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#include <fstream>  // IWYU pragma: keep
#include <set>      // IWYU pragma: keep
#include <sstream>
#include <string>

#include "../include/RouteInfo.hpp"
#include "Log.hpp"  // IWYU pragma: keep

namespace Filesystem {
  bool exists(const std::string&);
  bool isDir(const std::string&);
  bool isRegularFile(const std::string&);
  bool isExecutable(const std::string&);
  std::string getCurrentDir();
  std::string normalisePaths(const std::string&, std::string);
  const std::string readFile(const std::string&);
  const std::string serveDir(const RouteResponse&);

  template <typename Code>
  const std::string generateErrorPage(Code code, const std::string& msg) {
    std::stringstream html;
    html << "<!doctype html><head><title>" << code << "</title></head>"
         << "<body><style=\"text-align: center\">"
         << "<h1><strong>Error</strong></h1>"
         << "<h2><strong>" << code << "</strong></h2>"
         << "<p>" << msg << "</p></body></html>";
    return html.str();
  }
}
