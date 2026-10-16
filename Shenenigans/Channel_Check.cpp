#include "Channel.hpp"

/*
	
	
	
	
	ERR_KEYSET
	ERR_NEEDMOREPARAMS
	ERR_NOSUCHNICK
	
	ERR_UMODEUNKNOWNFLAG
	ERR_UNKNOWNMODE
	ERR_USERONCHANNEL
	ERR_USERSDONTMATCH
*/
static bool check(const Client &c, bool condition, const std::string &msg)
{
	if (!condition)
		c.sendReply(msg);
	return condition;
}

bool Channel::hasError(const Client &c, Check *checks)
{
	for (size_t i = 0; checks[i] != NULL; i++)
	{
		if (!(this->*checks[i])(c))
			return(true);
	}
	return false;
}

bool Channel::isOnChannel(const Client &c)
{
	return check(c, getClientStatus(c) != Status_None, "ERR_NOTONCHANNEL");
}


bool Channel::isOperator(const Client &c)
{
	ClientChannelStatus status = getClientStatus(c);
	if (!check(c, status != Status_None, "ERR_NOTONCHANNEL"))
		return false;
	return check(c, status == Status_Op, "ERR_CHANOPRIVSNEEDED");
}

bool Channel::fullChannel(const Client &c)
{
	return (check(c, _clientLimit != _clients.size(), "ERR_CHANNELISFULL"));
}

bool Channel::isInviteOnly(const Client &c)
{
	return (check(c, !_inviteOnly, "ERR_INVITEONLYCHAN"));
}

bool Channel::checkKey(const Client &c)
{
	return (check(c, _passwordVal == "" || _clientPassword == _passwordVal, "ERR_BADCHANNELKEY"));
}

