#include "Post.hpp"

namespace {

  /*std::string getFileUrl(const std::string& filename, const RouteResponse& data) {
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
      remove(filepath.c_str());
      Log::error("[POST] File Not Created: Could not write to File Stream:\n" + filepath);
      return false;
    }
    output_file.close();
    Log::info("[POST] File Created: " + filepath);
    return true;
  }

  std::string getUploadBase() {
    time_t now = time(0);
    struct tm* timeinfo = localtime(&now);
    char buffer[100];
    strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", timeinfo);
    std::stringstream base;
    base << "upload_" << buffer;
    return base.str();
  }

  std::string generateFilename(const std::string& path) {
    const std::set<std::string> dir_listing = Filesystem::getDirListing(path);
    const std::string base = getUploadBase();
    std::stringstream filename;
    filename << base;
    size_t suffix = 0;
    while (dir_listing.find(filename.str()) != dir_listing.end()) {
      filename.str("");
      filename.clear();
      filename << base << "_" << ++suffix;
    }
    return filename.str();
  }*/

  std::vector<std::string> splitBody(const RouteResponse& data, const std::string& boundary) {
    std::vector<std::string> split;
    std::string body("--" + data.request.body);
    while (!body.empty()) {
      size_t pos = body.find(boundary);
      if (pos == std::string::npos) {
        break;
      }
      body = body.substr(pos + boundary.length());
    }
    return split;
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

  HttpResponse handleUpload(const RouteResponse& data) {
    const std::string boundary = extractBoundary(data.request.content_type);
    if (boundary.empty()) {
      Log::error("[POST] No boundary value in Content-Type Header: " + data.request.content_type);
      return HttpResponse(400, ErrorPage::get(400, data.error_pages));
    }
    std::vector<std::string> parts = splitBody(data, boundary);
    return HttpResponse(500, ErrorPage::get(500, data.error_pages));
  }

  bool isUpload(const RouteResponse& data) {
    const std::string& upload_path = data.location.upload_path;
    if (upload_path.empty()) {
      Log::error("[POST] No upload path specified in config file");
      return false;
    }
    return data.request.content_type.find("multipart/form-data") != std::string::npos;
  }

  bool isCgi(const RouteResponse& data) {
    const std::string& cgi_ext = data.location.cgi_extension;
    if (cgi_ext.empty()) {
      return false;
    }
    std::string extension = Filesystem::getfileExtension(data.full_path);
    return extension == cgi_ext;
  }

  bool bodySizeCheck(const RouteResponse& data) {
    bool ok = data.request.body.size() <= data.client_body_max;
    if (!ok) {
      Log::error("[POST] Request body exceeds max size");
    }
    return ok;
  }

}

namespace Post {

  HttpResponse handle(const RouteResponse& data) {
    if (!bodySizeCheck(data)) {
      return HttpResponse(413, ErrorPage::get(413, data.error_pages));
    }
    if (isCgi(data)) {
      return Cgi::handle(data);
    }
    if (isUpload(data)) {
      return handleUpload(data);
    }
    // TODO: Check fallback return code
    return HttpResponse(400, ErrorPage::get(500, data.error_pages));

    /*    const std::string filename = generateFilename(data.location.upload_path);
    const std::string filepath(data.location.upload_path + filename);
    std::ofstream output(filepath.c_str(), std::ios::binary);
    if (!writeFile(output, filepath, data.request.body)) {
      return HttpResponse(500, ErrorPage::get(500, data.error_pages));
    }
    return HttpResponse(201, getFileUrl(filename, data));*/
  }

}
