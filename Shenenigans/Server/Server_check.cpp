#include "Server.hpp"

static bool check(const Client &c, bool condition, const std::string &msg)
{
	if (!condition)
		c.sendReply(msg);
	return condition;
}
