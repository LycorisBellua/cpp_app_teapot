#include "Response.hpp"
#include "Helper.hpp"
#include <ctime>

/* Public (Static) ---------------------------------------------------------- */

std::string Response::compose(const Router& router, const Client& c)
{
	/*
		TODO
		- Client functions we have:
			std::vector< std::pair<std::string, std::string> > getCookies() const;
			std::string getHexBackgroundColor() const;
			bool setHexBackgroundColor(const std::string& str);
		- BackgroundColorCookie functions we have:
			static bool isValidName(const std::string& str);
			static bool isValidValue(const std::string& str);

		- Select the listener.
		- For each cookie, ask whether the listener has a copy of the cookie.
			- If not, add the cookie to a list marked for "deletion" (response 
			headers).
				`Set-Cookie: weirdCookie=value; Max-Age=0; Path=/`
			- If so, then call `c.setBackgroundColor(value)`.
		- If there was no saved value, it means either no cookie was sent or 
		none was valid.
			- Generate a hex code that's not already in use (in uppercase).
			- Store this cookie into the listener's cookie list.
			- Be aware that you'll have to use the `Set-Cookie` response header.
			- Call `c.setBackgroundColor(value)`.
		- Once I receive the response body, if it's an HTML file (doctype is 
		case insensitive), add the background color (<body> might already have 
		a style option).
	*/
	const RequestData& req = c.getRequestData();
	const ResponseData& res = router.handle(req);
	bool is_head = req.method == "HEAD";
	bool should_close = c.shouldCloseConnection() || res.code == 400;
	return Response::serialize(res, is_head, should_close);
}

/* Private (Static) --------------------------------------------------------- */

std::string Response::serialize(const ResponseData& res, bool is_head,
	bool should_close)
{
	std::string str;
	str += Response::getStartLine(res.code, res.code_msg);
	if (res.code == 100)
		str += Response::getCRLF();
	else
	{
		str += getHeaderLine("Date", getCurrentDateGMT());
		str += getHeaderLine("Content-Length",
			Helper::nbrToString(res.content.length()));
		if (!res.content.empty())
			str += getHeaderLine("Content-Type", res.content_type);
		std::set< std::pair<std::string, std::string> >::iterator it;
		for (it = res.headers.begin(); it != res.headers.end(); ++it)
			str += getHeaderLine(it->first, it->second);
		if (should_close)
			str += getHeaderLine("Connection", "close");
		str += Response::getCRLF();
		if (!is_head)
			str += res.content;
	}
	return str;
}

std::string Response::getStartLine(int status, const std::string& msg)
{
	std::string str = "HTTP/1.1";
	str += " ";
	str += Helper::nbrToString(status);
	str += " ";
	str += msg;
	str += getCRLF();
	return str;
}

std::string Response::getHeaderLine(const std::string& key,
	const std::string& value)
{
	if (key.empty() || value.empty())
		return "";
	return key + ": " + value + getCRLF();
}

std::string Response::getCRLF()
{
	return "\r\n";
}

std::string Response::getCurrentDateGMT()
{
    std::time_t now = std::time(0);
    std::tm *gmt = std::gmtime(&now);
    if (!gmt)
		return "";
	char buffer[64] = {0};
	std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", gmt);
	return buffer;
}
