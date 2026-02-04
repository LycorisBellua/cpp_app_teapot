#pragma once

#include <sys/wait.h>
#include <string>

struct CgiInfo
{
	bool is_cgi;
	pid_t pid;
	int fd_input;
	int fd_output;
	std::string output;
	std::string script_path;
	std::string script_name;
	std::string path_info;
	std::string path_translated;
	std::string interpreter;

	CgiInfo();
};
