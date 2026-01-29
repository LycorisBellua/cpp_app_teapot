#ifndef COOKIE_JAR_HPP
#define COOKIE_JAR_HPP

#include "Cookie.hpp"
#include "Client.hpp"
#include <string>
#include <vector>

class CookieJar
{
	public:
		CookieJar(const std::string& ip);

		bool hasThisCookie(const std::string& key, const std::string& value)
			const;

		const Cookie& createBackgroundColorCookie();
		void removeExpiredCookies();

		static void checkRequestCookies(const CookieJar* jar, Client& c,
			std::vector<std::string>& cookie_headers);
		static void generateCookieIfMissing(CookieJar* jar, Client& c,
			std::vector<std::string>& cookie_headers);

	private:
		CookieJar();

		const std::string ip_;
		std::vector<Cookie> cookies_;
};

#endif
