#include <algorithm>
#include "Request.hpp"
#include "Helper.hpp"

/* Public (Instance) -------------------------------------------------------- */

Request::Request()
{
	resetData();
}

int Request::getStatus() const
{
	return status_;
}

std::string Request::getMethod() const
{
	return method_;
}

std::string Request::getURI() const
{
	return uri_;
}

std::string Request::getVersion() const
{
	return version_;
}

std::string Request::getDomain() const
{
	return domain_;
}

int Request::getPort() const
{
	return port_;
}

std::string Request::getContentType() const
{
	return content_type_;
}

size_t Request::getContentLength() const
{
	return content_length_;
}

bool Request::getIsChunked() const
{
	return is_chunked_;
}

bool Request::getDoesExpect100() const
{
	return does_expect_100_;
}

bool Request::getShouldCloseConnection() const
{
	return should_close_connection_;
}

std::vector< std::pair<std::string, std::string> > Request::getCookies() const
{
	return cookies_;
}

std::string Request::getBody() const
{
	return body_;
}

void Request::resetData()
{
	start_line_found_ = false;
	end_line_found_ = false;
	status_ = 0;
	method_ = "";
	uri_ = "";
	version_ = "";
	host_header_found_ = false;
	domain_ = "";
	port_ = 0;
	content_type_header_found_ = false;
	content_type_ = "application/octet-stream";
	content_length_header_found_ = false;
	content_length_ = 0;
	transfer_encoding_header_found_ = false;
	is_chunked_ = false;
	expect_header_found_ = false;
	does_expect_100_ = false;
	connection_header_found_ = false;
	should_close_connection_ = false;
	cookies_.clear();
	body_ = "";
}

void Request::setStatus(int value)
{
	if (value == 400)
		status_ = value;
	else if (status_)
		return;
	if (version_ == "HTTP/1.0" && value == 405)
		value = 403;
	if (value == 100 || value == 200 || value == 201 || value == 202
		|| value == 204 || value == 301 || value == 302 || value == 304
		|| value == 401 || value == 403 || value == 404 || value == 405
		|| value == 500 || value == 501 || value == 502 || value == 503
		|| value == 505)
		status_ = value;
}

void Request::parseStartLine(const std::vector<std::string>& tokens)
{
	if (tokens.size() != 3)
		setStatus(400);
	else
	{
		method_ = tokens[0];
		uri_ = tokens[1];
		version_ = Helper::touppercase(tokens[2]);
		if (!isRecognizedMethod(method_))
			setStatus(501);
		if (!parseUri(uri_, domain_, port_))
			setStatus(400);
		if (!isRecognizedVersion(version_))
			setStatus(505);
		else if (version_ == "HTTP/1.0")
			should_close_connection_ = true;
	}
}

void Request::parseHostHeader(const std::string value)
{
	if (host_header_found_)
		setStatus(400);
	else
	{
		host_header_found_ = true;
		std::vector<std::string> tokens = Helper::splitAtFirstChar(value, ':',
			false);
		domain_ = tokens[0] == "localhost" ? "127.0.0.1" : tokens[0];
		port_ = parsePort(tokens.size() == 1 ? "" : tokens[1], "http");
		if (!isValidDomain(domain_) || port_ < 0)
			setStatus(400);
	}
}

void Request::parseContentTypeHeader(const std::string value)
{
	if (content_type_header_found_)
		setStatus(400);
	else
	{
		content_type_header_found_ = true;
		content_type_ = value;
	}
}

void Request::parseContentLengthHeader(const std::string value)
{
	if (content_length_header_found_ || transfer_encoding_header_found_)
		setStatus(400);
	else
	{
		content_length_header_found_ = true;
		if (!Helper::decToUnsignedNbr(value, content_length_))
			setStatus(400);
	}
}

void Request::parseTransferEncodingHeader(const std::string value)
{
	if (content_length_header_found_ || transfer_encoding_header_found_)
		setStatus(400);
	else
	{
		transfer_encoding_header_found_ = true;
		if (Helper::insensitiveCmp(value, "chunked"))
			is_chunked_ = true;
		else
			setStatus(400);
	}
}

void Request::parseExpectHeader(const std::string value)
{
	if (version_ == "HTTP/1.0")
		return;
	if (expect_header_found_)
		setStatus(400);
	else
	{
		expect_header_found_ = true;
		if (Helper::insensitiveCmp(value, "100-continue"))
			does_expect_100_ = true;
		else
			setStatus(417);
	}
}

void Request::parseConnectionHeader(const std::string value)
{
	if (connection_header_found_)
		setStatus(400);
	else
	{
		connection_header_found_ = true;
		if (Helper::insensitiveCmp(value, "close"))
			should_close_connection_ = true;
		else if (Helper::insensitiveCmp(value, "keep-alive"))
			should_close_connection_ = false;
		else
			setStatus(400);
	}
}

void Request::parseCookie(const std::string value)
{
	std::vector<std::string> cookies = Helper::splitAtChar(value, ';', false);
	for (size_t i = 0; i < cookies.size(); ++i)
	{
		if (!Helper::isPrintableAscii(cookies[i]))
			continue;
		std::vector<std::string> kvp = Helper::splitAtFirstChar(cookies[i], '=',
			true);
		if (kvp.empty())
			continue;
		else if (kvp.size() == 1)
			kvp.insert(kvp.begin(), "");
		if (kvp[0].empty() && kvp[1].empty())
			continue;
		std::pair<std::string, std::string> c = std::make_pair(kvp[0], kvp[1]);
		if (std::find(cookies_.begin(), cookies_.end(), c) == cookies_.end())
			cookies_.push_back(c);
	}
}

void Request::afterHeaderCheck()
{
	if (version_ == "HTTP/1.1" && !host_header_found_)
		setStatus(400);
	if (status_)
		should_close_connection_ = true;
}

bool Request::parseRegularBody(std::string& req_buffer)
{
	size_t needed_len = content_length_ - body_.length();
	size_t available_len = std::min(needed_len, req_buffer.length());
	std::string to_append = Helper::extractLine(req_buffer, available_len,
		false);
	body_.append(to_append);
	return body_.length() == content_length_;
}

bool Request::parseChunkSize(std::string& line, size_t& chunk_size)
{
	size_t size_end = std::min(line.find(';'), line.length());
	std::string size = Helper::extractLine(line, size_end, false);
	return Helper::hexToUnsignedNbr(size, chunk_size);
}

bool Request::parseChunk(std::string& req_buffer, size_t chunk_size)
{
	if (req_buffer.length() <= chunk_size)
		return false;
	else if (req_buffer.find("\r\n", chunk_size) != chunk_size
		&& req_buffer.find("\n", chunk_size) != chunk_size)
		setStatus(400);
	else
	{
		std::string chunk = Helper::extractLine(req_buffer, chunk_size, true);
		body_.append(chunk);
	}
	return true;
}

void Request::afterBodyCheck()
{
	if (does_expect_100_ && body_.length())
		does_expect_100_ = false;
}

/* Private (Static) --------------------------------------------------------- */

bool Request::isRecognizedMethod(const std::string& str)
{
	return str == "GET" || str == "HEAD" || str == "POST" || str == "DELETE";
}

bool Request::isRecognizedVersion(const std::string& str)
{
	return str == "HTTP/1.0" || str == "HTTP/1.1";
}

bool Request::parseUri(std::string& uri, std::string& domain, int& port)
{
	if (uri.empty())
		return false;
	else if (isOriginForm(uri))
		return true;
	return processAbsoluteForm(uri, domain, port);
}

bool Request::isOriginForm(const std::string& uri)
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

bool Request::processAbsoluteForm(std::string& uri, std::string& domain, int& port)
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
	domain = domain_name == "localhost" ? "127.0.0.1" : domain_name;
	port = nbr_port;
	return true;
}

bool Request::setScheme(const std::string& uri, std::string& scheme)
{
	size_t scheme_end = uri.find("://");
	if (scheme_end == std::string::npos)
		return false;
	scheme = uri.substr(0, scheme_end);
	return scheme == "http" || scheme == "https";
}

bool Request::setPath(const std::string& uri, const std::string& scheme,
	std::string& path)
{
	size_t path_start = uri.find('/', scheme.length() + 3);
	if (path_start == std::string::npos)
		path = "/";
	else
		path = uri.substr(path_start);
	return isOriginForm(path);
}

bool Request::setDomainAndPort(const std::string& uri, const std::string& scheme,
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

int Request::parsePort(const std::string& port, const std::string& scheme)
{
	if (port.empty())
		return scheme == "http" ? 80 : scheme == "https" ? 443 : -1;
	int nbr;
	if (!Helper::decToUnsignedNbr(port, nbr) || nbr > 65535)
		return -1;
	return nbr;
}

bool Request::isValidDomain(const std::string& domain)
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
