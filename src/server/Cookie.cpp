#include "Cookie.hpp"
#include "Helper.hpp"

Cookie::Cookie(const std::string& key, const std::string& value,
	const std::string& domain, const std::string& path)
	: creation_time_(std::time(0)),
	key_(key), value_(value), domain_(domain), path_(path),
	has_max_age_(false), max_age_(0),
	has_expiration_date_(false), expiration_date_(0)
{
}

Cookie::Cookie(const std::string& key, const std::string& value,
	const std::string& domain, const std::string& path,
	int max_age)
	: creation_time_(std::time(0)),
	key_(key), value_(value), domain_(domain), path_(path),
	has_max_age_(true), max_age_(max_age),
	has_expiration_date_(false), expiration_date_(0)
{
}

Cookie::Cookie(const std::string& key, const std::string& value,
	const std::string& domain, const std::string& path,
	std::time_t expiration_date)
	: creation_time_(std::time(0)),
	key_(key), value_(value), domain_(domain), path_(path),
	has_max_age_(false), max_age_(0),
	has_expiration_date_(true), expiration_date_(expiration_date)
{
}

std::string Cookie::getKey() const
{
	return key_;
}

std::string Cookie::getValue() const
{
	return value_;
}

bool Cookie::hasExpired() const
{
	if (has_max_age_)
		return std::time(0) > (creation_time_ + max_age_);
	else if (has_expiration_date_)
		return std::time(0) > expiration_date_;
	return false;
}

std::string Cookie::getSetCookieValue() const
{
	std::string str = key_ + "=" + value_;
	if (has_max_age_)
		str += "; Max-Age=" + Helper::nbrToString(max_age_);
	else if (has_expiration_date_)
		str += "; Expires=" + Helper::getDateRFC(expiration_date_);
	if (!domain_.empty())
		str += "; Domain=" + domain_;
	if (!path_.empty())
		str += "; Path=" + path_;
	return str;
}
