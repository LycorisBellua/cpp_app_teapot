#include <ctime>
#include <fstream>
#include <iostream>

#include "../include/Log.hpp"

namespace {

  void writeLog(const std::string& msg_type, const std::string& msg) {
    std::ofstream logfile("/tmp/webserv.log", std::ios::app);
    if (!logfile.is_open()) {
      return;
    }

    time_t now = time(0);
    struct tm* timeinfo = localtime(&now);
    char buffer[100];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

    logfile << buffer << " " << msg_type << "\n" << msg << "\n\n";
  }

}

namespace Log {

  void error(const std::string& msg) {
    writeLog("ERROR", msg);
  }

  void errorPrint(const std::string& msg) {
    std::cerr << msg << std::endl;
    error(msg);
  }

  void info(const std::string& msg) {
    writeLog("INFO", msg);
  }

  void infoPrint(const std::string& msg) {
    std::cout << msg << std::endl;
    info(msg);
  }
}
