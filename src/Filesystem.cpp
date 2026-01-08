#include "../include/Filesystem.hpp"

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
    (void)getcwd(buffer, 4096);
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

  std::pair<bool, std::string> readFile(const std::string& path) {
    std::ifstream file(path.c_str(), std::ios::binary);
    if (!file) {
      Log::error("[READ FILE] Unable to open file: " + path);
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

  std::set<std::string> getDirListing(const std::string& index_path) {
    std::set<std::string> dir_listing;
    DIR* dir = opendir(index_path.c_str());
    if (!dir) {
      Log::error("[DIRECTORY LISTING] Unable to open directory: " + index_path);
      return dir_listing;
    }
    struct dirent* file;
    while ((file = readdir(dir)) != NULL) {
      dir_listing.insert(file->d_name);
    }
    closedir(dir);
    return dir_listing;
  }

}
