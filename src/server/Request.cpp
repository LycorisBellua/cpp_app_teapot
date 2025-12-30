#include "Request.hpp"
#include "Host.hpp"
#include "Helper.hpp"

Request::Request()
{
	resetRequestData();
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

void Request::resetRequestData()
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
	content_type_ = "text/plain";
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

void Request::parseStartLine(const std::vector<std::string>& tokens)
{
	if (tokens.size() != 3)
		status_ = 400;
	else
	{
		method_ = tokens[0];
		uri_ = tokens[1];
		version_ = Helper::touppercase(tokens[2]);
		if (!Host::parseUri(uri_, domain_, port_))
			status_ = 400;
		else if (!isRecognizedMethod(method_))
			status_ = 501;
		else if (!isRecognizedVersion(version_))
			status_ = 505;
		else if (version_ == "HTTP/1.0")
			should_close_connection_ = true;
	}
}

void Request::parseHostHeader(const std::string value)
{
	if (host_header_found_)
		status_ = 400;
	else
	{
		host_header_found_ = true;
		std::vector<std::string> tokens = Helper::splitAtFirstColon(value,
			false);
		domain_ = tokens[0];
		port_ = Host::parsePort(tokens.size() == 1 ? "" : tokens[1], "http");
		if (!Host::isValidDomain(domain_) || port_ < 0)
			status_ = 400;
	}
}

void Request::parseContentTypeHeader(const std::string value)
{
	if (content_type_header_found_)
		status_ = 400;
	else
	{
		content_type_header_found_ = true;
		content_type_ = value;
		/*
			TODO
			- The config file is needed to check that Content-Type's value is 
			valid. MIME?
			- If invalid, which status code do I return? It could be 400, 
			otherwise only change `this->status` if it was still 0.
		*/
	}
}

void Request::parseContentLengthHeader(const std::string value)
{
	if (content_length_header_found_ || transfer_encoding_header_found_)
		status_ = 400;
	else
	{
		content_length_header_found_ = true;
		if (!Helper::stringToUnsignedNbr(value, content_length_))
			status_ = 400;
		/*
			TODO
			- Check whether Content-Length's value is too long (because the 
			config file has a property about the body size).
			- If invalid, which status code do I return? It could be 400, 
			otherwise only change `this->status` if it was still 0.
		*/
	}
}

void Request::parseTransferEncodingHeader(const std::string value)
{
	if (content_length_header_found_ || transfer_encoding_header_found_)
		status_ = 400;
	else
	{
		transfer_encoding_header_found_ = true;
		if (Helper::insensitiveCmp(value, "chunked"))
			is_chunked_ = true;
		else
			status_ = 400;
	}
}

void Request::parseExpectHeader(const std::string value)
{
	if (version_ == "HTTP/1.0")
		return;
	if (expect_header_found_)
		status_ = 400;
	else
	{
		expect_header_found_ = true;
		if (Helper::insensitiveCmp(value, "100-continue"))
			does_expect_100_ = true;
		else if (!status_)
			status_ = 417;
	}
}

void Request::parseConnectionHeader(const std::string value)
{
	if (connection_header_found_)
		status_ = 400;
	else
	{
		connection_header_found_ = true;
		if (Helper::insensitiveCmp(value, "close"))
			should_close_connection_ = true;
		else if (Helper::insensitiveCmp(value, "keep-alive"))
			should_close_connection_ = false;
		else
			status_ = 400;
	}
}

void Request::postReadingHeaderCheck()
{
	if (version_ == "HTTP/1.1" && !host_header_found_)
		status_ = 400;
	else if (!status_)
	{
		/*
			TODO
			- Use the config struct to tell whether the domain and port are 
			valid. If these values are set, but they do not match anything we 
			have in store, then return 404.
			- If the values are not set (empty string and 0 for the port), then 
			use the default domain. If no default domain had been indicated by 
			the config file, then use the first domain.
		*/
	}
	if (status_)
		should_close_connection_ = true;
}

/* Private (static) --------------------------------------------------------- */

bool Request::isRecognizedMethod(const std::string& str)
{
	return str == "GET" || str == "HEAD" || str == "POST" || str == "DELETE";
}

bool Request::isRecognizedVersion(const std::string& str)
{
	return str == "HTTP/1.0" || str == "HTTP/1.1";
}
