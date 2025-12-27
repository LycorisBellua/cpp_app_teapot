#include "Client.hpp"
#include "Host.hpp"
#include <iostream>
#include <sstream>
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

void Client::resetRequestData()
{
	header_parsed_ = false;
	body_parsed_ = false;
	req_.status = 0;
	req_.method = std::string();
	req_.uri = std::string();
	req_.version = std::string();
	req_.host = std::string();
	req_.content_type = std::string();
	req_.content_length = 0;
	req_.chunked = false;
	req_.expect_100 = false;
	req_.close_connection = false;
	req_.body = std::string();
}

bool Client::parseRequest()
{
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

std::vector<std::string> Client::splitAtWhitespace(const std::string& str)
{
	std::vector<std::string> tokens;
	std::istringstream iss(str);
	std::string token;
	while (iss >> token)
		tokens.push_back(token);
	return tokens;
}

std::vector<std::string> Client::splitAtColon(const std::string& str)
{
	std::vector<std::string> tokens;
	std::istringstream iss(str);
	std::string token;
	while (std::getline(iss, token, ':'))
		tokens.push_back(tolowercase(trimWhitespaces(token)));
	return tokens;
}

std::string Client::trimWhitespaces(const std::string& str)
{
    size_t first = str.find_first_not_of(" \t\n\v\f\r");
    if (first == std::string::npos)
        return str;
    size_t last = str.find_last_not_of(" \t\n\v\f\r");
    return str.substr(first, last - first + 1);
}

std::string Client::tolowercase(const std::string& str)
{
	std::string res = str;
	for (size_t i = 0; i < res.length(); ++i)
		res[i] = std::tolower(static_cast<unsigned char>(res[i]));
	return res;
}

bool Client::isRecognizedMethod(const std::string& str)
{
	return str == "GET" || str == "HEAD" || str == "POST" || str == "DELETE";
}

bool Client::isRecognizedVersion(const std::string& str)
{
	return str == "HTTP/1.0" || str == "HTTP/1.1";
}

/* Private (Instance) ------------------------------------------------------- */

bool Client::parseHeader()
{
	bool start_line_found = false;
	bool host_header_found = false;
	bool content_length_header_found = false;
	bool transfer_encoding_header_found = false;
	bool expect_header_found = false;
	bool connection_header_found = false;
	while (!header_parsed_)
	{
		size_t end;
		while (!header_parsed_
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
				std::vector<std::string> tokens = splitAtWhitespace(line);
				if (tokens.size() != 3)
					req_.status = 400;
				else
				{
					req_.method = tokens[0];
					req_.uri = tokens[1];
					req_.version = tokens[2];
					if (!Host::parseUri(req_.uri, req_.host, req_.port))
						req_.status = 400;
					if (!isRecognizedMethod(req_.method))
						req_.status = 501;
					else if (!isRecognizedVersion(req_.version))
						req_.status = 505;
				}
				start_line_found = true;
			}
			else if (req_.status != 400)
			{
				std::vector<std::string> tokens = splitAtColon(line);
				if (tokens[0] == "host")
				{
					host_header_found = true;
					if (tokens.size() != 2 && tokens.size() != 3)
						req_.status = 400;
					else
					{
						req_.host = tokens[1];
						req_.port = Host::parsePort(tokens.size() == 2 ? ""
							: tokens[2], "http");
						if (!Host::isValidDomain(req_.host) || req_.port < 0)
							req_.status = 400;
					}
				}
				else if (tokens.size() != 2)
					req_.status = 400;
				else if (tokens[0] == "content-type")
				{
					if (!req_.content_type.empty())
						req_.status = 400;
					// TODO: Parse Content-Type
				}
				else if (tokens[0] == "content-length")
				{
					if (content_length_header_found
						|| transfer_encoding_header_found)
						req_.status = 400;
					content_length_header_found = true;
					// TODO: Parse Content-Length
				}
				else if (tokens[0] == "transfer-encoding")
				{
					if (content_length_header_found
						|| transfer_encoding_header_found)
						req_.status = 400;
					transfer_encoding_header_found = true;
					// TODO: Parse Transfer-Encoding
				}
				else if (tokens[0] == "expect")
				{
					if (expect_header_found)
						req_.status = 400;
					expect_header_found = true;
					// TODO: Parse Expect
				}
				else if (tokens[0] == "connection")
				{
					if (connection_header_found)
						req_.status = 400;
					connection_header_found = true;
					// TODO: Parse Connection
				}
			}
					/*
						TODO: Parse ordinary header line
						- Each header must appear only once.
							-> 400.
						- The first element is the name. If you don't recognize 
						it, ignore the line.
						- If the value is odd, which status code to return?
							-> If need be, set the status code to 400.
							-> If you want another status code, only set it if 
							it was still 0, otherwise leave it alone.
					*/
			/*
				TODO
				- If anything is missing, set the appropriate status code.
				- If it's all good, also set the appropriate status code.

				- [x] "Host" is optional if HTTP/1.0.
				- [ ] (Optional) `Content-Type: text/plain`.
						-> If no Content-Type, default to `text/plain`.
				- [ ] `Content-Length: 15` | `Transfer-Encoding: chunked`.
						-> Only one or the other. Both means 400.
				- [ ] (Optional) `Expect: 100-continue`.
				- [ ] (Optional) `Connection: close` | `Connection: keep-alive`.
				- `\r\n`.
				- Optional Body.
			*/
			(void)host_header_found;
			/**/std::cout << "[" << line.length() << "]" << line << std::endl;
		}
		if (!header_parsed_ && !readMoreRequestData())
			break;
	}
	return header_parsed_;

	/*
		TODO
		- Use the config struct to tell whether the host and port are valid. If 
		these values are set, but they do not match anything we have in store, 
		then return 404.
		- If the values are not set (empty string and 0 for the port), then use 
		the default host. If no default host had been indicated by the config 
		file, then use the first host.
		- Note that a port of -1 is invalid. Do not proceed. If unset, it's 0.
	*/
}

bool Client::parseBody()
{
	/*
		TODO
		- Test with a POST request (non-chunked).
		- Check whether the body is too long (the config file has a property 
		about that).
		- Handle chunked body.
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
	if (nread < 0)
	{
		std::cerr << "Error: Client: readMoreRequestData" << std::endl;
		return false;
	}
	else if (nread > 0)
		req_buffer_.append(buffer, nread);
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
