#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Router.hpp"
#include "Client.hpp"
#include "HttpResponse.hpp"
#include <string>

class Response
{
	public:
		static std::string compose(const Router& router, const Client& c);

	private:
		Response();

		class Adapter
		{
			public:
				int status;
				std::string status_msg;
				std::string body;
				std::string type;
				bool should_close;
				bool is_head;

				Adapter();
				void setFromRouteResponse(const RouteInfo& res);
				void setFromHttpResponse(const HttpResponse& res);
		};

		static std::string serialize(const Response::Adapter& res);
		static std::string getCRLF();
		static std::string getStartLine(int status, const std::string& msg);
		static std::string getVersion();
		static std::string getDateLine();
		static std::string getCurrentDateGMT();
		static std::string getContentLengthLine(size_t length);
		static std::string getContentTypeLine(const std::string& type);
		static std::string getConnectionCloseLine();
};

#endif
