#include "../include/Get.hpp"

namespace {

  typedef std::set<std::string>::const_iterator fl_it;

  std::string generateIndex(const std::string& index_path, const std::string& uri_path) {
    const std::set<std::string> file_list = Filesystem::getDirListing(index_path);
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

  HttpResponse handleFile(const RouteResponse& data) {
    const std::string& path = data.full_path;
    if (!Filesystem::exists(path)) {
      Log::error("[GET/HEAD] File Not Found: " + path);
      return HttpResponse(404, ErrorPage::get(404, data.error_pages));
    }
    const std::pair<bool, std::string> filebuf = Filesystem::readFile(path);
    if (!filebuf.first) {
      return HttpResponse(403, ErrorPage::get(403, data.error_pages));
    }
    Log::info("[GET/HEAD] Serving file: " + path);
    return HttpResponse(200, filebuf.second);
  }

  HttpResponse handleDirectory(const RouteResponse& data) {
    if (!data.location.index.empty()) {
      if (Filesystem::exists(data.full_path + data.location.index)) {
        const std::pair<bool, std::string> indexbuf = Filesystem::readFile(data.full_path + data.location.index);
        if (indexbuf.first) {
          Log::info("[GET/HEAD] Serving Index File: " + data.full_path + data.location.index);
          return HttpResponse(200, indexbuf.second);
        }
      }
    }
    if (data.location.autoindex) {
      const std::string indexbuf = generateIndex(data.full_path, data.location.path);
      if (!indexbuf.empty()) {
        Log::info("[GET/HEAD] Serving autoindex for: " + data.full_path);
        return HttpResponse(200, indexbuf);
      }
    }
    Log::error("[GET/HEAD] No index file specified and autoindex not active: " + data.full_path);
    return HttpResponse(403, ErrorPage::get(403, data.error_pages));
  }

}

namespace Get {

  HttpResponse handle(const RouteResponse& data) {
    if (Filesystem::isDir(data.full_path)) {
      return handleDirectory(data);
    }
    else if (Filesystem::isRegularFile(data.full_path)) {
      return handleFile(data);
    }
    Log::error("[GET/HEAD] Requested resource is not a directory or regular file: " + data.full_path);
    return HttpResponse(404, ErrorPage::get(404, data.error_pages));
  }

}
