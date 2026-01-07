#include "../include/Get.hpp"

namespace {

  typedef std::set<std::string>::const_iterator fl_it;

  std::set<std::string> getDirListing(const std::string& index_path) {
    std::set<std::string> dir_listing;
    DIR* dir = opendir(index_path.c_str());
    if (!dir) {
      Log::error("[Autoindex] Unable to open directory: " + index_path);
      return dir_listing;
    }
    struct dirent* file;
    while ((file = readdir(dir)) != NULL) {
      dir_listing.insert(file->d_name);
    }
    closedir(dir);
    return dir_listing;
  }

  std::string generateIndex(const std::string& index_path, const std::string& uri_path) {
    const std::set<std::string> file_list = getDirListing(index_path);
    if (file_list.empty()) {
      return "";
    }
    std::stringstream html;
    html << "<!doctype html><html><head><title>Index</title></head>"
         << "<body style=\"text-align: center\">"
         << "<h1><strong>Index</strong></h1>";
    std::string fixed_uri_path = uri_path;
    if (uri_path[uri_path.length() - 1] != '/') {
      fixed_uri_path.push_back('/');
    }
    for (fl_it file = file_list.begin(); file != file_list.end(); ++file) {
      if (*file != "." && *file != "..") {
        html << "<p><a href=" << fixed_uri_path << *file << ">" << *file << "</a></p>";
      }
    }
    html << "</body></html>";
    return html.str();
  }

  std::pair<bool, std::string> readFile(const std::string& path) {
    std::ifstream file(path.c_str(), std::ios::binary);
    if (!file) {
      Log::error("Unable to open file: " + path);
      return std::make_pair(false, "");
    }

    file.seekg(0, std::ios::end);
    size_t end = file.tellg();
    file.seekg(0, std::ios::beg);

    std::string content(end, '\0');
    file.read(&content[0], end);
    file.close();
    return std::make_pair(true, content);
  }

  HttpResponse handleFile(const std::string& path) {
    if (!Filesystem::exists(path)) {
      return HttpResponse(404, "", "Requested File Not Found");
    }
    const std::pair<bool, std::string> filebuf = readFile(path);
    if (!filebuf.first) {
      return HttpResponse(403, "", "Forbidden");
    }
    return HttpResponse(200, filebuf.second, "");
  }

  HttpResponse handleDirectory(const RouteResponse& data) {
    if (!data.location->index.empty()) {
      if (Filesystem::exists(data.full_path + data.location->index)) {
        const std::pair<bool, std::string> indexbuf = readFile(data.full_path + data.location->index);
        if (indexbuf.first) {
          return HttpResponse(200, indexbuf.second, "");
        }
      }
    }
    if (data.location->autoindex) {
      const std::string indexbuf = generateIndex(data.full_path, data.location->path);
      if (!indexbuf.empty()) {
        return HttpResponse(200, indexbuf, "");
      }
    }
    return HttpResponse(403, "", "Forbidden");
  }

}

namespace Get {

  HttpResponse handle(const RouteResponse& data) {
    if (Filesystem::isDir(data.full_path)) {
      return handleDirectory(data);
    }
    else if (Filesystem::isRegularFile(data.full_path)) {
      return handleFile(data.full_path);
    }
    return HttpResponse(404, "", "Requested File Not Found");
  }

}
