#include "../include/ServerData.hpp"

ServerData::ServerData() : port(0), client_body_max(1000000) {}

LocationData::LocationData() : autoindex(false), redirect(0, "") {}
