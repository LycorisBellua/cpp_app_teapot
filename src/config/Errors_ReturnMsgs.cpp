#include "Errors_ReturnMsgs.hpp"

namespace {

  typedef std::map<int, std::string>::const_iterator error_it;

  std::string getDefaultMessage(const int code) {
    static std::map<int, std::string> messages;
    if (messages.empty()) {
      messages[100] = "Continue";
      messages[101] = "Switching Protocols";
      messages[102] = "Processing";
      messages[103] = "Early Hints";
      messages[200] = "OK";
      messages[201] = "Created";
      messages[202] = "Accepted";
      messages[203] = "Non-Authoritive Information";
      messages[204] = "No Content";
      messages[205] = "Reset Content";
      messages[206] = "Partial Content";
      messages[207] = "Multi-Status";
      messages[208] = "Already Reported";
      messages[226] = "IM Used";
      messages[300] = "Multiple Choices";
      messages[301] = "Moved Permanently";
      messages[302] = "Found";
      messages[303] = "See Other";
      messages[304] = "Not Modified";
      messages[307] = "Temporary Redirect";
      messages[308] = "Permanent Redirect";
      messages[400] = "Bad Request";
      messages[401] = "Unauthorized";
      messages[402] = "Payment Required";
      messages[403] = "Forbidden";
      messages[404] = "Not Found";
      messages[405] = "Method Not Allowed";
      messages[406] = "Not Acceptable";
      messages[407] = "Proxy Authentication Required";
      messages[408] = "Request Timeout";
      messages[409] = "Conflict";
      messages[410] = "Gone";
      messages[411] = "Length Required";
      messages[412] = "Precondition Failed";
      messages[413] = "Content Too Large";
      messages[414] = "URI Too Long";
      messages[415] = "Unsupported Media Type";
      messages[416] = "Range Not Satisfiable";
      messages[417] = "Expectation Failed";
      messages[418] = "I'm A Teapot";
      messages[421] = "Misdirected Request";
      messages[422] = "Unprocessable Content";
      messages[423] = "Locked";
      messages[424] = "Failed Dependency";
      messages[425] = "Too Early";
      messages[426] = "Upgrade Required";
      messages[428] = "Precondition Required";
      messages[429] = "Too Many Requests";
      messages[431] = "Request Header Fields Too Large";
      messages[451] = "Unavailable For Legal Reasons";
      messages[500] = "Internal Server Error";
      messages[501] = "Not Implemented";
      messages[502] = "Bad Gateway";
      messages[503] = "Service Unavailable";
      messages[504] = "Gateway Timeout";
      messages[505] = "HTTP Version Not Supported";
      messages[506] = "Proxy Authentication Required";
      messages[507] = "Insufficient Storage";
      messages[508] = "Loop Detected";
      messages[510] = "Not Extended";
      messages[511] = "Network Authentication Required";
    }
    const error_it found = messages.find(code);
    return found == messages.end() ? "" : found->second;
  }

  std::string getDefaultErrorPage(const int code) {
    std::stringstream html;
    html << "<!doctype html><head><title>" << code << "</title></head>"
         << "<body style=\"text-align: center\">"
         << "<h1><strong>Error</strong></h1>"
         << "<h2><strong>" << code << "</strong></h2>"
         << "<p>" << getDefaultMessage(code) << "</p></body></html>";
    return html.str();
  }

  std::string getUserErrorPage(const int code, const std::map<int, std::string>& errors) {
    const error_it found = errors.find(code);
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

  std::string get(const int code, const std::map<int, std::string>& user_specified) {
    const std::string html = getUserErrorPage(code, user_specified);
    if (!html.empty()) {
      return html;
    }
    else {
      return get(code);
    }
  }

  std::string get(const int code) {
    return getDefaultErrorPage(code);
  }

}

namespace StatusMsg {
  std::string get(const int code) {
    return getDefaultMessage(code);
  }
}
