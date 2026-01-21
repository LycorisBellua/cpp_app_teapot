#include "Request.hpp"
#include "Host.hpp"
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
		if (!Host::parseUri(uri_, domain_, port_))
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
		std::vector<std::string> tokens = Helper::splitAtFirstColon(value,
			false);
		domain_ = tokens[0];
		port_ = Host::parsePort(tokens.size() == 1 ? "" : tokens[1], "http");
		if (!Host::isValidDomain(domain_) || port_ < 0)
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
