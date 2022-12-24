
#include "ServerData.hpp"

/*
 * Constructors && Destructors
 */

ServerData::ServerData(void)
	: host("0.0.0.0"), port(80), autoindex(false), limitSize(1), redirect(301, "")
{
}

ServerData::~ServerData(void)
{
}

LocationData::LocationData(const ServerData& ser)
	: acceptedMeths(ser.acceptedMeths), autoindex(ser.autoindex),
	limitSize(ser.limitSize), root(ser.root), index(ser.index),
	statusCode(ser.statusCode), redirect(301, ""), flag(false)
{
}

LocationData::~LocationData(void)
{
}
