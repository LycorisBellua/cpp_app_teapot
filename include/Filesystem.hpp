#pragma once

#include <sys/stat.h>
#include <unistd.h>

#include <sstream>
#include <string>

#include "Log.hpp"  // IWYU pragma: keep

namespace Filesystem {
  bool exists(const std::string&);
  bool isDir(const std::string&);
  bool isRegularFile(const std::string&);
  bool isExecutable(const std::string&);
  std::string getCurrentDir();
  std::string normalisePaths(const std::string&, std::string);

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
