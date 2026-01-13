#include "../include/Post.hpp"

namespace {

  std::string getFileUrl(const std::string& filename, const RouteResponse& data) {
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
  }

  bool bodySizeCheck(size_t size_limit, const std::string& body) {
    bool ok = body.size() <= size_limit;
    if (!ok) {
      Log::error("[POST] Request body exceeds max size");
    }
    return ok;
  }

  bool uploadPathCheck(const std::string& upload_path) {
    if (upload_path.empty()) {
      Log::error("[POST] No upload path specified in config file");
      return false;
    }
    if (!Filesystem::exists(upload_path)) {
      Log::error("[POST] File Not Created: Upload path does not exist:\n" + upload_path);
      return false;
    }
    if (!Filesystem::isDir(upload_path)) {
      Log::error("[POST] File Not Created: Upload path is not a directory:\n" + upload_path);
      return false;
    }
    return true;
  }

}

namespace Post {

  HttpResponse upload(const RouteResponse& data, const std::string& body) {
    /*if (isCgi(data)) {
      return handleCgi(data, body);
    }
    if (isUpload(data)) {
      return handleUpload(data, body);
    }*/
    if (!uploadPathCheck(data.location.upload_path)) {
      return HttpResponse(500, ErrorPage::get(500, data.error_pages));
    }
    if (!bodySizeCheck(data.client_body_max, body)) {
      return HttpResponse(413, ErrorPage::get(413, data.error_pages));
    }
    const std::string filename = generateFilename(data.location.upload_path);
    const std::string filepath(data.location.upload_path + filename);
    std::ofstream output(filepath.c_str(), std::ios::binary);
    if (!writeFile(output, filepath, body)) {
      return HttpResponse(500, ErrorPage::get(500, data.error_pages));
    }
    return HttpResponse(201, getFileUrl(filename, data));
  }

}
