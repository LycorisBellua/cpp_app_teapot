#include "RequestData.hpp"

RequestData::RequestData(int client_fd, int error_code, int port, const std::string &host, const std::string &uri,
		const std::string& client_ip, const std::string& protocol,
		const std::string &method, const std::string &content_type,
		const std::string &body)
	: client_fd(client_fd),
	error_code(error_code),
	port(port),
	host(host),
	uri(uri),
	client_ip(client_ip),
	protocol(protocol),
	method(method),
	content_type(content_type),
	body(body)
{
}
