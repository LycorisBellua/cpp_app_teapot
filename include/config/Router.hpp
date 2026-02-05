#pragma once

#include <string>
#include <exception>
#include <vector>
#include <set>
#include <map>
#include "Config.hpp"
#include "Router.hpp"
#include "RouteInfo.hpp"
#include "RequestData.hpp"
#include "LocationData.hpp"
#include "ServerData.hpp"

class Router
{
	public:
		Router(const Config& conf);

		std::set< std::pair<std::string, int> > getPorts() const;
		RouteInfo getRoute(const RequestData& request) const;

	private:
		// Class Data
		const std::vector<ServerData> servers;
		const std::map<std::string, std::string> mime;

		// Exception Class
		class RouterError : public std::exception
		{
			public:
				RouterError(const std::string msg);
				RouterError(const std::string& msg, const RequestData& request);
				~RouterError() throw();
				const char* what() const throw();

			private:
				std::string err_msg;
		};

		const ServerData* getServer(const RequestData& request) const;
		const LocationData* getLocation(
			const std::vector<LocationData>& locations, const std::string& path,
			const RequestData& request) const;
		const std::string getMime(const std::string& path) const;

		const ServerData* serverSearch(const std::string& host, const int port)
			const;
		const ServerData* serverFirstMatchingPort(const int port) const;
		const ServerData* serverFirstMatchingHost(const std::string& host)
			const;
		std::string decodeUri(const std::string& uri_no_query,
			const RequestData& req) const;
		std::string normalizePath(const std::string& path,
			const RequestData& request) const;
		void validMethod(const RequestData& req, const LocationData* location)
			const;
		void verifyBodySize(const RequestData& request,
			const ServerData* server) const;
		RouteInfo errorReturn(int code, const ServerData* srv,
			const RequestData& req) const;
};
