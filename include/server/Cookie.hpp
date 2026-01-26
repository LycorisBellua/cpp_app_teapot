#ifndef COOKIE_HPP
#define COOKIE_HPP

#include "Listener.hpp"
#include "Client.hpp"
#include <vector>

class Cookie
{
	public:
		static void checkRequestCookies(const Listener* listener, Client& c,
			std::vector<std::string>& cookie_headers);
		static void generateCookieIfMissing(Listener* listener, Client& c,
			std::vector<std::string>& cookie_headers);
		static void embedBackgroundColor(const std::string& color,
			std::string& html);
	
	private:
		static size_t findBodyTag(const std::string& html);
		static size_t insertStyleAttributeIfMissing(std::string& html,
			size_t body_tag);
};

#endif
