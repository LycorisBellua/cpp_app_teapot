#pragma once

#include "ResponseData.hpp"
#include "RouteInfo.hpp"

namespace Post
{
	ResponseData handle(const RouteInfo& data);
}
