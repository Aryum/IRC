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


bool Channel::checkMultiple(const Client &c, Check *checks)
{
	for (size_t i = 0; checks[i] != NULL; i++)
	{
		if (!(this->*checks[i])(c))
			return(false);
	}
	return true;
}

bool Channel::check_isOnChannel(const Client &c)
{
	if (getClientStatus(c) == Status_None)
		return (err_NOTONCHANNEL(c, _name), false);
	return true;
}


bool Channel::check_isOperator(const Client &c)
{
	ClientChannelStatus status = getClientStatus(c);
	switch (status)
	{
		case Status_None:
			return (err_NOTONCHANNEL(c, _name), false);
		case Status_Connected:
			return (err_CHANOPRIVSNEEDED(c, _name), false);
		default:
			return true;
	}
}
bool Channel::check_fullChannel(const Client &c)
{
	if (_clientLimit == _clients.size())
		return (err_CHANNELISFULL(c, _name), false);
	return true;


}

bool Channel::check_canInvite(const Client &c)
{
	ClientChannelStatus status = getClientStatus(c);
	if (status == Status_None)
		return (err_NOTONCHANNEL(c, _name), false);
	if (_inviteOnly && status != Status_Op)
		return (err_CHANOPRIVSNEEDED(c, _name), false);
	return true;
}

bool Channel::check_key(const Client &c)
{
	if (_passwordVal != "" && _clientPassword != _passwordVal)
		return (err_BADCHANNELKEY(c, _name), false);
	return true;
}

