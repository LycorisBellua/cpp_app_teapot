#include "Request.hpp"
#include "Host.hpp"
#include "Helper.hpp"
#include <iostream>

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

void Request::outputData() const
{
	std::cout << std::boolalpha;
	std::cout << "Status: " << getStatus() << std::endl;
	std::cout << "Method: " << getMethod() << std::endl;
	std::cout << "URI: " << getURI() << std::endl;
	std::cout << "Version: " << getVersion() << std::endl;
	std::cout << "Domain: " << getDomain() << std::endl;
	std::cout << "Port: " << getPort() << std::endl;
	std::cout << "Content Type: " << getContentType() << std::endl;
	std::cout << "Content Length: " << getContentLength() << std::endl;
	std::cout << "Is Chunked: " << getIsChunked() << std::endl;
	std::cout << "Does Expect 100: " << getDoesExpect100() << std::endl;
	std::cout << "Should Close Connection: " << getShouldCloseConnection()
		<< std::endl;
	std::cout << std::noboolalpha;
	std::cout << "Body: " << getBody() << std::endl;
}

std::string Request::getStatusMsg(int status) const
{
	if (status == 100)
		return "Continue";
	else if (status == 200)
		return "OK";
	else if (status == 201)
		return "Created";
	else if (status == 202)
		return "Accepted";
	else if (status == 204)
		return "No Content";
	else if (status == 301)
		return "Moved Permanently";
	else if (status == 302)
		return "Moved Temporarily";
	else if (status == 304)
		return "Not Modified";
	else if (status == 400)
		return "Bad Request";
	else if (status == 401)
		return "Unauthorized";
	else if (status == 403)
		return "Forbidden";
	else if (status == 404)
		return "Not Found";
	else if (status == 405)
		return "Method Not Allowed";
	else if (status == 417)
		return "Expectation Failed";
	else if (status == 500)
		return "Internal Server Error";
	else if (status == 501)
		return "Not Implemented";
	else if (status == 502)
		return "Bad Gateway";
	else if (status == 503)
		return "Service Unavailable";
	else if (status == 505)
		return "HTTP Version Not Supported";
	return "";
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
		|| value == 500 || value == 501 || value == 502 || value == 503)
		status_ = value;
}

void Request::appendToBody(const std::string& str)
{
	body_.append(str);
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
		/*
			TODO
			- The config file is needed to check that Content-Type's value is 
			valid. MIME?
			- If invalid, which status code do I return?
		*/
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
		/*
			TODO
			- Check whether Content-Length's value is too long (because the 
			config file has a property about the body size).
			- If invalid, which status code do I return?
		*/
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

void Request::postReadingHeaderCheck()
{
	if (version_ == "HTTP/1.1" && !host_header_found_)
		setStatus(400);
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
