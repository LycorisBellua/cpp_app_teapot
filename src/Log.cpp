#include "../include/Log.hpp"

namespace {

  void writeLog(std::ostream& os, const std::string& msg_type, const std::string& msg) {
    time_t now = time(0);
    struct tm* timeinfo = localtime(&now);
    char buffer[100];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

    os << buffer << " " << msg_type << "\n" << msg << "\n\n";
  }

}

namespace Log {

  void error(const std::string& msg) {
    writeLog(std::cerr, "ERROR", msg);
  }

  void info(const std::string& msg) {
    writeLog(std::cout, "INFO", msg);
  }

}
