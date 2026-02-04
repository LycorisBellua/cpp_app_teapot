#pragma once

#include <string>
#include <map>

namespace ErrorPage
{
	std::string get(const int code, const std::map<int, std::string>& user_specified);
	std::string get(const int code);
}
