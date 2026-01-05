#include "../include/Filesystem.hpp"

namespace {

  class FilesystemError : public std::exception {
   public:
    FilesystemError(const std::string& msg, const std::string& path) {
      std::stringstream err_string;
      err_string << "[Filesystem] " << msg << "\nPath: " << path << "\n\n";
      Log::error(err_string.str());
      err_msg = err_string.str();
    }
    ~FilesystemError() throw() {}
    const char* what() const throw() {
      return this->err_msg.c_str();
    }

   private:
    std::string err_msg;
  };

  typedef std::set<std::string>::const_iterator fl_it;

  const std::set<std::string> getDirListing(const std::string& index_path) {
    std::set<std::string> dir_listing;
    DIR* dir = opendir(index_path.c_str());
    if (!dir) {
      throw FilesystemError("Unable to open index file.", index_path);
    }
    struct dirent* file;
    while ((file = readdir(dir)) != NULL) {
      dir_listing.insert(file->d_name);
    }
    return dir_listing;
  }

  const std::string generateIndex(const std::string& index_path, const std::string& uri_path) {
    std::set<std::string> file_list;
    try {
      file_list = getDirListing(index_path);
    } catch (const FilesystemError& e) {
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
}

namespace Filesystem {

  bool exists(const std::string& path) {
    struct stat st;
    return stat(path.c_str(), &st) == 0;
  }

  bool isDir(const std::string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) != 0) {
      return false;
    }
    return S_ISDIR(st.st_mode);
  }

  bool isRegularFile(const std::string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) != 0) {
      return false;
    }
    return S_ISREG(st.st_mode);
  }

  bool isExecutable(const std::string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) != 0) {
      return false;
    }
    return (st.st_mode & S_IXUSR);
  }

  std::string getCurrentDir() {
    char buffer[4096];
    getcwd(buffer, 4096);
    return buffer;
  }

  std::string normalisePaths(const std::string& path, std::string path_root) {
    if (path.empty()) {
      return path;
    }
    std::string normalised = path;
    if (normalised[0] != '/') {
      std::stringstream absolute_path;
      absolute_path << path_root;
      if (path_root[path_root.length() - 1] != '/') {
        absolute_path << "/";
      }
      absolute_path << path;
      normalised = absolute_path.str();
    }
    if (isDir(normalised) && normalised[normalised.length() - 1] != '/') {
      normalised.push_back('/');
    }
    return normalised;
  }

  const std::string readFile(const std::string& path) {
    std::ifstream file(path.c_str(), std::ios::binary);
    if (!file) {
      Log::error("Unable to open file: " + path);
      return "";
    }

    file.seekg(0, std::ios::end);
    size_t end = file.tellg();
    file.seekg(0, std::ios::beg);

    std::string content(end, '\0');
    file.read(&content[0], end);
    file.close();
    return content;
  }

  const std::string serveDir(const RouteResponse& data) {
    if (!isDir(data.full_path)) {
      return "";
    }
    if (!data.location->index.empty()) {
      std::string index_path = data.full_path + data.location->index;
      if (exists(index_path)) {
        return readFile(index_path);
      }
    }
    if (data.location->autoindex) {
      return generateIndex(data.full_path, data.location->path);
    }
    return "";
  }

}
