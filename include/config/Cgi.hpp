#pragma once

#include "ResponseData.hpp"
#include "RouteInfo.hpp"

namespace Cgi
{
	ResponseData* handle(RouteInfo& data);
	void writeToCgi(RouteInfo& data);
	void readFromCgi(RouteInfo& data);
	ResponseData* reapCgiProcess(RouteInfo& data);
}
