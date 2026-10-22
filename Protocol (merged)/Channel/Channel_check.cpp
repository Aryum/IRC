#include "Channel.hpp"

bool Channel::checkMultiple(Client &c, Check *checks)
{
	for (size_t i = 0; checks[i] != NULL; i++)
	{
		if (!(this->*checks[i])(c))
			return(false);
	}
	return true;
}

bool Channel::check_isOnChannel(Client &c)
{
	if (getClientStatus(c) == Status_None)
		return (_server.err_NOTONCHANNEL(c, _name), false);
	return true;
}


bool Channel::check_isOperator(Client &c)
{
	ClientChannelStatus status = getClientStatus(c);
	switch (status)
	{
		case Status_None:
			return (_server.err_NOTONCHANNEL(c, _name), false);
		case Status_Connected:
			return (_server.err_CHANOPRIVSNEEDED(c, _name), false);
		default:
			return true;
	}
}
bool Channel::check_fullChannel(Client &c)
{
	if (_clientLimit == _clients.size())
		return (_server.err_CHANNELISFULL(c, _name), false);
	return true;
}

bool Channel::check_canInvite(Client &c)
{
	ClientChannelStatus status = getClientStatus(c);
	if (status == Status_None)
		return (_server.err_NOTONCHANNEL(c, _name), false);
	if (_inviteOnly && status != Status_Op)
		return (_server.err_CHANOPRIVSNEEDED(c, _name), false);
	return true;
}

bool Channel::check_key(Client &c)
{
	if (_passwordVal != "" && _clientPassword != _passwordVal)
		return (_server.err_BADCHANNELKEY(c, _name), false);
	return true;
}

bool Channel::isInviteOnly()
{
	return _inviteOnly;
}