#include <string>
#include <ctime>

struct Client
{
	std::time_t last_activity;
	std::string req_buffer;
	bool header_parsed;
	bool req_fully_parsed;
	std::string method;
	std::string path;
	std::string version;
	std::string host;
	std::string content_type;
	std::string content_length;
	bool chunked;
	bool expect_100;
	bool close_connection;
	std::string body;
};
