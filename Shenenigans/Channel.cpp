#include "Channel.hpp"

ClientStatus Channel::getClientStatus(Client &client)
{
	std::map<Client*, ClientStatus>::iterator it = _clients.begin();
	while (it != _clients.end())
	{
		if (&client == it->first)
			return it->second;
		it++;
	}
	return None;
}


//maybbe just return an int that will be read by the map reply
std::string Channel::joinClient(Client &client, std::string password)
{
	ClientStatus status = getClientStatus(client);
	switch (status)
	{
		case Banned:
			return ; //ERR_BANNEDFROMCHAN
		case None:
			break;
		default:
			return;
	}
	if (_inviteOnly)
	{
		//ERR_INVITEONLYCHAN
	}
	else if (!_passwordSet || password == _passwordVal)
		_clients.insert(std::make_pair(&client, Connected));
	else
	{
		//ERR_BADCHANNELKEY
	}
}

std::string Channel::partClient(Client &client)
{
	ClientStatus status = getClientStatus(client);
	if (status == None)
		return; //ERR_NOTONCHANNEL
	_clients.erase(&client);
}

std::string Channel::kickClient(Client &op, std::string name, std::string reason)
{
	ClientStatus callerStatus = getClientStatus(op);
	//maybe needs to check if op is in the server as well
	if (callerStatus != Op)
		return; //ERR_CHANOPRIVSNEEDED
	std::map<Client*, ClientStatus>::iterator it = _clients.begin();
		while (it != _clients.end())
	{
		if (name == it->first->getUserName())
		{
			if (it->second == Op)
				return ;//op on op violentce
			_clients.erase(it);
			return;
		}
		it++;
	}
	return; //ERR_NOTONCHANNEL
}