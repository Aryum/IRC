#include "Channel.hpp"

//maybbe just return an int that will be read by the map reply
std::string Channel::addClient(Client &client, std::string password)
{
	std::vector<Client*>::iterator it = _bannedClients.begin();
	while (it != _bannedClients.end())
	{
		//check  if this is atually correct
		if (client == **it)
			return ; //ERR_BANNEDFROMCHAN

		it++;
	}
	if (_inviteOnly)
	{
		//ERR_INVITEONLYCHAN
	}
	else if (!_passwordSet || password == _passwordVal)
		_connectedClients.push_back(&client);
	else
	{
		//ERR_BADCHANNELKEY
	}
}

std::string Channel::removeClient(Client &client)
{
	
}