#pragma once

#include <string>
#include <fstream>
#include <exception>
#include <vector>
#include <map>
#include "ServerData.hpp"

class Config
{
	public:
		Config(const std::string& conf);

		const std::string& getPath() const;
		const std::map<std::string, std::string>& getMime() const;
		const std::vector<ServerData>& getServers() const;

	private:
		// Member Data
		const std::string conf_path;
		std::vector<ServerData> servers;
		std::map<std::string, std::string> mime_types;

		// Parsing State
		enum ParseState { NONE, MIME, SERVER, LOCATION };

		struct ParsingData
		{
			std::ifstream infile;
			std::string line;
			int line_number;
			int nest_level;
			ParseState state;
			std::vector<std::string> tokens;
			std::vector<std::string> server_processed;
			std::vector<std::string> location_processed;
			ParsingData(const std::string& conf_file);
		};

		// Exception Class
		class ConfigError : public std::exception
		{
			public:
				ConfigError(const std::string msg);
				ConfigError(const ParsingData& data, const std::string msg);
				~ConfigError() throw();
				const char* what() const throw();

			private:
				std::string err_msg;
		};

		// Top Level Parsing
		void parse();
		void handleNoBlock(ParsingData& data);
		void handleMimeBlock(ParsingData& data);
		void handleServerBlock(ParsingData& data);
		void handleLocationBlock(ParsingData& data);

		void parseMime(ParsingData& data);

		enum ServerDirective { PORT, HOST, BODY, ERR, INVALID };
		ServerDirective strToServerDirective(const ParsingData& data);
		void parseServer(ParsingData& data);

		enum LocationDirective { LOC, MET, ROOT, IND, AUTOIND, UPLOAD, CGI,
			REDIR, INVLD };
		LocationDirective strToLocationDirective(const ParsingData& data);
		void parseLocation(ParsingData& data);

		// Block Open/Close
		Config::ParseState validateBlockOpen(ParsingData& data);
		bool validateBlockClose(ParsingData& data);

		// Validate and set
		void setPort(const ParsingData& data);
		void setHost(const ParsingData& data);
		void setBodySize(const ParsingData& data);
		void setErrorPage(const ParsingData& data);
		void setPath(const ParsingData& data);
		void setMethods(const ParsingData& data);
		void setRoot(const ParsingData& data);
		void setIndex(const ParsingData& data);
		void setAutoIndex(const ParsingData& data);
		void setUploadPath(const ParsingData& data);
		void setCgi(const ParsingData& data);
		void setRedirect(const ParsingData& data);

		// Final Verification
		void verifyRequiredData();
		void verifyServer(const ServerData& srv) const;
		void verifyLocation(const LocationData& loc) const;
		void verifyPortHostPairs() const;
		void normalisePaths();
		void setDefaultMime();
};
