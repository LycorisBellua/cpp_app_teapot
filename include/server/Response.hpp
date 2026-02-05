#pragma once

#include "Router.hpp"
#include "CookieJar.hpp"
#include "Client.hpp"
#include "RequestData.hpp"

namespace Response
{
	std::string compose(const Router& router, CookieJar* jar, Client& c);
}
