#include <errno.h>
#include <unistd.h>
#include "Delete.hpp"
#include "Filesystem.hpp"
#include "Log.hpp"

namespace
{
	ResponseData deleteFile(const RouteInfo& data);
}

namespace Delete
{
	ResponseData handle(const RouteInfo& data)
	{
		if (!Filesystem::exists(data.full_path))
		{
			Log::error("[DELETE] Requested file does not exist: "
				+ data.full_path);
			return ResponseData(404, data.server.errors);
		}
		if (Filesystem::isDir(data.full_path))
		{
			Log::error("[DELETE] Directory deletion is not allowed: "
				+ data.full_path);
			return ResponseData(403, data.server.errors);
		}
		if (!Filesystem::isRegularFile(data.full_path))
		{
			Log::error("[DELETE] Requested file is not a regular file: "
				+ data.full_path);
			return ResponseData(403, data.server.errors);
		}
		return deleteFile(data);
	}
}

namespace
{
	ResponseData deleteFile(const RouteInfo& data)
	{
		if (unlink(data.full_path.c_str()) == -1)
		{
			if (errno == EACCES || errno == EPERM)
			{
				Log::error("[DELETE] Incorrect Permissions: " + data.full_path);
				return ResponseData(403, data.server.errors);
			}
			Log::error("[DELETE] Unknown Error: " + data.full_path);
			return ResponseData(500, data.server.errors);
		}
		Log::info("[DELETE] Successfully Deleted: " + data.full_path);
		return ResponseData(204, "", "");
	}
}
