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

  ResponseData uploadFile(const RouteInfo& data) {
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
    if (data.request.body.empty()) {
      Log::error("[POST] No body to upload");
      return false;
    }
    return true;
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
    if (isUpload(data)) {
      return uploadFile(data);
    }
    return ResponseData(400, data.server.errors);
  }

}
