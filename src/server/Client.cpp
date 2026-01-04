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
		*/
		bool size_zero_found = false; // TODO: should be var in req_
		bool is_size_line = true; // TODO: should be var in req_
		size_t chunk_size = 0; // TODO: should be var in req_
		while (!body_end_found_)
		{
			if (is_size_line)
			{
				size_t eol = findEndOfLine(req_buffer_);
				if (eol == std::string::npos)
					break;
				std::string line = Helper::extractLine(req_buffer_, eol, true);
				if (line.empty())
				{
					body_end_found_ = true;
					if (!size_zero_found)
						req_.setStatus(400);
				}
				else if (!size_zero_found)
				{
					size_t size_end = std::min(line.find(';'), line.length());
					std::string size = Helper::extractLine(line, size_end,
						false);
					// TODO: Test function
					if (!Helper::hexToUnsignedNbr(size, chunk_size))
					{
						// Function failed
					}
					if (!chunk_size)
						size_zero_found = true;
					else
						is_size_line = false;
				}
			}
			else if (!size_zero_found)
			{
				if (req_buffer_.length() <= chunk_size)
					break;
				if (req_buffer_.find("\r\n", chunk_size) != chunk_size
					&& req_buffer_.find("\n", chunk_size) != chunk_size)
					req_.setStatus(400);
				else
				{
					std::string chunk = Helper::extractLine(req_buffer_,
						chunk_size, true);
					req_.appendToBody(chunk);
				}
				is_size_line = true;
			}
			if (req_.getStatus() == 400)
				body_end_found_ = true;
		}
	}
}
