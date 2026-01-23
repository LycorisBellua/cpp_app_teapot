#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Router.hpp"
#include "Client.hpp"
#include "Listener.hpp"
#include "RequestData.hpp"
#include <string>

class Response
{
	public:
		static std::string compose(const Router& router,
			const Listener* listener, Client& c);

	private:
		Response();

		static void checkRequestCookies(const Listener* listener, Client& c,
			std::vector<std::string>& cookie_headers);
		static std::string serialize(const ResponseData& res, bool is_head,
			bool should_close, const std::vector<std::string>& cookie_headers);
		static std::string getStartLine(int status, const std::string& msg);
		static std::string getHeaderLine(const std::string& key,
			const std::string& value);
		static std::string getCRLF();
		static std::string getCurrentDateGMT();
};

#endif
