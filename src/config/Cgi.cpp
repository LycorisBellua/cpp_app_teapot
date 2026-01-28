#include "Cgi.hpp"
#include <string.h>

namespace {
  typedef std::vector<std::string>::iterator str_vec_it;
  typedef std::set<std::pair<std::string, std::string> >::iterator header_it;

  void removeCodeAndContentTypeHeaders(ResponseData& response) {
    for (std::set<std::pair<std::string, std::string> >::iterator it = response.headers.begin();
         it != response.headers.end();) {
      if (it->first == "Status" || it->first == "Content-Type") {
        response.headers.erase(it++);
      }
      else {
        ++it;
      }
    }
  }

  bool setStatus(const std::pair<std::string, std::string>& status_header, ResponseData& response) {
    std::vector<std::string> tokens = Helper::splitAtWhitespace(status_header.second);
    if (tokens.empty() || tokens[0].size() != 3 || tokens[0].find_first_not_of("1234567890") != std::string::npos) {
      Log::error("[CGI] Invalid status header" + status_header.first + " " + status_header.second);
      return false;
    }
    response.code = std::atoi(tokens[0].c_str());
    for (size_t i = 1; i < tokens.size(); ++i) {
      response.code_msg.append(tokens[i]);
      if (i + 1 < tokens.size()) {
        response.code_msg.push_back(' ');
      }
    }
    return true;
  }

  bool validateCgiHeaders(ResponseData& response) {
    bool hasStatus = false;
    bool hasContentType = false;
    bool hasLocation = false;

    for (header_it it = response.headers.begin(); it != response.headers.end(); ++it) {
      if (it->first == "Status") {
        hasStatus = true;
        if (!setStatus(*it, response)) {
          return false;
        }
      }
      else if (it->first == "Content-Type") {
        hasContentType = true;
        response.content_type = it->second;
      }
      else if (it->first == "Location") {
        hasLocation = true;
      }
    }

    if (!hasStatus && !hasContentType && !hasLocation) {
      Log::error("[CGI] Missing required headers (need Status or Content-Type)");
      return false;
    }

    if (!response.content.empty() && !hasContentType) {
      Log::error("[CGI] Missing required headers (need Content-Type when body is returned)");
      return false;
    }

    if (!hasStatus) {
      if (hasLocation) {
        response.code = 302;
        response.code_msg = "Found";
      }
      else {
        response.code = 200;
        response.code_msg = "OK";
      }
    }

    return true;
  }

  std::string getSplitMarker(const std::string& output) {
    return output.find("\n\n") == std::string::npos ? "\r\n\r\n" : "\n\n";
  }

  std::string splitBody(const std::string& output) {
    std::string split_marker = getSplitMarker(output);
    size_t split_pos = output.find(split_marker);
    if (split_pos == std::string::npos) {
      Log::error("[CGI] No header/body separator in cgi output");
      return "ERROR";
    }
    if (split_pos + split_marker.length() >= output.size()) {
      return "";
    }
    return output.substr(split_pos + split_marker.length());
  }

  std::set<std::pair<std::string, std::string> > splitHeaders(const std::string& output) {
    std::set<std::pair<std::string, std::string> > result;

    size_t split_pos = output.find(getSplitMarker(output));
    if (split_pos == std::string::npos) {
      Log::error("[CGI] No header/body separator in cgi output");
      return result;
    }

    std::string header_string = output.substr(0, split_pos);
    std::istringstream iss(header_string);
    std::string line;
    while (std::getline(iss, line)) {
      if (!line.empty() && line[line.size() - 1] == '\r') {
        line.erase(line.size() - 1);
      }

      size_t i = line.find(":");
      if (i == std::string::npos) {
        continue;
      }

      std::string key = line.substr(0, i);
      if (key.empty() || key.find_first_of(" \t\n\v\f\r") != std::string::npos) {
        Log::error("[CGI] CGI Output Header Key empty or contains whitespace");
        result.clear();
        return result;
      }
      ++i;

      std::string value;
      while (i < line.size() && std::isspace(static_cast<unsigned char>(line[i]))) {
        ++i;
      }
      if (i < line.size()) {
        value = Helper::trimWhitespaces(line.substr(i));
      }

      result.insert(std::make_pair(key, value));
    }
    return result;
  }

  ResponseData cgiOutput(const RouteInfo& data, const std::string& output) {
    Log::info("OUTPUT: " + output);
    ResponseData response;
    response.headers = splitHeaders(output);
    if (response.headers.empty()) {
      return ResponseData(502, data.server.errors);
    }
    response.content = splitBody(output);
    if (response.content == "ERROR") {
      return ResponseData(502, data.server.errors);
    }
    if (!validateCgiHeaders(response)) {
      return ResponseData(502, data.server.errors);
    }
    removeCodeAndContentTypeHeaders(response);
    return response;
  }

  /*ResponseData runScript(const RouteInfo& data, const std::vector<char*>& envPointers) {
    int stdin_pipe[2];
    int stdout_pipe[2];

    if (pipe(stdin_pipe) == -1) {
      Log::error("[CGI] Failed to set up stdin pipe");
      return ResponseData(500, data.server.errors);
    }

    if (pipe(stdout_pipe) == -1) {
      close(stdin_pipe[0]);
      close(stdin_pipe[1]);
      Log::error("[CGI] Failed to set up stdout pipe");
      return ResponseData(500, data.server.errors);
    }

    const pid_t pid = fork();

    if (pid == -1) {
      Log::error("[CGI] Failed to fork");
      close(stdin_pipe[0]);
      close(stdin_pipe[1]);
      close(stdout_pipe[0]);
      close(stdout_pipe[1]);
      return ResponseData(500, data.server.errors);
    }

    std::vector<char*> args;
    args.push_back(const_cast<char*>(data.cgi.interpreter.c_str()));
    args.push_back(const_cast<char*>(data.full_path.c_str()));
    args.push_back(NULL);

    // Child
    if (pid == 0) {
      close(stdin_pipe[1]);
      close(stdout_pipe[0]);
      dup2(stdin_pipe[0], STDIN_FILENO);
      close(stdin_pipe[0]);
      dup2(stdout_pipe[1], STDOUT_FILENO);
      close(stdout_pipe[1]);
      execve(data.cgi.interpreter.c_str(), args.data(), envPointers.data());
      _exit(1);
    }

    // Parent
    close(stdin_pipe[0]);
    close(stdout_pipe[1]);
    if (data.request.method == "POST" && !data.request.body.empty()) {
      write(stdin_pipe[1], data.request.body.c_str(), data.request.body.size());
    }
    close(stdin_pipe[1]);
    char buffer[4096];
    std::string output;
    ssize_t bytes_read;
    while ((bytes_read = read(stdout_pipe[0], buffer, sizeof(buffer))) > 0) {
      output.append(buffer, bytes_read);
    }
    close(stdout_pipe[0]);
    int status;
    waitpid(pid, &status, 0);

    if (WIFEXITED(status)) {
      int exit_code = WEXITSTATUS(status);
      if (exit_code != 0) {
        Log::error("[CGI] Script exited with code: " + Helper::nbrToString(exit_code));
        return ResponseData(500, data.server.errors);
      }
    } else if (WIFSIGNALED(status)) {
      int signal = WTERMSIG(status);
      Log::error("[CGI] Script killed by signal: " + Helper::nbrToString(signal));
      return ResponseData(500, data.server.errors);
    }

    return cgiOutput(data, output);
  }*/

ResponseData runScript(const RouteInfo& data, const std::vector<char*>& envPointers) {
  // Log all environment variables
  Log::info("[CGI] Environment variables:");
  for (size_t i = 0; envPointers[i] != NULL; i++) {
    Log::info("[CGI] env[" + Helper::nbrToString(i) + "]: " + std::string(envPointers[i]));
  }

  int stdin_pipe[2];
  int stdout_pipe[2];
  int stderr_pipe[2];

  if (pipe(stdin_pipe) == -1) {
    Log::error("[CGI] Failed to set up stdin pipe");
    return ResponseData(500, data.server.errors);
  }

  if (pipe(stdout_pipe) == -1) {
    close(stdin_pipe[0]);
    close(stdin_pipe[1]);
    Log::error("[CGI] Failed to set up stdout pipe");
    return ResponseData(500, data.server.errors);
  }

  if (pipe(stderr_pipe) == -1) {
    close(stdin_pipe[0]);
    close(stdin_pipe[1]);
    close(stdout_pipe[0]);
    close(stdout_pipe[1]);
    Log::error("[CGI] Failed to set up stderr pipe");
    return ResponseData(500, data.server.errors);
  }

  const pid_t pid = fork();

  if (pid == -1) {
    Log::error("[CGI] Failed to fork");
    close(stdin_pipe[0]);
    close(stdin_pipe[1]);
    close(stdout_pipe[0]);
    close(stdout_pipe[1]);
    close(stderr_pipe[0]);
    close(stderr_pipe[1]);
    return ResponseData(500, data.server.errors);
  }

  std::vector<char*> args;
  args.push_back(const_cast<char*>(data.cgi.interpreter.c_str()));
  args.push_back(const_cast<char*>(data.full_path.c_str()));
  args.push_back(NULL);

  // Child
  if (pid == 0) {
    close(stdin_pipe[1]);
    close(stdout_pipe[0]);
    close(stderr_pipe[0]);
    dup2(stdin_pipe[0], STDIN_FILENO);
    close(stdin_pipe[0]);
    dup2(stdout_pipe[1], STDOUT_FILENO);
    close(stdout_pipe[1]);
    dup2(stderr_pipe[1], STDERR_FILENO);
    close(stderr_pipe[1]);

    // DEBUG: Write before execve
    const char* msg = "Child: About to call execve\n";
    write(STDERR_FILENO, msg, strlen(msg));

    execve(data.cgi.interpreter.c_str(), args.data(), envPointers.data());

    // If we get here, execve failed
    const char* fail_msg = "Child: execve failed!\n";
    write(STDERR_FILENO, fail_msg, strlen(fail_msg));
    _exit(1);
  }

  // Parent
  close(stdin_pipe[0]);
  close(stdout_pipe[1]);
  close(stderr_pipe[1]);

  if (data.request.method == "POST" && !data.request.body.empty()) {
    write(stdin_pipe[1], data.request.body.c_str(), data.request.body.size());
  }
  close(stdin_pipe[1]);

  char buffer[4096];
  std::string output;
  ssize_t bytes_read;
  while ((bytes_read = read(stdout_pipe[0], buffer, sizeof(buffer))) > 0) {
    output.append(buffer, bytes_read);
  }
  close(stdout_pipe[0]);

  // Read stderr
  std::string error_output;
  char err_buffer[4096];
  ssize_t err_bytes;
  while ((err_bytes = read(stderr_pipe[0], err_buffer, sizeof(err_bytes))) > 0) {
    error_output.append(err_buffer, err_bytes);
  }
  close(stderr_pipe[0]);

  int status;
  waitpid(pid, &status, 0);

  // Log stderr if not empty
  if (!error_output.empty()) {
    Log::error("[CGI] stderr output: " + error_output);
  } else {
    Log::info("[CGI] No stderr output");
  }

  if (WIFEXITED(status)) {
    int exit_code = WEXITSTATUS(status);
    if (exit_code != 0) {
      Log::error("[CGI] Script exited with code: " + Helper::nbrToString(exit_code));
      return ResponseData(500, data.server.errors);
    }
  } else if (WIFSIGNALED(status)) {
    int signal = WTERMSIG(status);
    Log::error("[CGI] Script killed by signal: " + Helper::nbrToString(signal));
    return ResponseData(500, data.server.errors);
  }

  return cgiOutput(data, output);
}

  std::vector<char*> getEnvPointers(std::vector<std::string>& envStrings) {
    std::vector<char*> result;
    for (str_vec_it it = envStrings.begin(); it != envStrings.end(); ++it) {
      result.push_back(const_cast<char*>(it->c_str()));
    }
    result.push_back(NULL);
    return result;
  }

  std::vector<std::string> getEnvStrings(const RouteInfo& data) {
    std::vector<std::string> result;
    if (data.request.method == "POST") {
      result.push_back("CONTENT_LENGTH=" + Helper::nbrToString(data.request.body.size()));
      std::string c_type = (data.request.content_type.empty()) ? "application/octet-stream" : data.request.content_type;
      result.push_back("CONTENT_TYPE=" + c_type);
    }
    result.push_back("GATEWAY_INTERFACE=CGI/1.1");
    result.push_back("PATH_INFO=" + data.cgi.path_info);
    if (!data.cgi.path_info.empty()) {
      result.push_back("PATH_TRANSLATED=" + data.cgi.path_translated);
    }
    result.push_back("QUERY_STRING=" + data.query);
    result.push_back("REMOTE_ADDR=" + data.request.client_ip);
    result.push_back("REMOTE_HOST=" + data.request.client_ip);
    result.push_back("REQUEST_METHOD=" + data.request.method);
    result.push_back("SCRIPT_NAME=" + data.cgi.script_name);
    result.push_back("SERVER_NAME=" + data.request.host);
    result.push_back("SERVER_PORT=" + Helper::nbrToString(data.request.port));
    result.push_back("SERVER_PROTOCOL=" + data.request.protocol);
    result.push_back("SERVER_SOFTWARE=webserv");
    result.push_back("REDIRECT_STATUS=200");
    result.push_back("SCRIPT_FILENAME=" + data.full_path);

    return result;
  }

  std::vector<char*> getArgs(const RouteInfo& data) {
    std::vector<char*> result;
    result.push_back(const_cast<char*>(data.cgi.interpreter.c_str()));
    result.push_back(const_cast<char*>(data.full_path.c_str()));
    result.push_back(NULL);
    return result;
  }
}


namespace Cgi {
  ResponseData handle(const RouteInfo& data) {
    if (!Filesystem::exists(data.full_path)) {
      Log::error("[CGI] Requested Script does not exist: " + data.full_path);
      return ResponseData(404, data.server.errors);
    }
    std::vector<char*> args = getArgs(data);
    std::vector<std::string> envStrings = getEnvStrings(data);
    std::vector<char*> envPointers = getEnvPointers(envStrings);
    return runScript(data, envPointers);
  }
}