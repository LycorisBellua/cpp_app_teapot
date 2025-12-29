#include "Client.hpp"
#include "Host.hpp"
#include "Helper.hpp"
#include <iostream>
#include <unistd.h>

Client::Client() : fd_(-1), req_buffer_(""), header_parsed_(false),
	body_parsed_(false)
{
	updateLastActivity();
}

Client::Client(int fd) : fd_(fd), req_buffer_(""), header_parsed_(false),
	body_parsed_(false)
{
	updateLastActivity();
}

std::time_t Client::getLastActivity() const
{
	return last_activity_;
}

bool Client::getIsParsed() const
{
	return header_parsed_ && body_parsed_;
}

void Client::updateLastActivity()
{
	last_activity_ = std::time(0);
}

bool Client::parseRequest()
{
	header_parsed_ = false;
	body_parsed_ = false;
	req_.resetRequestData();
	req_start_ = std::time(0);
	bool success = parseHeader() && parseBody();
	updateLastActivity();
	return success;
}

/* Private (Static) --------------------------------------------------------- */

size_t Client::findEndOfLine(const std::string& str)
{
	size_t crlf = str.find("\r\n");
	size_t lf = str.find("\n");
	return std::min(crlf, lf);
}

std::string Client::extractLine(std::string& str, size_t end)
{
	std::string line = str.substr(0, end);
	str.erase(0, end + 1 + (str[end] == '\r'));
	return line;
}

/* Private (Instance) ------------------------------------------------------- */

bool Client::parseHeader()
{
	bool start_line_found = false;
	while (!header_parsed_ && req_.getStatus() != 400)
	{
		size_t end;
		while (!header_parsed_ && req_.getStatus() != 400
				&& (end = findEndOfLine(req_buffer_)) != std::string::npos)
		{
			std::string line = extractLine(req_buffer_, end);
			if (line.empty())
			{
				if (start_line_found)
					header_parsed_ = true;
			}
			else if (!start_line_found)
			{
				start_line_found = true;
				req_.parseStartLine(Helper::splitAtWhitespace(line));
			}
			else
			{
				std::vector<std::string> tokens = Helper::splitAtFirstColon
					(line, true);
				if (Helper::insensitiveCmp(tokens[0], "Host"))
					req_.parseHostHeader(tokens[1]);
				else if (Helper::insensitiveCmp(tokens[0], "Content-Type"))
					req_.parseContentTypeHeader(tokens[1]);
				else if (Helper::insensitiveCmp(tokens[0], "Content-Length"))
					req_.parseContentLengthHeader(tokens[1]);
				else if (Helper::insensitiveCmp(tokens[0], "Transfer-Encoding"))
					req_.parseTransferEncodingHeader(tokens[1]);
				else if (Helper::insensitiveCmp(tokens[0], "Expect"))
					req_.parseExpectHeader(tokens[1]);
				else if (Helper::insensitiveCmp(tokens[0], "Connection"))
					req_.parseConnectionHeader(tokens[1]);
			}
		}
		if (!header_parsed_ && req_.getStatus() != 400
			&& !readMoreRequestData())
			break;
	}
	/*
		TODO
		- Returning false does close the connection without sending a response. 
		If the status code is 400, we want to return a response. The only time 
		where we just abort is if the client itself closed the connection 
		(which is noticed when `read` returns 0) or on timeout.
		- Otherwise, of course the body isn't parsed if the status code is 400. 
		Or maybe if the status is set, even?
	*/
	/*
		TODO
		- Test that the status code isn't set if unrecognized headers are used.
		- Test that 400 is returned if a header (that I recognize and therefore 
		don't ignore) appears more than once.
		- Test all headers, even to put a space in between the domain and port 
		within the Host header value.
	*/
	if (!header_parsed_)
		return false;
	req_.postReadingHeaderCheck();
	return true;
}

bool Client::parseBody()
{
	/*
		TODO
		- Test with a POST request (non-chunked).
		- Handle chunked body, and check whether the chunked body is too long 
		(the config file has a property about that).
		- Once you handle chunked requests, use the intra's testers to check 
		your implementation.
	*/
	body_parsed_ = true;
	return body_parsed_;
}

bool Client::readMoreRequestData()
{
	if (isRequestTooSlow())
		return false;
	char buffer[1024];
	ssize_t nread = read(fd_, buffer, sizeof(buffer));
	if (!nread) // Client closed the connection
		return false;
	else if (nread > 0)
		req_buffer_.append(buffer, nread);
	// `nread < 0` -> Not an error / Currently nothing to read, try again later
	return true;
}

bool Client::isRequestTooSlow() const
{
	/*
		TODO
		- Check that the timeout actually works by sending in a slow request. 
		Because the browser is too fast to trigger it.
		- Also, which error should the response contain to express a request 
		timeout?
	*/
	std::time_t now = std::time(0);
	if (now - req_start_ > 5)
	{
		std::cerr << "Error: Client: Request timeout" << std::endl;
		return true;
	}
	return false;
}
