#include "Delete.hpp"

namespace {

  HttpResponse deleteFile(const RouteResponse& data) {
    if (unlink(data.full_path.c_str()) == -1) {
      if (errno == EACCES || errno == EPERM) {
        Log::error("[DELETE] Incorrect Permissions: " + data.full_path);
        return HttpResponse(403, ErrorPage::get(403, data.error_pages));
      }
      Log::error("[DELETE] Unknown Error: " + data.full_path);
      return HttpResponse(500, ErrorPage::get(500, data.error_pages));
    }
    Log::info("[DELETE] Successfully Deleted: " + data.full_path);
    return HttpResponse(204, "");
  }

}

namespace Delete {

  HttpResponse handle(const RouteResponse& data) {
    if (!Filesystem::exists(data.full_path)) {
      Log::error("[DELETE] Requested file does not exist: " + data.full_path);
      return HttpResponse(404, ErrorPage::get(404, data.error_pages));
    }
    if (Filesystem::isDir(data.full_path)) {
      Log::error("[DELETE] Directory deletion is not allowed: " + data.full_path);
      return HttpResponse(403, ErrorPage::get(403, data.error_pages));
    }
    if (!Filesystem::isRegularFile(data.full_path)) {
      Log::error("[DELETE] Requested file is not a regular file: " + data.full_path);
      return HttpResponse(403, ErrorPage::get(403, data.error_pages));
    }
    return deleteFile(data);
  }

}
