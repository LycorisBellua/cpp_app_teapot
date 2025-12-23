#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <string>
#include <vector>
#include <ctime>

class Client
{
	public:
		Client();
		Client(int fd);

		std::time_t get_last_activity() const;
		bool get_is_parsed() const;

		void update_last_activity();
		void reset_req_data();
		bool parse_request();

	private:
		int fd_;
		std::time_t last_activity_;
		std::time_t req_start_;
		std::string req_buffer_;
		bool header_parsed_;
		bool body_parsed_;
		int status_;
		std::string method_;
		std::string uri_;
		std::string version_;
		std::string host_;
		std::string content_type_;
		std::string content_length_;
		bool chunked_;
		bool expect_100_;
		bool close_connection_;
		std::string body_;

		static size_t find_end_of_line(const std::string& str);
		static std::string extract_line(std::string& str, size_t end);
		static std::vector<std::string> split_at_whitespace(const std::string& 
			str);
		static std::vector<std::string> split_at_colon(const std::string& str);
		static std::string trim_whitespaces(const std::string& str);
		static std::string tolowercase(const std::string& str);
		static bool is_recognized_method(const std::string& str);
		static bool is_recognized_version(const std::string& str);

		bool parse_header();
		bool parse_body();
		bool read_more_request_data();
		bool is_request_too_slow() const;
};

#endif
