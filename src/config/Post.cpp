#include "Post.hpp"

namespace {

  typedef std::map<std::string, std::string>::const_iterator map_it;

  std::string getFileUrl(const std::string& filename, const RouteInfo& data) {
    const std::string& url_path = data.location.path;
    if (url_path[url_path.length() - 1] == '/') {
      return url_path + filename;
    }
    else {
      return url_path + '/' + filename;
    }
  }

  bool writeFile(std::ofstream& output_file, const std::string& filepath, const std::string& body) {
    if (!output_file.is_open()) {
      Log::error("[POST] File Not Created: Could Not Open File Stream:\n" + filepath);
    }
    output_file.write(body.c_str(), body.size());
    if (!output_file.good()) {
      output_file.close();
      std::remove(filepath.c_str());
      Log::error("[POST] File Not Created: Could not write to File Stream:\n" + filepath);
      return false;
    }
    output_file.close();
    Log::info("[POST] File Created: " + filepath);
    return true;
  }

  std::string lookupMime(const RouteInfo& data) {
    for (map_it it = data.mime_list.begin(); it != data.mime_list.end(); ++it) {
      if (it->second == data.request.content_type) {
        return it->first;
      }
    }
    Log::info("[POST] No matching file extension found for: " + data.request.content_type);
    return "";
  }

  std::string getUploadBase() {
    time_t now = std::time(0);
    struct tm* timeinfo = std::localtime(&now);
    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", timeinfo);
    std::stringstream base;
    base << "upload_" << buffer;
    return base.str();
  }

  std::string generateFilename(const RouteInfo& data) {
    const std::set<std::string> dir_listing = Filesystem::getDirListing(data.full_path);
    const std::string base = getUploadBase();
    const std::string extension = lookupMime(data);
    std::stringstream filename;
    filename << base << extension;
    size_t suffix = 0;
    while (dir_listing.find(filename.str()) != dir_listing.end()) {
      filename.str("");
      filename.clear();
      filename << base << "_" << ++suffix << extension;
    }
    return filename.str();
  }

  /*std::string extractFilename(const std::string& raw) {
    size_t i = raw.find('=');
    if (i == std::string::npos) {
      return "";
    }
    ++i;

    // Skip whitespace after '='
    while (i < raw.length() && std::isspace(static_cast<unsigned char>(raw[i]))) {
      ++i;
    }

    if (i >= raw.length()) {
      return "";
    }

    // Quoted value
    if (raw[i] == '\"') {
      ++i;  // Skip opening quote
      std::string result;

      while (i < raw.length()) {
        if (raw[i] == '\\' && i + 1 < raw.length()) {
          // Handle escape sequences: \\ and \"
          char next = raw[i + 1];
          if (next == '\\' || next == '\"') {
            result += next;
            i += 2;
            continue;
          }
        }
        if (raw[i] == '\"') {
          // Found closing quote - verify only whitespace remains
          ++i;
          while (i < raw.length()) {
            if (!std::isspace(static_cast<unsigned char>(raw[i]))) {
              return "";  // Junk after closing quote
            }
            ++i;
          }
          return result;
        }
        result += raw[i];
        ++i;
      }
      // No closing quote found - malformed
      return "";
    }
    // Unquoted value
    std::string result;
    while (i < raw.length() && !std::isspace(static_cast<unsigned char>(raw[i]))) {
      // Quotes not allowed in unquoted values
      if (raw[i] == '\"') {
        return "";
      }
      result += raw[i];
      ++i;
    }

    // Verify only whitespace remains after unquoted value
    while (i < raw.length()) {
      if (!std::isspace(static_cast<unsigned char>(raw[i]))) {
        return "";  // Junk after unquoted value
      }
      ++i;
    }

    return result;
  }

  std::pair<bool, size_t> findInVector(const std::vector<std::string>& vec, const std::string& to_find) {
    for (size_t index = 0; index < vec.size(); ++index) {
      if (vec[index].find(to_find) != std::string::npos) {
        return std::make_pair(true, index);
      }
    }
    return std::make_pair(false, 0);
  }

  std::vector<std::string> splitOnStr(const std::string& to_split, const std::string& separator) {
    std::string to_process = to_split;
    std::vector<std::string> result;
    while (!to_process.empty()) {
      size_t pos = to_process.find(separator);
      if (pos == std::string::npos) {
        result.push_back(to_process);
        break;
      }
      result.push_back(to_process.substr(0, pos));
      if (pos + separator.length() < to_process.length()) {
        to_process = to_process.substr(pos + separator.length());
      }
      else {
        break;
      }
    }
    return result;
  }

  ResponseData processParts(const RouteResponse& data, const std::vector<std::string>& parts) {
    for (str_vec_it it = parts.begin(); it != parts.end(); ++it) {
      const std::vector<std::string> lines = splitOnStr(*it, "\r\n");
      std::pair<bool, size_t> c_disp = findInVector(lines, "Content-Disposition");
      if (!c_disp.first) {
        Log::error("[POST] Missing Content-Disposition header in multipart body" + *it);
        return ResponseData(400, ErrorPage::get(400, data.server.errors));
      }
      const std::vector<std::string> content_disp = splitOnStr(lines[c_disp.second], ";");
      std::pair<bool, size_t> name = findInVector(content_disp, "filename");
      if (!name.first) {
        continue;
      }
      const std::string filename = extractFilename(content_disp[name.second]);
      if (filename.empty()) {
        return ResponseData(400, ErrorPage::get(400, data.server.errors));
      }
      std::pair<bool, size_t> c_type = findInVector(lines, "Content-Type");
    }
    return ResponseData(400, ErrorPage::get(400, data.server.errors));
  }

  std::vector<std::string> splitBody(const RouteResponse& data, const std::string& bound) {
    std::vector<std::string> split;
    std::string body = data.request.body;
    const std::string boundary = "--" + bound;
    bool final_delimiter = false;
    while (!body.empty()) {
      size_t pos = body.find(boundary);
      if (pos == std::string::npos) {
        break;
      }
      pos += boundary.length();
      if (pos + 1 < body.length() && body[pos] == '-' && body[pos + 1] == '-') {
        final_delimiter = true;
        break;
      }
      size_t end_pos = body.find(boundary, pos);
      if (end_pos == std::string::npos) {
        break;
      }
      split.push_back(body.substr(pos, end_pos - pos));
      body = body.substr(end_pos);
    }
    return (final_delimiter == true) ? split : std::vector<std::string>();
  }

  std::string extractBoundary(const std::string& content_type) {
    size_t pos = content_type.find("boundary=");
    if (pos == std::string::npos) {
      return "";
    }
    pos += 9;
    std::string value;
    for (size_t i = pos; i < content_type.length(); ++i) {
      char c = content_type[i];
      if (c == ';') {
        break;
      }
      if (c != '\"') {
        value.push_back(c);
      }
    }
    return value;
  }

  ResponseData multipartUpload(const RouteResponse& data) {
    const std::string boundary = extractBoundary(data.request.content_type);
    if (boundary.empty()) {
      Log::error("[POST] No boundary value in Content-Type Header: " + data.request.content_type);
      return ResponseData(400, ErrorPage::get(400, data.server.errors));
    }
    std::vector<std::string> parts = splitBody(data, boundary);
    if (parts.empty()) {
      Log::error("[POST] No final boundary found in multipart body");
      return ResponseData(400, ErrorPage::get(400, data.server.errors));
    }
    return processParts(data, parts);
  }

  ResponseData handleUpload(const RouteResponse& data) {
    if (data.request.content_type.find("multipart/form-data") != std::string::npos) {
      return multipartUpload(data);
    }
    else {
      return simpleUpload(data);
    }
  }*/

  ResponseData simpleUpload(const RouteInfo& data) {
    const std::string filename = generateFilename(data);
    const std::string filepath = data.full_path + filename;
    std::ofstream output_file(filepath.c_str(), std::ios::binary);
    if (!writeFile(output_file, filename, data.request.body)) {
      return ResponseData(500, data.server.errors);
    }
    return ResponseData(201, getFileUrl(filename, data), "text/plain");
  }

  bool isUpload(const RouteInfo& data) {
    const std::string& upload_path = data.location.upload_path;
    if (upload_path.empty()) {
      Log::error("[POST] No upload path specified in config file");
      return false;
    }
    // return data.request.content_type.find("multipart/form-data") != std::string::npos;
    if (data.request.body.empty()) {
      Log::error("[POST] No body to upload");
      return false;
    }
    return true;
  }

  bool isCgi(const RouteInfo& data) {
    const std::map<std::string, std::string>& cgi = data.location.cgi;
    if (cgi.empty()) {
      return false;
    }
    std::string extension = Filesystem::getfileExtension(data.full_path);
    return cgi.find(extension) != cgi.end();
  }

  bool bodySizeCheck(const RouteInfo& data) {
    bool ok = data.request.body.size() <= data.server.client_body_max;
    if (!ok) {
      Log::error("[POST] Request body exceeds max size");
    }
    return ok;
  }

}

namespace Post {

  ResponseData handle(const RouteInfo& data) {
    if (!bodySizeCheck(data)) {
      return ResponseData(413, data.server.errors);
    }
    if (isCgi(data)) {
      return Cgi::handle(data);
    }
    if (isUpload(data)) {
      // return handleUpload(data);
      return simpleUpload(data);
    }
    return ResponseData(400, data.server.errors);

    /*    const std::string filename = generateFilename(data.location.upload_path);
    const std::string filepath(data.location.upload_path + filename);
    std::ofstream output(filepath.c_str(), std::ios::binary);
    if (!writeFile(output, filepath, data.request.body)) {
      return ResponseData(500, ErrorPage::get(500, data.error_pages));
    }
    return ResponseData(201, getFileUrl(filename, data));*/
  }

}
