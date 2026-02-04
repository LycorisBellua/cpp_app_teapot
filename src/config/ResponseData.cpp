#include "ResponseData.hpp"
#include "StatusMsg.hpp"
#include "ErrorPage.hpp"

// Empty Response
ResponseData::ResponseData() : code(0)
{
}

// Error (No User Error Pages)
ResponseData::ResponseData(const int error_code)
	: code(error_code),
	code_msg(StatusMsg::get(error_code)),
	content(ErrorPage::get(error_code)),
	content_type("text/html")
{
}

// Error (User Error pages)
ResponseData::ResponseData(const int error_code, const std::map<int, std::string> &user_errors)
	: code(error_code),
	code_msg(StatusMsg::get(error_code)),
	content(ErrorPage::get(error_code, user_errors)),
	content_type("text/html")
{
}

// Success Response
ResponseData::ResponseData(const int return_code, const std::string &content,
		const std::string &content_type)
	: code(return_code),
	code_msg(StatusMsg::get(return_code)),
	content(content),
	content_type(content_type)
{
}

ResponseData::~ResponseData()
{
}
