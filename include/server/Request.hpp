#ifndef REQUEST_HPP
#define REQUEST_HPP

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
		std::string getBody() const;

		void outputData() const;
		void resetData();
		void setStatus(int value);
		void appendToBody(const std::string& str);

		void parseStartLine(const std::vector<std::string>& tokens);
		void parseHostHeader(const std::string value);
		void parseContentTypeHeader(const std::string value);
		void parseContentLengthHeader(const std::string value);
		void parseTransferEncodingHeader(const std::string value);
		void parseExpectHeader(const std::string value);
		void parseConnectionHeader(const std::string value);
		void postReadingHeaderCheck();

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
		std::string body_;

		static bool isRecognizedMethod(const std::string& str);
		static bool isRecognizedVersion(const std::string& str);
};

#endif
