#include <string>
#include <ctime>

struct Client
{
	std::time_t last_activity;
	std::string req_buffer;
};
