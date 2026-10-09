#include "placeholder.hpp"

void placeholder::createMap()
{
	_func["PASS"] = cmd_PASS;
	_func["NICK"] = cmd_NICK;
	_func["USER"] = cmd_USER;
	_func["QUIT"] = cmd_QUIT;
	_func["JOIN"] = cmd_JOIN;
	_func["KICK"] = cmd_KICK;
	_func["INVITE"] = cmd_INVITE;
	_func["TOPIC"] = cmd_TOPIC;
	_func["MODE"] = cmd_MODE;

}