#pragma once

#include "Router.hpp"
#include "CookieJar.hpp"
#include "Client.hpp"
#include "RequestData.hpp"

class Response
{
	public:
		static std::string compose(const Router& router, CookieJar* jar,
			Client& c);

	private:
		Response();

		static void setRouteInfoAndResponseData(const Router& router,
			Client& c);
		static std::string serialize(const ResponseData& res, bool is_head,
			bool should_close, const std::vector<std::string>& cookie_headers);
		static std::string getStartLine(int status, const std::string& msg);
		static std::string getHeaderLine(const std::string& key,
			const std::string& value);
		static std::string getCRLF();
};
