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
	size_zero_found_ = false;
	is_size_line_ = true;
	chunk_size_ = 0;
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
	if (isFullyParsed())
		updateLastActivity();
	return true;
}

std::string Client::composeResponse() const
{
	if (!req_.getStatus())
	{
		/*
			TODO
			- If the method is HEAD, say it's GET.
			- Fetch the requested resource (static page or CGI), and set the 
			status code accordingly (an error, or 200 or 201 for success).
		*/
	}
	if (req_.getStatus() != 100)
	{
		/*
			TODO
			- If the status code represents an error, an error page needs to be 
			returned.
		*/
	}

	/*
		TODO
		- Compose the response.
	*/
	int res_status = req_.getStatus();
	std::string res_body = "Hello World!";
	std::string res_type = "text/plain";
	bool connection_close = req_.getShouldCloseConnection();
	//
	std::string res;
	res += Response::getStartLine(res_status);
	if (res_status == 100)
		res += Response::getCRLF();
	else
	{
		res += Response::getDateLine();
		res += Response::getContentLengthLine(res_body.length());
		if (!res_body.empty())
			res += Response::getContentTypeLine(res_type);
		if (connection_close)
			res += Response::getConnectionCloseLine();
		res += Response::getCRLF();
		if (req_.getMethod() != "HEAD")
			res += res_body;
	}
	return res;
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
		req_.afterHeaderCheck();
}

void Client::parseBody()
{
	if (!end_line_found_ || body_end_found_)
		return;
	else if (req_.getStatus()
		|| (!req_.getContentLength() && !req_.getIsChunked()))
		body_end_found_ = true;
	else if (!req_.getIsChunked())
		body_end_found_ = req_.parseRegularBody(req_buffer_);
	else
		body_end_found_ = parseChunkedBody();
	if (body_end_found_)
		req_.afterBodyCheck();
}

bool Client::parseChunkedBody()
{
	while (!body_end_found_)
	{
		if (is_size_line_)
		{
			size_t eol = findEndOfLine(req_buffer_);
			if (eol == std::string::npos)
				break;
			std::string line = Helper::extractLine(req_buffer_, eol, true);
			if (line.empty())
			{
				body_end_found_ = true;
				if (!size_zero_found_)
					req_.setStatus(400);
			}
			else if (!size_zero_found_)
			{
				if (!req_.parseChunkSize(line, chunk_size_))
					req_.setStatus(400);
				else if (!chunk_size_)
					size_zero_found_ = true;
				else
					is_size_line_ = false;
			}
		}
		else if (!size_zero_found_)
		{
			if (!req_.parseChunk(req_buffer_, chunk_size_))
				break;
			/*
				TODO
				- Check whether the chunked body is too long (the config file 
				has a property about that).
			*/
			is_size_line_ = true;
		}
		if (req_.getStatus() == 400)
			body_end_found_ = true;
	}
	return body_end_found_;
}
