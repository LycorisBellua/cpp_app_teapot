#include "ServerData.hpp"

ServerData::ServerData() : port(0), client_body_max(1000000)
{
}

ServerData::ServerData(const ServerData& src)
	: port(src.port), host(src.host), name(src.name),
	client_body_max(src.client_body_max), errors(src.errors),
	locations(src.locations)
{
}

ServerData::~ServerData()
{
}

ServerData& ServerData::operator=(const ServerData& src)
{
	if (this != &src)
	{
		this->port = src.port;
		this->host = src.host;
		this->name = src.name;
		this->client_body_max = src.client_body_max;
		this->errors = src.errors;
		this->locations = src.locations;
	}
	return *this;
}
