#include "../include/ServerData.hpp"

ServerData::ServerData() : port(0), client_body_max(1000000) {}

LocationData::LocationData() : autoindex(false), redirect(0, "") {}

ConfigData::ConfigData(const std::vector<ServerData>& s,
                       const std::map<std::string, std::string>& m)
    : servers(s), mime(m) {}
