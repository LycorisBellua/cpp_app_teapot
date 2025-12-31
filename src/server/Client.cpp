#include "Client.hpp"
#include "Host.hpp"
#include "Helper.hpp"
#include <iostream>
#include <unistd.h>

Client::Client() : fd_(-1), req_buffer_("")
{
	resetParsingData();
	updateLastActivity();
}

Client::Client(int fd) : fd_(fd), req_buffer_("")
{
	resetParsingData();
	updateLastActivity();
}

std::time_t Client::getLastActivity() const
{
	return last_activity_;
}

bool Client::isFullyParsed() const
{
	return body_end_found_;
}

void Client::updateLastActivity()
{
	last_activity_ = std::time(0);
}

void Client::resetParsingData()
{
	start_line_found_ = false;
	end_line_found_ = false;
	body_end_found_ = false;
	req_.resetData();
}

bool Client::parseRequest()
{
	if (isFullyParsed())
		return true;
	else if (!readMoreRequestData())
		return false;
	parseHeader();
	parseBody();
	if (body_end_found_)
		updateLastActivity();
	return true;
}

/* Private (Static) --------------------------------------------------------- */

size_t Client::findEndOfLine(const std::string& str)
{
	size_t crlf = str.find("\r\n");
	size_t lf = str.find("\n");
	return std::min(crlf, lf);
}

/* Private (Instance) ------------------------------------------------------- */

bool Client::readMoreRequestData()
{
	char buffer[1024];
	ssize_t nread = read(fd_, buffer, sizeof(buffer));
	if (!nread) // Client closed the connection
		return false;
	else if (nread > 0)
		req_buffer_.append(buffer, nread);
	// `nread < 0` -> Not an error / Currently nothing to read, try again later
	return true;
}

void Client::parseHeader()
{
	if (end_line_found_)
		return;
	size_t eol;
	while (!end_line_found_
		&& (eol = findEndOfLine(req_buffer_)) != std::string::npos)
	{
		std::string line = Helper::extractLine(req_buffer_, eol, true);
		if (line.empty())
		{
			if (start_line_found_)
				end_line_found_ = true;
		}
		else if (!start_line_found_)
		{
			start_line_found_ = true;
			updateLastActivity();
			req_.parseStartLine(Helper::splitAtWhitespace(line));
		}
		else
		{
			std::vector<std::string> tokens = Helper::splitAtFirstColon
				(line, true);
			if (tokens.size() != 2 || tokens[0].empty() || tokens[1].empty())
				req_.setStatus(400);
			else if (Helper::insensitiveCmp(tokens[0], "Host"))
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
		if (req_.getStatus() == 400)
			end_line_found_ = true;
	}
	if (end_line_found_)
		req_.postReadingHeaderCheck();
}

void Client::parseBody()
{
	/*
		TODO
		- Test the regular body.
		- Test the chunked body yourself, and also using intra testers.
	*/
	if (!end_line_found_)
		return;
	else if (req_.getStatus()
		|| (!req_.getContentLength() && !req_.getIsChunked()))
		body_end_found_ = true;
	else if (!req_.getIsChunked())
	{
		size_t needed_len = req_.getContentLength() - req_.getBody().length();
		size_t available_len = std::min(needed_len, req_buffer_.length());
		std::string to_append = Helper::extractLine(req_buffer_, available_len,
			false);
		req_.appendToBody(to_append);
		if (req_.getBody().length() == req_.getContentLength())
			body_end_found_ = true;
	}
	else
	{
		/*
			TODO
			- Handle the chunked body.
			- Check whether the chunked body is too long (the config file has a 
			property about that).

			Example of chunked request (body is "0123456789abcde"):
			```
			POST /upload HTTP/1.1\r\n
			Host: example.com\r\n
			Content-Type: text/plain\r\n
			Transfer-Encoding: chunked\r\n
			\r\n
			A\r\n
			0123456789\r\n
			5\r\n
			abcde\r\n
			0;note="final chunk"\r\n
			X-Checksum: sha256:abcd1234\r\n
			X-Meta: part=7\r\n
			\r\n
			```

			Each chunk is prefaced with the byte size in hex format. This value 
			can be followed by a semicolon, and extra parameters. Ignore 
			everything in between the byte size value and CRLF.

			A final size line of 0 is given at the end to indicate that the 
			chunks have all been sent.

			In between this final size line and the final CRLF line, trailer 
			lines can be found. Ignore them.
		*/
		//body_end_found_ = true;
	}
}
