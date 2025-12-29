#include "Host.hpp"
#include "Helper.hpp"

bool Host::parseUri(std::string& uri, std::string& domain, int& port)
{
	if (uri.empty())
		return false;
	else if (isOriginForm(uri))
		return true;
	return processAbsoluteForm(uri, domain, port);
}

bool Host::isValidDomain(const std::string& domain)
{
	size_t label_len = 0;
	for (size_t i = 0; i <= domain.length(); ++i)
	{
		if (i == domain.length() || domain[i] == '.')
		{
			if (!label_len || label_len > 63)
				return false;
			label_len = 0;
		}
		else if (domain[i] == '-')
		{
			if (!label_len || domain[i + 1] == '.' || !domain[i + 1])
				return false;
			++label_len;
		}
		else if (std::isalnum(domain[i]))
			++label_len;
		else
			return false;
	}
	return true;
}

int Host::parsePort(const std::string& port, const std::string& scheme)
{
	if (port.empty())
		return scheme == "http" ? 80 : scheme == "https" ? 443 : -1;
	int nbr;
	if (!Helper::stringToUnsignedNbr(port, nbr) || nbr > 65535)
		return -1;
	return nbr;
}

/* Private ------------------------------------------------------------------ */

bool Host::isOriginForm(const std::string& uri)
{
	if (uri[0] != '/')
		return false;
	for (size_t i = 0; i < uri.length(); ++i)
	{
		if (uri[i] == '/' && uri[i + 1] == '/')
			return false;
	}
	return true;
}

bool Host::processAbsoluteForm(std::string& uri, std::string& domain, int& port)
{
	std::string scheme;
	std::string path;
	std::string domain_name;
	std::string str_port;
	if (!setScheme(uri, scheme) || !setPath(uri, scheme, path)
		|| !setDomainAndPort(uri, scheme, domain_name, str_port))
		return false;
	int nbr_port = parsePort(str_port, scheme);
	if (nbr_port < 0)
		return false;
	uri = path;
	domain = domain_name;
	port = nbr_port;
	return true;
}

bool Host::setScheme(const std::string& uri, std::string& scheme)
{
	size_t scheme_end = uri.find("://");
	if (scheme_end == std::string::npos)
		return false;
	scheme = uri.substr(0, scheme_end);
	return scheme == "http" || scheme == "https";
}

bool Host::setPath(const std::string& uri, const std::string& scheme,
	std::string& path)
{
	size_t path_start = uri.find('/', scheme.length() + 3);
	if (path_start == std::string::npos)
		path = "/";
	else
		path = uri.substr(path_start);
	return isOriginForm(path);
}

bool Host::setDomainAndPort(const std::string& uri, const std::string& scheme,
	std::string& domain, std::string& str_port)
{
	domain = uri.substr(scheme.length() + 3,
		uri.find('/', scheme.length() + 3) - (scheme.length() + 3));
	if (domain.empty())
		return false;
	size_t port_start = domain.find(':');
	if (port_start != std::string::npos)
	{
		str_port = domain.substr(port_start + 1);
		if (str_port.find(':') != std::string::npos)
			return false;
		domain = domain.substr(0, port_start);
	}
	return isValidDomain(domain);
}
