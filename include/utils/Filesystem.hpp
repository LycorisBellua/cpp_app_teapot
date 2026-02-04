#pragma once

#include <string>
#include <set>

namespace Filesystem
{
	bool exists(const std::string& path);
	bool isDir(const std::string& path);
	bool isRegularFile(const std::string& path);
	bool isExecutable(const std::string& path);
	std::string getCurrentDir();
	std::string normalisePaths(const std::string& path, std::string path_root);
	std::pair<bool, std::string> readFile(const std::string& path);
	std::set<std::string> getDirListing(const std::string& index_path);
	std::string getfileExtension(const std::string& filepath);
}
