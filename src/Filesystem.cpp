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
    const char* what() const throw() {
      return this->err_msg.c_str();
    }

   private:
    std::string err_msg;
  };

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

  const std::string generateIndex(const std::string& index_path) {
    const std::set<std::string>& file_list = getDirListing(index_path);
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
    if (!data.location->index.empty()) {
      std::string index_path = data.full_path + data.location->index;
      if (exists(index_path)) {
        return readFile(index_path);
      }
    }
    if (data.location->autoindex) {
      return generateIndex(data.full_path);
    }
    return "";
  }
}
