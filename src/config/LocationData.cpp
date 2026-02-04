#include "LocationData.hpp"

LocationData::LocationData() : autoindex(false), redirect(0, "")
{
}

LocationData::LocationData(const LocationData& src)
	: path(src.path),
	allowed_methods(src.allowed_methods),
	root(src.root),
	index(src.index),
	autoindex(src.autoindex),
	upload_path(src.upload_path),
	cgi(src.cgi),
	redirect(src.redirect)
{
}

LocationData::~LocationData()
{
}

LocationData& LocationData::operator=(const LocationData& src)
{
	if (this != &src)
	{
		this->path = src.path;
		this->allowed_methods = src.allowed_methods;
		this->root = src.root;
		this->index = src.index;
		this->autoindex = src.autoindex;
		this->upload_path = src.upload_path;
		this->cgi = src.cgi;
		this->redirect = src.redirect;
	}
	return *this;
}
