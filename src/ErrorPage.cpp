#include "../include/ErrorPage.hpp"

namespace {

  typedef std::map<int, std::string>::const_iterator error_it;

  std::string getDefaultMessage(int code) {
    static std::map<int, std::string> errors;
    if (errors.empty()) {
      errors[400] = "Bad Request";
      errors[403] = "Forbidden";
      errors[404] = "Not Found";
      errors[405] = "Method Not Allowed";
      errors[413] = "Content Too Large";
      errors[500] = "Internal Server Error";
    }
    error_it found = errors.find(code);
    return found == errors.end() ? "" : found->second;
  }

  std::string getDefaultErrorPage(int code) {
    const std::string message = getDefaultMessage(code);
    std::stringstream html;
    html << "<!doctype html><head><title>" << code << "</title></head>"
         << "<body style=\"text-align: center\">"
         << "<h1><strong>Error</strong></h1>"
         << "<h2><strong>" << code << "</strong></h2>"
         << "<p>" << message << "</p></body></html>";
    return html.str();
  }

  std::string getUserErrorPage(int code, const std::map<int, std::string>& errors) {
    error_it found = errors.find(code);
    if (found != errors.end()) {
      std::pair<bool, std::string> html = Filesystem::readFile(found->second);
      if (!html.second.empty()) {
        return html.second;
      }
    }
    return "";
  }

}

namespace ErrorPage {

  std::string get(int code, const std::map<int, std::string>& user_specified) {
    const std::string html = getUserErrorPage(code, user_specified);
    if (!html.empty()) {
      return html;
    }
    else {
      return get(code);
    }
  }

  std::string get(int code) {
    return getDefaultErrorPage(code);
  }

}
