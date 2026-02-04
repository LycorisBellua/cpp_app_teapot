#pragma once

#include <string>
#include <set>
#include <map>

struct ResponseData
{
	int code;
	std::string code_msg;
	std::string content;
	std::string content_type;
	std::set< std::pair<std::string, std::string> > headers;

	ResponseData();
	ResponseData(const int error_code);
	ResponseData(const int error_code,
		const std::map<int, std::string>& user_errors);
	ResponseData(const int return_code, const std::string& content,
		const std::string& content_type);
	~ResponseData();
};
