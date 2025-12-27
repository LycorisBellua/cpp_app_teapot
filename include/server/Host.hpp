#ifndef HOST_HPP
#define HOST_HPP

#include <string>

class Host
{
	public:
		static bool parseUri(std::string& uri, std::string& host, int& port);
		static bool isValidDomain(const std::string& domain);
		static int parsePort(const std::string& port,
			const std::string& scheme);

	private:
		Host();

	static bool isOriginForm(const std::string& uri);
	static bool processAbsoluteForm(std::string& uri, std::string& host,
		int& port);
	static bool setScheme(const std::string& uri, std::string& scheme);
	static bool setPath(const std::string& uri, const std::string& scheme,
		std::string& path);
	static bool setDomainAndPort(const std::string& uri,
		const std::string& scheme, std::string& domain, std::string& str_port);
};

#endif
