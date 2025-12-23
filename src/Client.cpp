#include "Client.hpp"
#include <iostream>
#include <sstream>
#include <unistd.h>

Client::Client() : fd_(-1)
{
	update_last_activity();
}

Client::Client(int fd) : fd_(fd)
{
	update_last_activity();
}

std::time_t Client::get_last_activity() const
{
	return last_activity_;
}

bool Client::get_is_parsed() const
{
	return header_parsed_ && body_parsed_;
}

void Client::update_last_activity()
{
	last_activity_ = std::time(0);
}

void Client::reset_req_data()
{
	header_parsed_ = false;
	body_parsed_ = false;
	status_ = 0;
	method_ = std::string();
	uri_ = std::string();
	version_ = std::string();
	host_ = std::string();
	content_type_ = std::string();
	content_length_ = std::string();
	chunked_ = false;
	expect_100_ = false;
	close_connection_ = false;
	body_ = std::string();
}

bool Client::parse_request()
{
	req_start_ = std::time(0);
	bool success = parse_header() && parse_body();
	update_last_activity();
	return success;
}

/* Private (Static) --------------------------------------------------------- */

size_t Client::find_end_of_line(const std::string& str)
{
	size_t crlf = str.find("\r\n");
	size_t lf = str.find("\n");
	return std::min(crlf, lf);
}

std::string Client::extract_line(std::string& str, size_t end)
{
	std::string line = str.substr(0, end);
	str.erase(0, end + 1 + (str[end] == '\r'));
	return line;
}

std::vector<std::string> Client::split_at_whitespace(const std::string& str)
{
	std::vector<std::string> tokens;
	std::istringstream iss(str);
	std::string token;
	while (iss >> token)
		tokens.push_back(token);
	return tokens;
}

std::vector<std::string> Client::split_at_colon(const std::string& str)
{
	std::vector<std::string> tokens;
	std::istringstream iss(str);
	std::string token;
	while (std::getline(iss, token, ':'))
		tokens.push_back(tolowercase(trim_whitespaces(token)));
	return tokens;
}

std::string Client::trim_whitespaces(const std::string& str)
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

bool Client::is_recognized_method(const std::string& str)
{
	return str == "GET" || str == "HEAD" || str == "POST" || str == "DELETE";
}

bool Client::is_recognized_version(const std::string& str)
{
	return str == "HTTP/1.0" || str == "HTTP/1.1";
}

/* Private (Instance) ------------------------------------------------------- */

bool Client::parse_header()
{
	bool start_line_found = false;
	while (!header_parsed_)
	{
		size_t end;
		while (!header_parsed_
				&& (end = find_end_of_line(req_buffer_)) != std::string::npos)
		{
			std::string line = extract_line(req_buffer_, end);
			if (line.empty())
			{
				if (start_line_found)
					header_parsed_ = true;
			}
			else if (!start_line_found)
			{
				std::vector<std::string> tokens = split_at_whitespace(line);
				if (tokens.size() != 3)
					status_ = 400;
				else
				{
					method_ = tokens[0];
					uri_ = tokens[1];
					version_ = tokens[2];
					if (is_recognized_method(method_))
						status_ = 501;
					else if (is_recognized_version(version_))
						status_ = 505;
				}
				start_line_found = true;
			}
			else
			{
				/*
					TODO: Parse ordinary header line

					- Split at colon. We need exactly two elements. Except if 
					it's "Host", due to the port.
						-> Status code 400.
					- In each element, remove leading and trailing whitespaces.
					- The first element is the name. If you don't recognize it, 
					ignore the line.
					- If the value is odd, which status code to return?
						-> If need be, set the status code to 400.
						-> If you want another status code, only set it if it 
						was still 0, otherwise leave it alone.
				*/
				std::vector<std::string> tokens = split_at_colon(line);
				if (tokens.size() < 2
					|| (tokens[0] != "host" && tokens.size() > 2))
					status_ = 400;
			}
			/*
				TODO
				- If anything is missing, set the appropriate status code.
				- If it's all good, also set the appropriate status code.

				- `Host: example.com:8080\r\n`
					-> Optional if HTTP/1.0
				- (Optional) `Content-Type: text/plain\r\n`
					-> If no Content-Type, default to `text/plain`
				- `Content-Length: 15\r\n` | `Transfer-Encoding: chunked\r\n`
				- (Optional) `Expect: 100-continue`
				- (Optional) `Connection: close` | `Connection: keep-alive`
				- `\r\n`
				- Optional Body
			*/
			/**/std::cout << "[" << line.length() << "]" << line << std::endl;
		}
		if (!header_parsed_ && !read_more_request_data())
			break;
	}
	return header_parsed_;
}

bool Client::parse_body()
{
	/*
		TODO
		- Test with a POST request (non-chunked).
		- Check whether the body is too long (the config file has a property 
		about that).
		- Handle chunked body.
	*/
	body_parsed_ = true;
	return body_parsed_;
}

bool Client::read_more_request_data()
{
	if (is_request_too_slow())
		return false;
	char buffer[1024];
	ssize_t nread = read(fd_, buffer, sizeof(buffer));
	if (nread < 0)
	{
		std::cerr << "Error: Client: read_more_request_data" << std::endl;
		return false;
	}
	else if (nread > 0)
		req_buffer_.append(buffer, nread);
	return true;
}

bool Client::is_request_too_slow() const
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
