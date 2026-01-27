#ifndef LISTENER_HPP
#define LISTENER_HPP

#include "Cookie.hpp"
#include "Client.hpp"
#include <string>
#include <vector>

class Listener
{
	public:
		Listener(int fd, const std::string& ip, int port);

		bool hasThisIP(const std::string& ip) const;
		bool hasThisPort(int port) const;
		bool hasThisCookie(const std::string& key, const std::string& value)
			const;

		Cookie createBackgroundColorCookie();
		void removeExpiredCookies();

		static void checkRequestCookies(const Listener* listener, Client& c,
			std::vector<std::string>& cookie_headers);
		static void generateCookieIfMissing(Listener* listener, Client& c,
			std::vector<std::string>& cookie_headers);

	private:
		Listener();

		const int fd_;
		const std::string ip_;
		const int port_;
		std::vector<Cookie> cookies_;
};

#endif
