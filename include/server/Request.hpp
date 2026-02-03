#pragma once

#include <string>
#include <vector>

struct Request
{
	public:
		Request();

		int getStatus() const;
		std::string getMethod() const;
		std::string getURI() const;
		std::string getVersion() const;
		std::string getDomain() const;
		int getPort() const;
		std::string getContentType() const;
		size_t getContentLength() const;
		bool getIsChunked() const;
		bool getDoesExpect100() const;
		bool getShouldCloseConnection() const;
		std::vector< std::pair<std::string, std::string> > getCookies() const;
		std::string getBody() const;

		void resetData();
		void setStatus(int value);

		void parseStartLine(const std::vector<std::string>& tokens);
		void parseHostHeader(const std::string value);
		void parseContentTypeHeader(const std::string value);
		void parseContentLengthHeader(const std::string value);
		void parseTransferEncodingHeader(const std::string value);
		void parseExpectHeader(const std::string value);
		void parseConnectionHeader(const std::string value);
		void parseCookie(const std::string value);
		void afterHeaderCheck();
		bool parseRegularBody(std::string& req_buffer);
		bool parseChunkSize(std::string& line, size_t& chunk_size);
		bool parseChunk(std::string& req_buffer, size_t chunk_size);
		void afterBodyCheck();


	private:
		bool start_line_found_;
		bool end_line_found_;
		int status_;
		std::string method_;
		std::string uri_;
		std::string version_;
		bool host_header_found_;
		std::string domain_;
		int port_;
		bool content_type_header_found_;
		std::string content_type_;
		bool content_length_header_found_;
		size_t content_length_;
		bool transfer_encoding_header_found_;
		bool is_chunked_;
		bool expect_header_found_;
		bool does_expect_100_;
		bool connection_header_found_;
		bool should_close_connection_;
		std::vector< std::pair<std::string, std::string> > cookies_;
		std::string body_;

		static bool isRecognizedMethod(const std::string& str);
		static bool isRecognizedVersion(const std::string& str);
		static bool parseUri(std::string& uri, std::string& domain, int& port);
		static bool isOriginForm(const std::string& uri);
		static bool processAbsoluteForm(std::string& uri, std::string& domain,
			int& port);
		static bool setScheme(const std::string& uri, std::string& scheme);
		static bool setPath(const std::string& uri, const std::string& scheme,
			std::string& path);
		static bool setDomainAndPort(const std::string& uri,
			const std::string& scheme, std::string& domain,
			std::string& str_port);
		static int parsePort(const std::string& port,
			const std::string& scheme);
		static bool isValidDomain(const std::string& domain);
};
