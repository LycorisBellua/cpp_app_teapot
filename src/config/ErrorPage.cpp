#include <sstream>
#include "ErrorPage.hpp"
#include "Filesystem.hpp"
#include "StatusMsg.hpp"

namespace
{
	std::string getDefaultErrorPage(const int code);
	std::string getUserErrorPage(const int code, const std::map<int, std::string>& errors);
}

namespace ErrorPage
{
	std::string get(const int code, const std::map<int, std::string>& user_specified)
	{
		const std::string html = getUserErrorPage(code, user_specified);
		if (!html.empty())
			return html;
		return get(code);
	}

	std::string get(const int code)
	{
		return getDefaultErrorPage(code);
	}
}

namespace
{
	typedef std::map<int, std::string>::const_iterator error_it;

	std::string getDefaultErrorPage(const int code)
	{
		std::stringstream html;
		html << "<!DOCTYPE html><html><head><meta charset=\"UTF-8\">"
			<< "<title>Error " << code << "</title>"
			<< "<style>"
			<< "body { font-family: system-ui, -apple-system, sans-serif; max-width: 600px; margin: 50px auto; padding: 20px; }"
			<< "h1 { margin-bottom: 20px; }"
			<< ".error-box { border: 2px solid #ccc; border-radius: 8px; padding: 30px; background: #f9f9f9; text-align: center; }"
			<< ".error-code { font-size: 48px; color: #dc3545; margin: 20px 0; }"
			<< ".error-message { color: #666; }"
			<< "</style></head>"
			<< "<body><h1>Error</h1>"
			<< "<div class=\"error-box\">"
			<< "<div class=\"error-code\">" << code << "</div>"
			<< "<div class=\"error-message\">" << StatusMsg::get(code) << "</div>"
			<< "</div></body></html>";
		return html.str();
	}

	std::string getUserErrorPage(const int code, const std::map<int, std::string>& errors)
	{
		const error_it found = errors.find(code);
		if (found != errors.end())
		{
			std::pair<bool, std::string> html = Filesystem::readFile(found->second);
			if (!html.second.empty())
				return html.second;
		}
		return "";
	}
}
