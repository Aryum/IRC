#include "Channel.hpp"

bool Channel::isOnChannel(Client &c)
{
	ClientChannelStatus status = getClientStatus(c);
	return (status != Status_None && status != Status_Invited);
}

bool Channel::test_senderOnChannel(Client &c)
{
	if (!isOnChannel(c))
		return (_server->err_NOTONCHANNEL(c, _name), false);
	return true;
}
bool Channel::test_targetOnChannel(Client &c, Client &tar)
{
	if (!isOnChannel(tar))
		return (_server->err_USERNOTINCHANNEL(c, tar.get(Name_Nick), _name), false);
	return true;
}


bool Channel::test_isOperator(Client &c)
{
	ClientChannelStatus status = getClientStatus(c);
	switch (status)
	{
		case Status_Op:
			return true;
		case Status_Connected:
			return (_server->err_CHANOPRIVSNEEDED(c, _name), false);
		default:
			return (_server->err_NOTONCHANNEL(c, _name), false);
	}
}

bool Channel::test_canChangeTopic(Client &c)
{
	switch (getClientStatus(c))
	{
		case Status_Op:
			return true;
		case Status_Connected:
			if (_topicLocked)
				return (_server->err_CHANOPRIVSNEEDED(c, _name), false);
			return true;
		default:
			return (_server->err_NOTONCHANNEL(c, _name), false);
	}
}
bool Channel::test_canInvite(Client &c)
{
	ClientChannelStatus status = getClientStatus(c);
	if (status == Status_None || status == Status_Invited)
		return (_server->err_NOTONCHANNEL(c, _name), false);
	if (_inviteOnly && status != Status_Op)
		return (_server->err_CHANOPRIVSNEEDED(c, _name), false);
	return true;
}

bool Channel::test_clientCanEnter(Client &c, std::string password)
{
	if (getClientStatus(c) != Status_None)
		return true;
	if (_inviteOnly)
		return (_server->err_INVITEONLYCHAN(c, _name), false);
	if (_passwordVal != "" && password != _passwordVal)
		return (_server->err_BADCHANNELKEY(c, _name), false);
	if (_clientLimit != -1 && _clientCount >= _clientLimit)
		return (_server->err_CHANNELISFULL(c, _name), false);
	return true;
}

bool Channel::test_targetNotOnChannel(Client &c, Client &tar)
{
	ClientChannelStatus status = getClientStatus(tar);
	if (status != Status_None && status != Status_Invited)
		return (_server->err_USERONCHANNEL(c, tar, _name), false);
	return true;
}
