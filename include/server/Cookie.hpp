#pragma once

#include <ctime>
#include <string>

class Cookie
{
	public:
		explicit Cookie(const std::string& key, const std::string& value,
			const std::string& domain, const std::string& path);
		explicit Cookie(const std::string& key, const std::string& value,
			const std::string& domain, const std::string& path,
			int max_age);
		explicit Cookie(const std::string& key, const std::string& value,
			const std::string& domain, const std::string& path,
			std::time_t expiration_date);

		std::string getKey() const;
		std::string getValue() const;
		bool hasExpired() const;
		std::string getSetCookieValue() const;

	private:
		std::time_t creation_time_;
		std::string key_;
		std::string value_;
		std::string domain_;
		std::string path_;
		bool has_max_age_;
		int max_age_;
		bool has_expiration_date_;
		std::time_t expiration_date_;
};
