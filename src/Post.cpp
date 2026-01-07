#include "../include/Post.hpp"

namespace {

  std::string getUploadBase() {
    time_t now = time(0);
    struct tm* timeinfo = localtime(&now);
    char buffer[100];
    strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", timeinfo);
    std::stringstream base;
    base << "upload_" << buffer;
    return base.str();
  }

  std::set<std::string> getDirListing(const std::string& index_path) {
    std::set<std::string> dir_listing;
    DIR* dir = opendir(index_path.c_str());
    if (!dir) {
      Log::error("[Upload] Unable to open directory: " + index_path);
      return dir_listing;
    }
    struct dirent* file;
    while ((file = readdir(dir)) != NULL) {
      dir_listing.insert(file->d_name);
    }
    closedir(dir);
    return dir_listing;
  }

  std::string generateFilename(const std::string& path) {
    const std::set<std::string> dir_listing = getDirListing(path);
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

}

namespace Post {

  int upload(const RouteResponse& data, const std::string& body) {
    const std::string& upload_path = data.location->upload_path;
    const std::vector<std::string> methods = data.location->allowed_methods;
    if (std::find(methods.begin(), methods.end(), "POST") == methods.end()) {
      return 405;
    }
    if (upload_path.empty() || !Filesystem::exists(upload_path) || !Filesystem::isDir(upload_path)) {
      return 404;
    }
    const std::string filename = generateFilename(upload_path);
    if (filename.empty()) {
      return 404;
    }
    const std::string full_filepath(data.location->upload_path + filename);
    std::ofstream output(full_filepath.c_str(), std::ios::binary);
    if (!output.is_open()) {
      return 404;
    }
    output.write(body.c_str(), body.size());
    if (!output.good()) {
      output.close();
      remove(full_filepath.c_str());
      return 500;
    }
    output.close();
    return 200;
  }

}
